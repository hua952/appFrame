#include "impMainLoop.h"
#include "tSingleton.h"
#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "mLog.h"
#include "mArgMgr.h"
#include "impMainLoop.h"
#include "strFun.h"

packetHead* sallocPacket(udword udwS)
{
	auto & mgr = tSingleton<loopMgr>::single().getForLogicFun();
	packetHead* pRet = (packetHead*)(mgr.fnAllocPack(AllPacketHeadSize + udwS));
	pNetPacketHead pN = P2NHead(pRet);
	memset(pN, 0, NetHeadSize);
	pN->udwLength = udwS;
	pRet->pAsk = 0;
	return pRet;
}

packetHead* sallocPacketExt(udword udwS, udword ExtNum)
{
	auto pRet =  sallocPacket (udwS + sizeof(packetHead) * ExtNum);
	if (ExtNum) {
		pNetPacketHead pN = P2NHead(pRet);
		NSetExtPH(pN);
	}
	return pRet;
}

packetHead* sNClonePack(netPacketHead* pN)
{
	udword extNum = NIsExtPH(pN)?1:0;
	auto pRet = sallocPacketExt (pN->udwLength, extNum);
	pRet->pAsk = 0;
	pRet->sessionID = EmptySessionID;
	auto pRN = P2NHead(pRet);
	auto udwLength = pN->udwLength;
	*pRN++ = *pN++;
	if (udwLength ) {
		memcpy (pRN, pN, udwLength);
	}
	return pRet;
}

packetHead* sclonePack(packetHead* p)
{
	auto pN = P2NHead(p);
	auto pRet = sNClonePack(pN);
	pRet->sessionID = p->sessionID;
	return pRet;
}

int onMidLoopBegin(loopHandleType pThis)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopBegin();
	rMgr.onLoopBegin(pThis);
	return nRet;
}

int onMidLoopEnd(loopHandleType pThis)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopEnd();
	rMgr.onLoopEnd(pThis);
	return nRet;
}

int onLoopFrame(loopHandleType pThis)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopFrame();
	return nRet;
}

int processOncePack(loopHandleType pThis, packetHead* pPack)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->processOncePack(pPack);
	return nRet;
}

impLoop::impLoop()//:m_MsgMap(8)
{
	m_funOnFrame = nullptr;
	m_pArg = nullptr;
	m_packSave = nullptr;
	m_packSave = nullptr;
	// m_midTcpServer = nullptr;
	m_id = c_emptyLoopHandle;
	m_serverNode.listenerNum = 0;
	m_serverNode.connectorNum = 0;
	m_frameNum = 0;
	m_defSession = nullptr;
	m_canUpRoute = false;
	m_canDownRoute = false;
}

impLoop::~impLoop()
{
}

int  impLoop:: clonePackToOtherNetThread (packetHead* pack)	
{
    int 	 nRet = 0;
    do {
		auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rCS = rMgr.getPhyCallback();
		const auto c_Max = 16;
		loopHandleType buf[c_Max];
		auto upN = rMgr.getAllCanRouteServerS (buf, c_Max);
		// auto upN = rMgr.upNum ();
		auto myId = id ();
		for (decltype (upN) i = 0; i < upN; i++) {
			auto objSer = buf[i]; // rMgr.getLoopByIndex (i)->id();
			if (myId == objSer) {
				continue;
			}
			auto pS = sclonePack (pack);
			pS->sessionID = EmptySessionID;
			rCS.fnPushPackToLoop (objSer, pS);
		}
    } while (0);
    return nRet;
}

int  impLoop:: processAllGatePack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto pDS =   defSession ();
		auto& rMgr =  tSingleton<loopMgr>::single();
		if (pDS) {
			packetHead* pNew = nullptr;
			auto toNetPack = rMgr.toNetPack ();
			toNetPack (pPack, pNew);
			if (pNew) {
				pDS->send (pNew);  /* 发压缩包 */
				nRet = procPacketFunRetType_del;  /* 发压缩包不需保存原包 */
			} else {
				pDS->send (pPack);
				nRet = procPacketFunRetType_doNotDel;  /* 发原包,不能删 */
			}
		} else {
			/*  本线程就是gate    */
			auto pN = P2NHead(pPack);
			auto pF = findMsg(pN->uwMsgID);
			if (pF) {
				packetHead* pRet = nullptr;
				nRet = pF(pPack, pRet, nullptr);
				// myAssert(!pRet);
				if (pRet) {
					auto fnFree = rMgr.getForLogicFun().fnFreePack;
					fnFree (pRet);
					pRet = nullptr;
					pPack->pAsk = 0;
				}
				if (!NIsOtherNetLoopSend(pN)) {
					NSetOtherNetLoopSend(pN);
					clonePackToOtherNetThread (pPack);
				}
			}
		}
	} while (0);
	return nRet;
}

int  impLoop:: processOtherAppToMePack(ISession* session, packetHead* pPack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rCS = rMgr.getPhyCallback();
		auto pN = P2NHead (pPack);
		auto bIsRet = NIsRet (pN);
		auto pF = findMsg(pN->uwMsgID);
		auto myHandle = id();
		if(!pF) {
			mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			break;
		}
		if (bIsRet) {
			auto pIPackSave = getIPackSave ();
			auto pAskPack = pIPackSave->netTokenPackFind (pN->dwToKen);
			if (!pAskPack) {
				mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
						<<" length = "<<pN->udwLength);
				break;
			}
			myAssert (pAskPack);
			pPack->pAsk = (uqword)pAskPack;  // must for delete pAskPack
			nRet = pF(pAskPack, pPack, nullptr);
			pIPackSave->netTokenPackErase (pN->dwToKen);
			break;
		}
		bool bNeetRet = NNeetRet(pN);
		packetHead* pRet = nullptr;
		nRet = pF(pPack, pRet, nullptr);
		if (pRet) {
			if (bNeetRet) {
				pRet->sessionID = pPack->sessionID;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = pN->ubyDesServId;
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				session->send (pRet);
			} else {
				/* 发送Ask端不想处理Ret */
				auto freeFun = rCS.fnFreePack;
				freeFun (pRet);
				pRet = nullptr;
			}
		}
    } while (0);
    return nRet;
}

int  impLoop:: processOtherAppPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	auto& rCS = tSingleton<loopMgr>::single().getPhyCallback();
	auto pN = P2NHead (pPack);
	auto bIsRet = NIsRet (pN);
	auto freeFun = rCS.fnFreePack;
	do {
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		auto myHandle = id();
		auto pF = findMsg(pN->uwMsgID);
		if(!pF) {
			mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			break;
		}
		if (bIsRet) {
			auto pIPackSave = getIPackSave ();
			auto pAskPack = pIPackSave->netTokenPackFind (pN->dwToKen);
			if (pAskPack) {
				myAssert (pAskPack);
				pPack->pAsk = (uqword)pAskPack;  // must for delete pAskPack
				nRet = pF(pAskPack, pPack, nullptr);
				pIPackSave->netTokenPackErase (pN->dwToKen);
			} else {
				mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
						<<" length = "<<pN->udwLength);
			}
			break;
		} 
		packetHead* pRet = nullptr;
		nRet = pF(pPack, pRet, nullptr);

		if (pRet) {
			bool bNeetRet = NNeetRet(pN);
			if (bNeetRet) {
				myAssert (EmptySessionID != pPack->sessionID && c_emptyLoopHandle != pPack->loopId);
				pRet->sessionID = pPack->sessionID;
				pRet->loopId = pPack->loopId;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = pN->ubyDesServId;
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				rCS.fnPushPackToLoop (pPack->loopId, pRet);
			} else {
				freeFun (pRet);
				pRet = nullptr;
			}
		}
	} while (0);
	return nRet;
}
static bool sDelTokenInfo(void* pUserData)
{
	auto pArgS = (std::pair<impLoop*, NetTokenType>*) pUserData;
	auto pI = pArgS->first->getIPackSave ();
	pI->oldTokenErase(pArgS->second);
	return false;
}
bool isMyChannelMsg (msgIdType	msgId)
{
	return msgId > 60000;
}

ISession*   impLoop:: getProcessSession(ServerIDType pid)
{
    ISession*   nRet = nullptr;
    do {
		auto& rS =   processSessionS ();
		uqword uqwB = pid;
		uqwB <<= 32;
		uqword uqwE = uqwB;
		uqwE |= 0xffffffff;
		std::vector<uword> temp;
		auto itB = rS.lower_bound (uqwB);
		auto itE = rS.upper_bound (uqwE);
		for (auto it = itB; it != itE; it++) {
			temp.push_back((udword)(*it));
		}
		if (temp.empty()) {
			break;
		}
		auto nI = rand () % temp.size ();
		auto nS = temp[nI];
		nRet = getSession (nS);
    } while (0);
    return nRet;
}

ISession*   impLoop:: getProcessSessionByFullServerId(ServerIDType fullId)
{
    ISession*   nRet = 0;
    do {
		loopHandleType objPId;
		loopHandleType objSId;
		fromHandle (fullId, objPId, objSId);
		nRet = getProcessSession (objPId);
    } while (0);
    return nRet;
}

int  impLoop:: sendPackToSomeSession(netPacketHead* pN, uqword* pSessS, udword sessionNum)
{
    int  nRet = 0;
    do {
		for (decltype (sessionNum) i = 0; i < sessionNum; i++) {
			auto uqw = pSessS[i];
			auto seId = (SessionIDType)uqw;
			auto pSe = getSession (seId);
			if (!pSe) {
				continue;
			}
			uqw >>= 32;
			auto loopId = (loopHandleType)uqw;
			auto pSend = sNClonePack(pN);
			auto pSN = P2NHead(pSend);
			pSN->ubyDesServId = loopId;
			pSe->send (pSend);
		}
    } while (0);
    return nRet;
}

int  impLoop:: forwardForOtherServer(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		auto& rMgr = tSingleton<loopMgr>::single ();
		iPackSave* pISave = getIPackSave ();
		auto pN = P2NHead (pPack);
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		myAssert (sPId != myPId); /* 该函数只处理非首站发出 */
		myAssert (NIsOtherNetLoopSend(pN));
		myAssert (pN->inGateSerId!=c_emptyLoopHandle);
		ISession* pSe = nullptr;
		 
		auto isRet = NIsRet (pN);
		if (isRet) {
			auto recToken = pN->dwToKen;
			auto pInfo = pISave->oldTokenFind (recToken);
			myAssert(pInfo);

			pN->dwToKen = pInfo->oldToken;
			myAssert (c_emptyLocalServerId == pInfo->inGateSerId);
			pSe = getSession (pInfo->sessionId); // getServerSession (pInfo->sessionId);
			myAssert (pSe);
			if (pSe) {
				pSe->send (pPack);
			} else {
				nRet = procPacketFunRetType_del;
			}
			pISave->oldTokenErase(recToken);
			break;
		}
		bool neetRet = NNeetRet(pN);
		if (!neetRet) {
			pSe = getServerSession (pN->ubyDesServId);
			if (!pSe) {
				pSe = getProcessSessionByFullServerId (pN->ubyDesServId);
			}
			if (!pSe) {
				pSe = defSession ();
			}
			if (pSe) {
				pSe->send (pPack);
			} else {
				nRet = procPacketFunRetType_del; /* 无法发送,删除包 */ 
			}
			break;
		}
		auto newToken = nextToken ();
		tokenInfo info;
		info.lastServer = myHandle;
		info.oldToken = pN->dwToKen;
		info.sessionId = pPack->sessionID;
		info.inGateSerId = pN->inGateSerId;
		pN->inGateSerId = c_emptyLoopHandle;
		pN->dwToKen = newToken;

		pSe = getServerSession (pN->ubyDesServId);
		if (!pSe) {
			pSe = getProcessSessionByFullServerId (pN->ubyDesServId);
		}
		if (!pSe) {
			pSe = defSession ();
		}
		if (pSe) {
			pSe->send (pPack);
		} else {
			nRet = procPacketFunRetType_del; /* 无法发送,删除包 */ 
			break;
		}
		pISave->oldTokenInsert(newToken, info);
		std::pair<impLoop*, NetTokenType> argS;
		argS.first = this;
		argS.second = newToken;
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto timeOut = rArgS.packTokenTime ();
		auto& rTimeMgr = getTimerMgr ();
		rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
    } while (0);
    return nRet;
}

int  impLoop:: forward(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		auto& rMgr = tSingleton<loopMgr>::single ();
		iPackSave* pISave = getIPackSave ();
		auto pN = P2NHead (pPack);
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		myAssert (sPId != myPId); /* 该函数只处理非首站发出 */
		myAssert (!NIsOtherNetLoopSend(pN));
		ISession* pSe = nullptr;
		if (NIsRet (pN)) {
			// auto sessionId = pPack->sessionID;
			// pSe = getSession (sessionId);
			auto recToken = pN->dwToKen;
			auto pInfo = pISave->oldTokenFind (recToken);
			myAssert(pInfo);

			pN->dwToKen = pInfo->oldToken;
			if (c_emptyLocalServerId == pInfo->inGateSerId) {
				pSe = getSession (pInfo->sessionId); // getServerSession (pInfo->sessionId);
				myAssert (pSe);
				if (pSe) {
					pSe->send (pPack);
				} else {
					nRet = procPacketFunRetType_del;
				}
			} else {
				auto& rCS = rMgr.getPhyCallback();
				auto realSer = toHandle (myPId, pInfo->inGateSerId);
				NSetOtherNetLoopSend(pN);
				rCS.fnPushPackToLoop (realSer, pPack);
			}
			pISave->oldTokenErase(recToken);
			break;
		} 
		bool neetRet = NNeetRet(pN);
		if (!neetRet) {
			pSe = getServerSession (pN->ubyDesServId);
			if (!pSe) {
				pSe = getProcessSessionByFullServerId (pN->ubyDesServId);
			}
			if (!pSe) {
				pSe = defSession ();
			}
			if (pSe) {
				pSe->send (pPack);
			} else {
				NSetOtherNetLoopSend(pN);
				clonePackToOtherNetThread (pPack);
			}
			break;
		}
		auto newToken = nextToken ();
		tokenInfo info;
		info.lastServer = myHandle;
		info.oldToken = pN->dwToKen;
		info.sessionId = pPack->sessionID;
		info.inGateSerId = c_emptyLocalServerId;
		pN->dwToKen = newToken;
		pISave->oldTokenInsert(newToken, info);
		std::pair<impLoop*, NetTokenType> argS;
		argS.first = this;
		argS.second = newToken;
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto timeOut = rArgS.packTokenTime ();
		auto& rTimeMgr = getTimerMgr ();
		rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));

		pSe = getServerSession (pN->ubyDesServId);
		if (!pSe) {
			pSe = getProcessSessionByFullServerId (pN->ubyDesServId);
		}
		if (!pSe) {
			pSe = defSession ();
		}
		if (pSe) {
			pSe->send (pPack);
		} else {
			NSetOtherNetLoopSend(pN);
			pN->inGateSerId = mySId;  /*   请求其它线程代发,记录下本线程ID, 便于返回     */
			clonePackToOtherNetThread (pPack);
		}
		
    } while (0);
    return nRet;
}

int  impLoop:: procProx(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		iPackSave* pISave = getIPackSave ();
		auto pN = P2NHead (pPack);
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		bool bIsRet = NIsRet (pN);
		myAssert (!bIsRet);
		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		myAssert (sPId == myPId); /* 该函数只处理首站发出的ask包 */

		auto toNetPack = tSingleton<loopMgr>::single().toNetPack ();
		bool bSave = NNeetRet(pN);
		ISession* pSe = nullptr;
		myAssert (EmptySessionID == pPack->sessionID && c_emptyLoopHandle == pPack->loopId);    /*   纯测试,不确定会不会出现该情况   */
		if (EmptySessionID == pPack->sessionID) {
			pSe = getServerSession (pN->ubyDesServId);
			if (!pSe) {
				pSe = defSession ();
			}
		} else {
			pSe = getSession (pPack->sessionID);
		}

		pPack->pAsk = 0;
		if (pSe) {
			auto toNetPack = tSingleton<loopMgr>::single().toNetPack ();
			packetHead* pNew = nullptr;
			toNetPack (pPack, pNew);
			if (bSave) {
				if (!pNew) {
					pNew =  sclonePack (pPack);  /*  由于要保存原包,克隆一份用于发送 */
				}
				pSe->send (pNew);
				nRet = procPacketFunRetType_doNotDel; /* 此处是首发ask类消息, 不能删原包  */
			} else {
				if (pNew) {
					pSe->send (pNew);  /* 发压缩包 */
					nRet = procPacketFunRetType_del;  /* 发压缩包不需保存原包 */
				} else {
					pSe->send (pPack);  /* 发原包,不能删 */
				}
			}
		} else {
			auto bMyCh = isMyChannelMsg (pN->uwMsgID);  /*  */
			if (!bMyCh) {
				mError ("procProx pSe null pack = "<<*pPack);
			}
			nRet = procPacketFunRetType_del; /* 无法发送,删除包 */
		}
    } while (0);
    return nRet;
}

int impLoop::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto& rPho = tSingleton<loopMgr>::single();
	auto& rCS = rPho.getPhyCallback();
	auto pN = P2NHead(pPack);
	bool bIsRet = NIsRet(pN);
	auto getCurServerHandleFun = rCS.fnGetCurServerHandle;
	auto cHandle = getCurServerHandleFun ();
	auto myHandle = id ();
	myAssert (cHandle == myHandle);
	do {
		procPacketArg argP;
		argP.handle = id ();
		if (c_emptyLoopHandle == pN->ubyDesServId	) {
			nRet = processAllGatePack (pPack);
			break;
		}
		auto bInOncePro = packInOnceProc(pPack);
		if (bInOncePro) {
			myAssert (myHandle == pN->ubyDesServId);
			nRet = processLocalServerPack (pPack);    /*  一定是本进程其它线程发给本线程的 */
			break;
		}

		loopHandleType desPId;
		loopHandleType desSId;
		fromHandle (pN->ubyDesServId, desPId, desSId);

		loopHandleType srcPId;
		loopHandleType srcSId;
		fromHandle (pN->ubySrcServId, srcPId, srcSId);
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		if (desPId == myPId) {
			myAssert (myHandle == pN->ubyDesServId);  /* 如果不等在接到网络数据包时就转发了不会到这  */
			nRet = processOtherAppPack (pPack);  /*   其它进程发给本线程处理的包(通过本进程的其它线程从网络将收再转交给本线程处理)    */
			break;
		}
		if (srcPId == myPId) {
			if (myHandle == pPack->loopId) {
				/*    通过本线程从网络接下的Ask包交给其它线程处理后, 对应的ret包再通过本线程从网络发出          */
				myAssert(bIsRet);
				myAssert (EmptySessionID != pPack->sessionID && c_emptyLoopHandle != pPack->loopId);
				auto pS = getSession (pPack->sessionID);
				myAssert (pS);
				if (pS) {
					pS->send(pPack);
					nRet = procPacketFunRetType_doNotDel;
				} else {
					mError("can not find sesssion pack: "<<*pPack);
				}
			} else {
				nRet = procProx (pPack);
			}
			break;
		}
		auto isOtherLoopSend = NIsOtherNetLoopSend(pN);
		myAssert (isOtherLoopSend);
		if (isOtherLoopSend) {
			nRet = forwardForOtherServer (pPack); /*   本进程的其它线程在转发包时无法路由, 请求本线程尝试一下   */
		}
	} while (0);
	return nRet;
}

serverNode*  impLoop:: getServerNode ()
{
	return    &m_serverNode;
}
cTimerMgr&  impLoop::getTimerMgr()
{
	return m_timerMgr;
}
static bool sShowFps (void* pUserData)
{
	auto pServer =	(impLoop*)(*(void**)(pUserData));
	pServer->showFps ();
	return true;
}

int  impLoop:: processLocalServerPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rCS = tSingleton<loopMgr>::single().getPhyCallback();
		auto sendFun = rCS.fnPushPackToLoop;
		auto pN = P2NHead(pPack);
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		auto fromId = pN->ubySrcServId;
		auto toId = pN->ubyDesServId;
		bool bIsRet = NIsRet(pN);
		auto pF = findMsg(pN->uwMsgID);
		auto myHandle = id ();
		if(!pF) {
			if (pN->uwMsgID < 60000) {
				mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			}
			break;
		}
		if (bIsRet) { // pPack->pAsk put by other server
			auto  pAsk = (pPacketHead)(pPack->pAsk);
			pPack->pAsk = (decltype (pPack->pAsk))pAsk;
			nRet = pF((pPacketHead)(pPack->pAsk), pPack, nullptr);
		} else {
			packetHead* pRet = nullptr;
			auto fRet = pF(pPack, pRet, nullptr);
			if (pRet) {
				pRet->pAsk = (uqword)pPack;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = toId;
				pRN->ubyDesServId = fromId;
				pRN->dwToKen = pN->dwToKen;
				nRet = procPacketFunRetType_doNotDel;

				if (procPacketFunRetType_exitNow == fRet || procPacketFunRetType_exitAfterLoop == fRet) {
					nRet |= fRet;
				}
				sendFun (pRN->ubyDesServId, pRet);
			}
		}
	} while (0);
	return nRet;
}

ISession*   impLoop:: getServerSession(ServerIDType sid)
{
    ISession*   nRet = nullptr;
    do {
		auto& rSS = serverSessionS ();
		auto it = rSS.find (sid);
		if (rSS.end () != it) {
			nRet = getSession (it->second);
		}
    } while (0);
    return nRet;
}

ISession*   impLoop:: getSession(SessionIDType sid)
{
	ISession*   nRet = nullptr;
	do {
		auto pServer = tcpServer ();
		if (pServer) {
			nRet = pServer->getSession (sid);
		}
	} while (0);
	return nRet;
}

int impLoop::processNetPackFun(ISession* session, packetHead* pack)
{
	int nRet = procPacketFunRetType_del;
	do {
		auto pN = P2NHead (pack);
		auto sid = session->id();
		pack->sessionID = sid;
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			nRet = processAllGatePack (pack);
			break;
		}
		auto& rMgr =  tSingleton<loopMgr>::single();
		auto fnPushPackToLoop = rMgr.getPhyCallback().fnPushPackToLoop;
		auto myHandle = id ();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);

		loopHandleType dPId;
		loopHandleType dSId;
		fromHandle (pN->ubyDesServId, dPId, dSId);
		auto fnFree = rMgr.getForLogicFun().fnFreePack;

		bool bIsRet = NIsRet(pN);
		bool bNeetRet = NNeetRet(pN);
		if (myPId == dPId) {
			auto fromNetPack = rMgr.fromNetPack ();
			packetHead* pNew = nullptr;
			fromNetPack (pack, pNew);
			if (pNew) {
				fnFree (pack);
				pack = pNew;
				pN = P2NHead (pack);
			}
			if (mySId == dSId) {
				nRet = processOtherAppToMePack(session, pack);  /*  发个本线程的收到就直接处理掉    */
			} else {
				if (!bIsRet && bNeetRet) {
					pack->sessionID = sid;
					pack->loopId = myHandle;
				} else {
					pack->sessionID = EmptySessionID;
					pack->loopId = c_emptyLoopHandle;
				}
				fnPushPackToLoop (pN->ubyDesServId, pack);
				nRet = procPacketFunRetType_doNotDel;
			}
			break;
		}
		/* 以下处理目标非本进程的包  */
		/*
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		if (sPId != myPId) {
		*/
			nRet = forward (pack);  /*   处理转发包(非本进程发出)   */
			break;
		// }
		/*
		auto upN = rMgr.canRouteNum ();
		iPackSave* pISave = getIPackSave ();
		bool bIsRet = NIsRet(pN);
		if (bIsRet) {
			auto pF = pISave->oldTokenFind (pN->dwToKen);
			if (!pF) {
				mError("ret pack can not find old token = "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID);
				break;
			} 
			auto delToken = pN->dwToKen;
			pN->dwToKen = pF->oldToken;
			auto sessionId = pF->sessionId;
			pISave->oldTokenErase (delToken);
			// pISave->netTokenPackErase (delToken);
			auto pSe = getSession (sessionId);
			if (pSe) {
				pSe->send (pack);
				nRet = procPacketFunRetType_doNotDel;
			} else {
				if (upN < 2) {
					mError ("session null pack: "<<*pack);
				} else {
					if (!NIsOtherNetLoopSend(pN)) {
						NSetOtherNetLoopSend(pN);
						clonePackToOtherNetThread (pack);
					}
				}
			}
			break;
		}
		auto pSe = getServerSession (pN->ubyDesServId);
		if (!pSe) {
			pSe = defSession ();
		}
		if (!pSe && upN < 2) {
			break;
		}
		auto& rMsgInfoMgr = rMgr.defMsgInfoMgr ();
		auto retMsgId = rMsgInfoMgr.getRetMsg (pN->uwMsgID);
		if (c_null_msgID != retMsgId) {
			auto newToken = nextToken ();
			tokenInfo info;
			info.lastServer = id ();
			info.oldToken = pN->dwToKen;
			info.sessionId = pSe?pSe->id():EmptySessionID;
			pN->dwToKen = newToken;
			pISave->oldTokenInsert(newToken, info);
			std::pair<impLoop*, NetTokenType> argS;
			argS.first = this;
			argS.second = newToken;
			auto& rArgS = tSingleton<mArgMgr>::single ();
			auto timeOut = rArgS.packTokenTime ();
			mTrace ("change token oldToken = "<<info.oldToken<<"newToken = "<<newToken
					<<"msgId = "<<pN->uwMsgID<<" pack = "<<pack);
			auto& rTimeMgr = getTimerMgr ();
			rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
		}
		if (pSe) {
			pSe->send (pack);
			nRet = procPacketFunRetType_doNotDel;
		} else {
			if (!NIsOtherNetLoopSend(pN)) {
				NSetOtherNetLoopSend(pN);
				clonePackToOtherNetThread (pack);
			}
		}
		*/
	} while (0);
	return nRet;
}

void impLoop::onAcceptSession(ISession* session, void* userData)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	rMgr.logicOnAccept (id(), session->id(), *((uqword*)userData));
}

void impLoop::onConnect(ISession* session, void* userData)
{
	auto pBuff = (uqword*)userData;
	mInfo ("onConnect userData = "<<userData<<" pBuff = "<<pBuff );
	auto sid = session->id();
	if (pBuff[0]) {  // not def
		auto pDS = defSession ();
		// myAssert (!pDS);
		if (!pDS) {
			setDefSession (session);
		}
	} else {
		auto& rMap = serverSessionS ();
		auto inRet = rMap.insert (std::make_pair((ServerIDType)(pBuff[1]), sid));
		myAssert (inRet.second);
	}

	auto& rMgr = tSingleton<loopMgr>::single();
	rMgr.logicOnConnect (id(), sid, *(pBuff + 2));
}

void impLoop::onClose(ISession* session)
{
}

void impLoop::onWritePack(ISession* session, packetHead* pack)
{
	auto pFree = tSingleton<loopMgr>::single().getPhyCallback().fnFreePack;
	pFree (pack);
	/*
	auto pPushPack = tSingleton<loopMgr>::single().getPhyCallback().fnPushPackToLoop;
	auto pN = P2NHead (pack);
	bool bIsRet = NIsRet (pN);
	do {
		if (bIsRet) {
			pFree (pack);
			break;
		}
		
		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		if (myPId != sPId) {
			pFree (pack);
			break;
		}
		if (c_null_msgID == pack->sessionID) {
			pFree (pack);
			break;
		}
		NSetAskSave(pN);
		pPushPack (pN->ubySrcServId, pack);
	} while (0);
	*/
}

int impLoop::init(const char* szName, ServerIDType id, serverNode* pNode,
		frameFunType funOnFrame, void* argS)
{
	int nRet = 0;
	do {
		auto nL = strlen(szName);
		auto pN = std::make_unique<char[]>(nL + 1);
		strcpy(pN.get(), szName);
		m_name = std::move(pN);
		m_id = id;
		m_funOnFrame = funOnFrame;
		m_pArg = argS;
		if (pNode) {
			m_serverNode = *pNode;
		}
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto packSaveTag = rArgS.savePackTag ();
		if (0 == packSaveTag) {
			m_pImpPackSave_map = std::make_unique <impPackSave_map> ();
			m_packSave = m_pImpPackSave_map.get();
		}

		if (pNode->fpsSetp) {
			auto& rTimer = getTimerMgr();
			auto pSer = this;
			rTimer.addComTimer (pNode->fpsSetp, sShowFps, &pSer, sizeof(pSer));
		}
		const auto c_tempStackSize = 32;
		const auto c_perLoopNum = 3;
		auto pTempBuf = std::make_unique<uqword[]>(c_tempStackSize);
		int nIndexT = 0;
		if (pNode->listenerNum || pNode->connectorNum) {
			myAssert (pNode->listenerNum <= c_onceServerMaxListenNum);
			myAssert (pNode->connectorNum <= c_onceServerMaxConnectNum);
			endPoint listerEP [c_onceServerMaxListenNum];
			for (decltype (pNode->listenerNum)i = 0; i < pNode->listenerNum; i++) {
				auto& ep = listerEP[i];
				auto& info = pNode->listenEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				ep.port = info.port;
				ep.userData = &info.userData;
				ep.userDataLen = sizeof(info.userData);
				if (info.bDef) {
					setCanDownRoute (true);
				}
			}
			endPoint  connectEP [c_onceServerMaxConnectNum];
			auto pTemp = pTempBuf.get();
			for (decltype (pNode->connectorNum)i = 0; i < pNode->connectorNum; i++) {
				myAssert (nIndexT < c_tempStackSize - c_perLoopNum);
				uqword* pBuff = pTemp + nIndexT;
				auto& ep = connectEP [i];
				auto& info = pNode->connectEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				uword uwDef = info.bDef ? 1 : 0;
				if (info.bDef) {
					ep.ppIConnector = &m_defSession;
				} else {
					ep.ppIConnector = nullptr;
				}
				ep.port = info.port;
				ep.userDataLen = c_perLoopNum * sizeof (uqword); // info.userDataLen;
				ep.userData = pBuff; // (void*)(&(ep.userLogicData[0]));
				if (info.bDef) {
					setCanUpRoute (true);
				}
				pBuff = (uqword*)(ep.userData);
				pBuff[0] = uwDef;
				pBuff[1] = info.targetHandle;
				// bool bRegRoute = info.rearEnd?true:info.regRoute;
				// pBuff[2] = bRegRoute?EmptySessionID:id;
				memcpy(pBuff + c_userQuewordNum, &(info.userData), sizeof (info.userData));
				mInfo("connect endpoint ip = "<<ep.ip<<" port = "<<ep.port<<" userData = "<<ep.userData<<" pBuff = "<<pBuff);
				nIndexT += c_perLoopNum;
			}
			int nR = 0;
			nR = initNet (listerEP, pNode->listenerNum,
					connectEP, pNode->connectorNum, nullptr, 0);
			if (nR) {
				mError ("initNet return error nR = "<<nR);
				break;
			}
		}
	} while(0);
	return nRet;
}

void   impLoop:: showFps ()
{
    do {
		auto& fps = fpsC ();
		auto dFps = fps.update (m_frameNum);
		mInfo(" FPS : "<<dFps);
    } while (0);
}

fpsCount&  impLoop:: fpsC ()
{
    return m_fpsC;
}

ServerIDType impLoop::id()
{
	return m_id;
}

const char* impLoop::name()
{
	return m_name.get();
}
	
void impLoop::clean()
{
}

int  impLoop:: onLoopBegin()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  impLoop:: onLoopEnd()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int impLoop::onLoopFrame()
{
	int nRet = 0;
	m_timerMgr.onFrame ();
	do {
	if (m_funOnFrame) {
		nRet = m_funOnFrame(m_pArg);
		if (procPacketFunRetType_exitNow == nRet || procPacketFunRetType_exitAfterLoop == nRet) {
			break;
		}
	}
	auto pNet = tcpServer ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
	}while(0);
	m_frameNum++;
	return nRet;
}

bool impLoop::regMsg(uword uwMsgId, procRpcPacketFunType pFun)
{
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
	return bRet;
}

bool impLoop::removeMsg(uword uwMsgId)
{
	bool bRet = m_MsgMap.erase(uwMsgId);
	return bRet;
}

procRpcPacketFunType impLoop::findMsg(uword uwMsgId)
{
	procRpcPacketFunType pRet = nullptr;
	auto it = m_MsgMap.find(uwMsgId);
	if (m_MsgMap.end () != it) {
		pRet = it->second;
	}
	return pRet;
}

iPackSave*     impLoop:: getIPackSave ()
{
	return m_packSave;
}

impLoop::serverSessionMap&  impLoop:: serverSessionS ()
{
    return m_serverSessionS;
}

ISession*  impLoop:: defSession ()
{
    return m_defSession;
}

void  impLoop:: setDefSession (ISession* v)
{
    m_defSession = v;
}

NetTokenType	 impLoop:: nextToken ()
{
    NetTokenType	 nRet = 0;
    do {
		auto pF = tSingleton<loopMgr>::single().getPhyCallback().fnNextToken;
		nRet = pF(id());
    } while (0);
    return nRet;
}

int  impLoop:: regRoute (ServerIDType objServer, SessionIDType sessionId, udword onlyId)
{
    int  nRet = 0;
    do {
		auto& rMap = serverSessionS ();
		auto inRet = rMap.insert (std::make_pair(objServer, sessionId));
		myAssert (inRet.second);
		loopHandleType objPId;
		loopHandleType objSId;
		fromHandle (objServer, objPId, objSId);
		auto& rSet =  processSessionS ();
		uqword key = objPId;
		key <<=32;
		key |= sessionId;
		auto setRet = rSet.insert (key);
		myAssert (setRet.second);
    } while (0);
    return nRet;
}

impLoop::processSessionMap&  impLoop:: processSessionS ()
{
    return m_processSessionS;
}

bool  impLoop:: canUpRoute ()
{
    return m_canUpRoute;
}

void  impLoop:: setCanUpRoute (bool v)
{
    m_canUpRoute = v;
}

bool  impLoop:: canDownRoute ()
{
    return m_canDownRoute;
}

void  impLoop:: setCanDownRoute (bool v)
{
    m_canDownRoute = v;
}

