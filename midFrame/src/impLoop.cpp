#include "impMainLoop.h"
#include "tSingleton.h"
#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mLog.h"
#include "mArgMgr.h"
#include "impMainLoop.h"
#include "strFun.h"

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
	m_id = c_emptyModelId;
	m_serverNode.listenerNum = 0;
	m_serverNode.connectorNum = 0;
	m_frameNum = 0;
	m_defSession = nullptr;
}

impLoop::~impLoop()
{
}

int  impLoop:: processOtherAppPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;

	auto& rCS = tSingleton<loopMgr>::single().getPhyCallback();
	auto pN = P2NHead (pPack);
	auto bIsRet = NIsRet (pN);
	do {
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
			if (!pAskPack) {
				mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
						<<" length = "<<pN->udwLength);
				break;
			}
			myAssert (pAskPack);
			pPack->pAsk = (uqword)pAskPack;  // must for delete pAskPack
			nRet = pF(pAskPack, pPack, nullptr);
			pIPackSave->netTokenPackErase (pN->dwToKen);
		} else {
			packetHead* pRet = nullptr;
			nRet = pF(pPack, pRet, nullptr);
			if (pRet) {
				myAssert (c_null_msgID != pPack->sessionID);
				if (c_null_msgID == pPack->sessionID) {
					/* 发送Ask端不想处理Ret */
					auto freeFun = rCS.fnFreePack;
					freeFun (pRet);
					pRet = nullptr;
				} else {
					pRet->sessionID = pPack->sessionID;
					auto pRN = P2NHead (pRet);
					pRN->ubySrcServId = pN->ubyDesServId;
					pRN->ubyDesServId = pN->ubySrcServId;
					pRN->dwToKen = pN->dwToKen;
					int midSendPackToLoopFun(packetHead* pack);
					midSendPackToLoopFun (pRet);
				}
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
int  impLoop:: procProx(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		iPackSave* pISave = getIPackSave ();
		auto pN = P2NHead (pPack);
		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);
		myAssert (sPId == myPId); /* 该函数只处理首站发出 */
		if (NIsRet (pN)) {
			auto sessionId = pPack->sessionID;
			auto pS = getSession (sessionId);
			if (pS) {
				mDebug("prox send ret"<<*pPack);
				pS->send (pPack);
			} else {
				mWarn ("ret pack can not find session sessionId = "<<sessionId<<" packHead = "<<*pPack);
				nRet = procPacketFunRetType_del;
			}
		} else {
			// pPack->sessionId = EmptySessionID;
			ISession* pSe = nullptr;
			if (EmptySessionID == pPack->sessionID) {
				pSe = getServerSession (pN->ubyDesServId);
				if (!pSe) {
					pSe = defSession ();
				}
			} else {
				pSe = getSession (pPack->sessionID);
			}
			
			if (!pSe) {
				auto bMyCh = isMyChannelMsg (pN->uwMsgID);
				if (!bMyCh) {
					mError ("procProx pSe null pack = "<<*pPack);
				}
				nRet = procPacketFunRetType_del;
				break;
			}
			mDebug("prox send ask"<<*pPack);
			pSe->send (pPack);
		}
    } while (0);
    return nRet;
}

static bool sDelNetPack (void* pUP)
{
	auto pArg = (std::pair<NetTokenType, impLoop*>*) pUP; 
	auto pS = pArg->second;
	auto pISavePack = pS->getIPackSave ();
	auto pack = pISavePack->netTokenPackFind (pArg->first);
	if (pack) {
		mWarn ("pack delete by timer"<<*pack);
		auto fnFree = tSingleton<loopMgr>::single ().getPhyCallback().fnFreePack;
		pISavePack->netTokenPackErase (pArg->first);
		fnFree (pack);
	}
	return false;
}

int impLoop::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto& rPho = tSingleton<loopMgr>::single();
	auto& rCS = rPho.getPhyCallback();
	auto pN = P2NHead(pPack);
	auto getCurServerHandleFun = rCS.fnGetCurServerHandle;
	auto cHandle = getCurServerHandleFun ();
	auto myHandle = id ();
	myAssert (cHandle == myHandle);
	do {
		procPacketArg argP;
		argP.handle = id ();
		if (NIsAskSave(pN)) {
			myAssert (!NIsRet(pN));
			iPackSave* pISave = getIPackSave ();
			pISave->netTokenPackInsert (pPack);
			std::pair<NetTokenType, impLoop*> pa;
			pa.first = pN->dwToKen;
			pa.second = this;
			// mDebug ("netTokenPackInsert "<<*pPack);
			auto delTime = 5000;
			auto& rTimeMgr = getTimerMgr ();
			rTimeMgr.addComTimer (delTime, sDelNetPack, &pa, sizeof (pa));
			break;
		}
		auto bInOncePro = packInOnceProc(pPack);
		if (bInOncePro) {
			myAssert (myHandle == pN->ubyDesServId);
			nRet = processLocalServerPack (pPack);
		} else {
			loopHandleType desPId;
			loopHandleType desSId;
			fromHandle (pN->ubyDesServId, desPId, desSId);

			loopHandleType myPId;
			loopHandleType mySId;
			fromHandle (myHandle, myPId, mySId);
			if (desPId == myPId) {
				myAssert (myHandle == pN->ubyDesServId);  /* 如果不等在接到网络数据包时就转发了不会到这  */
				nRet = processOtherAppPack (pPack);
			}else {
				nRet = procProx (pPack);
			}
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
		bool bIsRet = NIsRet(pN);
		auto pF = findMsg(pN->uwMsgID);
		auto myHandle = id ();
		if(!pF) {
			mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			break;
		}
		if (bIsRet) { // pPack->pAsk put by other server
			nRet = pF((pPacketHead)(pPack->pAsk), pPack, nullptr);
		} else {
			packetHead* pRet = nullptr;
			pF(pPack, pRet, nullptr);
			if (pRet) {
				pRet->pAsk = (uqword)pPack;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = pN->ubyDesServId;
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				nRet = procPacketFunRetType_doNotDel;

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
		auto pServer = tcpServer ();
		if (pServer) {
			auto& rSS = serverSessionS ();
			auto it = rSS.find (sid);
			if (rSS.end () != it) {
				nRet = pServer->getSession (it->second);
			}
		}
    } while (0);
    return nRet;
}

int impLoop::processNetPackFun(ISession* session, packetHead* pack)
{
	int nRet = procPacketFunRetType_del;
	do {
		auto sid = session->id();
		pack->sessionID = sid;
		auto pN = P2NHead (pack);
		auto myHandle = id ();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);

		loopHandleType dPId;
		loopHandleType dSId;
		fromHandle (pN->ubyDesServId, dPId, dSId);
		if (myPId == dPId) {
			auto fnPushPackToLoop = tSingleton<loopMgr>::single().getPhyCallback().fnPushPackToLoop;
			fnPushPackToLoop (pN->ubyDesServId, pack);
			nRet = procPacketFunRetType_doNotDel;
			break;
		}

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
			pISave->netTokenPackErase (delToken);
			auto pSe = getSession (sessionId);
			if (pSe) {
				pSe->send (pack);
				nRet = procPacketFunRetType_doNotDel;
			} else {
				mError ("session null pack: "<<*pack);
			}
			break;
		}
		auto pSe = getServerSession (pN->ubyDesServId);
		if (!pSe) {
			pSe = defSession ();
		}

		if (!pSe) {
			mError ("session null pack: "<<*pack);
			break;
		}
		auto& rMsgInfoMgr = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		auto retMsgId = rMsgInfoMgr.getRetMsg (pN->uwMsgID);
		if (c_null_msgID != retMsgId) {
			auto newToken = nextToken ();
			tokenInfo info;
			info.oldToken = pN->dwToKen;
			info.sessionId = pSe->id();
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
	ServerIDType* pBuff = (ServerIDType*)&userData;
	auto sid = session->id();
	if (pBuff[0]) {
		myAssert (!m_defSession);
		m_defSession = session;
	} else {
		auto& rMap = serverSessionS ();
		auto inRet = rMap.insert (std::make_pair(pBuff[1], sid));
		myAssert (inRet.second);
	}

	auto& rMgr = tSingleton<loopMgr>::single();
	rMgr.logicOnConnect (id(), sid, *(pBuff + 2));
	// auto fu = m_serverNode.fnOnConnect;
	// fu(id(), sid, *(pBuff + 2));
}

void impLoop::onClose(ISession* session)
{
}

void impLoop::onWritePack(ISession* session, packetHead* pack)
{
	auto pFree = tSingleton<loopMgr>::single().getPhyCallback().fnFreePack;
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
			/* first send*/
			pFree (pack);
			break;
		}
		/* not first send*/
		if (c_null_msgID == pack->sessionID) {
			pFree (pack);
			break;
		}
		NSetAskSave(pN);
		pPushPack (pN->ubySrcServId, pack);
	} while (0);
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
			}
			endPoint  connectEP [c_onceServerMaxConnectNum];
			for (decltype (pNode->connectorNum)i = 0; i < pNode->connectorNum; i++) {
				auto& ep = connectEP [i];
				auto& info = pNode->connectEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				uword uwDef = info.bDef ? 1 : 0;
				ep.port = info.port;
				ep.userDataLen = sizeof(loopHandleType)*2 + sizeof(info.userData); // info.userDataLen;
				auto pTemp = std::make_unique<ubyte[]>(ep.userDataLen);
				ep.userData = pTemp.get();
				loopHandleType* pBuff = (loopHandleType*)(ep.userData);
				pBuff[0] = uwDef;
				pBuff[1] = info.targetHandle;
				memcpy(pBuff + 2, &info.userData, sizeof (info.userData));
				mTrace ("connect endpoint ip = "<<ep.ip<<" port = "<<ep.port<<" userData = "<<ep.userData);
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
	if (m_funOnFrame) {
		nRet = m_funOnFrame(m_pArg);
	}
	auto pNet = tcpServer ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
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


