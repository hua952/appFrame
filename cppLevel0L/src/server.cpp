/*
#include <iostream>
#include "server.h"
#include "serverMgr.h"
#include "loop.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "strFun.h"
#include "myAssert.h"
#include "tSingleton.h"
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds
#include "cLog.h"
#include "cppLevel0LCom.h"
#include "iPackSave.h"
#include "mLog.h"
#include "argConfig.h"
#include "workServer.h"
#include "workServerMgr.h"

int onLoopFrame(loopHandleType pThis);
int processOncePack(loopHandleType pThis, packetHead* pPack);

server::server()
{
	// m_loopHandle = c_emptyLoopHandle;
	m_nextToken  = 0;
	m_curCallNum = 0;
	m_sleepSetp = 10;
	m_autoRun = true;

	m_id = c_emptyLoopHandle;
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
	m_route = false;
}

server::~server()
{
	
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

loopHandleType  server::myHandle ()
{
	return m_id; // m_loopHandle;
}

loopHandleType server:: myProId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	auto myId = id ();
	fromHandle (myId, proId, serId);
	return proId;
}

loopHandleType server:: myLoopId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	auto myId = id ();
	fromHandle (myId, proId, serId);
	return serId;
}

server::netMsgMap&  server::netMsgProcMap()
{
	return m_netMsgMap;
}

int server::init(serverNode* pMyNode)
{
	int nRet = 0;
	do {
		if (!pMyNode) {
			nRet = 1;
			break;
		}
		auto handle = pMyNode->handle;
		// m_loopHandle = handle;
		setSleepSetp (pMyNode->sleepSetp);
		setAutoRun (pMyNode->autoRun);
	} while (0);
	return nRet;
}

thread_local  loopHandleType  server::s_loopHandleLocalTh = c_emptyLoopHandle;
bool server::start()
{
	// rTrace("startThread handle = "<<m_loopHandle<<" this = "<<this);
	rTrace("startThread handle = "<<m_id<<" this = "<<this);
	m_pTh =std::make_unique<std::thread>(server::ThreadFun, this);
	return true;
}

void server::join()
{
	myAssert (m_pTh);
	m_pTh->join();
}

void server::detach ()
{
	myAssert (m_pTh);
	m_pTh->detach ();
}

void server::run()
{
	s_loopHandleLocalTh = m_id;// m_loopHandle;
	onMidLoopBegin(m_id);
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		
	}
	onMidLoopEnd(m_id);
	// auto& os = std::cout;
	// os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
	// os<<"Loop "<<(int)(m_id)<<" exit"<<std::endl;
}

bool server::pushPack (packetHead* pack)
{
	return m_slistMsgQue.pushPack (pack);
}

bool server::onFrame()
{
	PUSH_FUN_CALL
	bool bExit = false;
	m_timerMgr.onFrame ();
	
	auto myHandle = m_id; // m_loopHandle;
	auto nQuit = ::onLoopFrame(myHandle); // call by level 0
	if (procPacketFunRetType_exitNow & nQuit) {
		bExit = true;
	} else {
		if (procPacketFunRetType_exitAfterLoop & nQuit) {
			bExit = true;
		}
		packetHead head;
		auto pH = &head;
		m_slistMsgQue.getMsgS(pH);
		auto n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto p = d->pPer;
			int nRet =  procPacketFunRetType_del;
			auto pN = P2NHead (d);
			nRet = ::processOncePack(m_id, d);// call by level 0
			if (procPacketFunRetType_doNotDel & nRet) {
				p->pNext = n;
				n->pPer = p;
			}
			if (procPacketFunRetType_exitNow & nRet) {
				bExit = true;
				break;
			}
			if (procPacketFunRetType_exitAfterLoop & nRet) {
				bExit = true;
			}
		}
		n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			
			freePack(d);
		}
	}
	if (m_sleepSetp) {
		std::this_thread::sleep_for(std::chrono::microseconds (m_sleepSetp));
	}
	POP_FUN_CALL
	return bExit;
}

NetTokenType	 server::nextToken ()
{
    return m_nextToken++;
}
void   server::      popFromCallStack ()
{
	if (m_curCallNum) {
		m_curCallNum--;
	}
}

void  server:: pushToCallStack (const char* szTxt)
{
	if (m_curCallNum < c_MaxStackSize) {
		strCpy (szTxt, m_callStack[m_curCallNum++]);
	}
}

void   server:: logCallStack (int nL)
{
	for (decltype (m_curCallNum) i = 0; i < m_curCallNum; i++) {
		rTrace (m_callStack[i].get());
	}
}

udword  server:: sleepSetp ()
{
    return m_sleepSetp;
}

void  server:: setSleepSetp (udword v)
{
    m_sleepSetp = v;
}

bool  server:: autoRun ()
{
    return m_autoRun;
}

void  server:: setAutoRun (bool v)
{
    m_autoRun = v;
}

packetHead* sallocPacket(udword udwS)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto & mgr = rMgr.getForLogicFun();
	// packetHead* pRet = (packetHead*)(mgr.fnAllocPack(AllPacketHeadSize + udwS));
	packetHead* pRet = (packetHead*)(allocPack(AllPacketHeadSize + udwS));
	
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
	auto pS = tSingleton<workServerMgr>::single().getServer (pThis);
	myAssert (pS);
	return pS->onLoopBegin ();
}

int onMidLoopEnd(loopHandleType pThis)
{
	auto pS = tSingleton<workServerMgr>::single().getServer (pThis);
	myAssert (pS);
	return pS->onLoopEnd();
}

int onLoopFrame(loopHandleType pThis)
{
	
	auto pS = tSingleton<workServerMgr>::single().getServer (pThis);
	myAssert (pS);
	return pS->onLoopFrame();
}

int processOncePack(loopHandleType pThis, packetHead* pPack)
{
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->processOncePack(pPack);
	return nRet;
}

int  server:: clonePackToOtherNetThread (packetHead* pack, bool excMe)	
{
    int 	 nRet = 0;
    do {
		auto& rMgr = tSingleton<serverMgr>::single();
		// auto upN = rMgr.getAllCanRouteServerS (buf, c_Max);
		auto myId = id ();
		uword num = 0;
		auto pSS = rMgr.getNetServerS (num);
		for (decltype (num) i = 0; i < num; i++) {
			auto& rS = pSS[i];
			auto objId = rS.id();
			if (myId == objId && excMe) {
				continue;
			}
			auto pS = sclonePack (pack);
			pS->sessionID = EmptySessionID;
			auto pN = P2NHead(pack);
			pN->ubyDesServId = c_emptyLoopHandle;
			rMgr.pushPackToLoop (objId, pS);
			rS.pushPack (pS);
		}
    } while (0);
    return nRet;
}

int server:: processAllGatePack(ISession* session, packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rConfig = tSingleton<argConfig>::single ();
		nRet = processOtherAppToMePack(session, pPack);
		if (procPacketFunRetType_stopBroadcast & nRet) {
			nRet &= (~procPacketFunRetType_stopBroadcast);
			break;
		} else {
			myAssert (!NNeetRet(P2NHead(pPack)));
		}
		if (rConfig.appNetType () != appNetType_gate) {
			break;
		}
		clonePackToOtherNetThread (pPack); 
	} while (0);
	return nRet;
}

int server:: processOtherAppToMePack(ISession* session, packetHead* pPack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		auto& rMgr = tSingleton<serverMgr>::single();
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
		if (session) {
			pPack->sessionID = session->id ();
		}
		procPacketArg argP;
		argP.handle = id ();
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
			nRet = pF(pAskPack, pPack, &argP);
			pIPackSave->netTokenPackErase (pN->dwToKen);
			break;
		}
		bool bNeetRet = NNeetRet(pN);
		packetHead* pRet = nullptr;

		nRet = pF(pPack, pRet, &argP);
		if (pRet) {
			if (bNeetRet && session) {
				packetHead* pNew = nullptr;
				toNetPack (P2NHead(pRet), pNew);
				if (pNew) {
					freePack (pRet);
					pRet = pNew;
				}
				pRet->sessionID = pPack->sessionID;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = id ();
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				session->send (pRet);
			} else {
				freePack (pRet);
				pRet = nullptr;
			}
		}
    } while (0);
    return nRet;
}

int server:: processOtherAppPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	auto& rSerMgr = tSingleton<serverMgr>::single ();
	auto pN = P2NHead (pPack);
	auto bIsRet = NIsRet (pN);

	procPacketArg argP;
	argP.handle = id ();
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
			myAssert (pAskPack);
			if (pAskPack) {
				pPack->pAsk = (uqword)pAskPack;  // must for delete pAskPack
				nRet = pF(pAskPack, pPack, &argP);
				pIPackSave->netTokenPackErase (pN->dwToKen);
			} else {
				mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
						<<" length = "<<pN->udwLength);
			}
			break;
		} 
		packetHead* pRet = nullptr;
		nRet = pF(pPack, pRet, &argP);
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
				rSerMgr.pushPackToLoop (pPack->loopId, pRet);
			} else {
				freePack (pRet); 
				pRet = nullptr;
			}
		}
	} while (0);
	return nRet;
}
static bool sDelTokenInfo(void* pUserData)
{
	auto pArgS = (std::pair<server*, NetTokenType>*) pUserData;
	auto pI = pArgS->first->getIPackSave ();
	pI->oldTokenErase(pArgS->second);
	return false;
}
bool isMyChannelMsg (msgIdType	msgId)
{
	return msgId > 60000;
}

ISession*   server:: getProcessSession(ServerIDType pid)
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

ISession* server:: getProcessSessionByFullServerId(ServerIDType fullId)
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

int  server:: sendPackToSomeSession(netPacketHead* pN, uqword* pSessS, udword sessionNum)
{
    int  nRet = 0;
    do {
		// auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rMgr = tSingleton<serverMgr>::single();
		// auto toNetPack = rMgr.toNetPack ();
		packetHead* pNew = nullptr;
		toNetPack (pN, pNew);
		if (pNew) {
			pN = P2NHead(pNew);
		}
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
			auto pB = (ubyte*)N2User(pSN);
			pSe->send (pSend);
		}
		if (pNew) {
			freePack (pNew);
		}
    } while (0);
    return nRet;
}


int server:: forward(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		auto& rMgr = tSingleton<serverMgr>::single();
		auto& rConfig = tSingleton<argConfig>::single ();
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
		myAssert (sPId != myPId);
		ISession* pSe = nullptr;
		if (NIsRet (pN)) {
			auto recToken = pN->dwToKen;
			auto pInfo = pISave->oldTokenFind (recToken);
			myAssert(pInfo);
			pN->dwToKen = pInfo->oldToken;
			pSe = getSession (pInfo->sessionId); // getServerSession (pInfo->sessionId);
			if (pSe) {
				pSe->send (pPack);
			} else {
				rWarn("ret pack can not find client scession sesId = "<<pInfo->sessionId<<" pPack = "<<*pPack);
				nRet = procPacketFunRetType_del;
			}
			pISave->oldTokenErase(recToken);
			break;
		} 

		pSe = getProcessSessionByFullServerId (pN->ubyDesServId);
		myAssert (pSe);
		if (!pSe) {
			rError(" can not find server session so can not forward pPack = "<<*pPack);
			nRet = procPacketFunRetType_del;
			break;
		}
		bool neetRet = NNeetRet(pN);
		if (neetRet) {
			auto newToken = nextToken ();
			tokenInfo info;
			info.lastServer = myHandle;
			info.oldToken = pN->dwToKen;
			info.sessionId = pPack->sessionID;
			info.inGateSerId = c_emptyLocalServerId;
			pN->dwToKen = newToken;
			pISave->oldTokenInsert(newToken, info);
			std::pair<server*, NetTokenType> argS;
			argS.first = this;
			argS.second = newToken;
			auto timeOut = rConfig.delSaveTokenTime ();
			auto& rTimeMgr = getTimerMgr ();
			rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
		}
		pSe->send (pPack);
    } while (0);
    return nRet;
}

int  server:: procProx(packetHead* pPack)
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
		myAssert (sPId == myPId);

		ISession* pSe = nullptr;
		if (EmptySessionID == pPack->sessionID) {
			pSe = defSession ();
			if (!pSe) {
				pSe = getProcessSessionByFullServerId(pN->ubyDesServId);
			}
		} else {
			pSe = getSession (pPack->sessionID);
		}

		pPack->pAsk = 0;
		if (pSe) {
			pSe->send (pPack);
		} else {
			auto bMyCh = isMyChannelMsg (pN->uwMsgID);
			if (!bMyCh) {
				mError ("procProx pSe null pack = "<<*pPack);
			}
			nRet = procPacketFunRetType_del;
		}
    } while (0);
    return nRet;
}

int server::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rConfig = tSingleton<argConfig>::single();
	auto pN = P2NHead(pPack);
	bool bIsRet = NIsRet(pN);
	auto myHandle = id ();
	do {
		procPacketArg argP;
		argP.handle = id ();
		auto appNetType = rConfig.appNetType ();	
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			myAssert (route());
			if (appNetType_gate == appNetType) {
				nRet = processOtherAppToMePack (nullptr, pPack);
			} else {
				auto pDS = defSession ();
				myAssert (pDS);
				if (pDS) {
					packetHead* pNew = nullptr;
					toNetPack (pN, pNew);
					if (pNew) {
						pDS->send(pNew);
					} else {
						pDS->send(pPack);
						nRet = procPacketFunRetType_doNotDel;
					}
				}
			}
			break;
		}
		myAssert (c_emptyLoopHandle != pN->ubyDesServId);
		auto bInOncePro = packInOnceProc(pPack);
		if (bInOncePro) {
			myAssert (myHandle == pN->ubyDesServId);
			nRet = processLocalServerPack (pPack);
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
			myAssert (myHandle == pN->ubyDesServId);
			nRet = processOtherAppPack (pPack);
			break;
		}
		if (srcPId == myPId) {
			if (myHandle == pPack->loopId) {
				myAssert(bIsRet);
				myAssert (EmptySessionID != pPack->sessionID && c_emptyLoopHandle != pPack->loopId);
				auto pS = getSession (pPack->sessionID);
				myAssert (pS);
				if (pS) {
					// auto toNetPack = rMgr.toNetPack ();
					packetHead* pNew = nullptr;
					toNetPack (pN, pNew);
					if (pNew) {
						pS->send(pNew);
					} else {
						pS->send(pPack);
						nRet = procPacketFunRetType_doNotDel;
					}
				} else {
					mError(" ret pack can not find sesssion pack: "<<*pPack);
				}
			} else {
				myAssert(!bIsRet);
				nRet = procProx (pPack);
			}
			break;
		}
	} while (0);
	return nRet;
}

serverNode*  server:: getServerNode ()
{
	return    &m_serverNode;
}
cTimerMgr& server::getTimerMgr()
{
	return m_timerMgr;
}
static bool sShowFps (void* pUserData)
{
	auto pServer =	(server*)(*(void**)(pUserData));
	pServer->showFps ();
	return true;
}

int  server:: processLocalServerPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rSerMgr = tSingleton<serverMgr>::single ();
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
		procPacketArg argP;
		argP.handle = id ();
		if (bIsRet) { // pPack->pAsk put by other server
			auto  pAsk = (pPacketHead)(pPack->pAsk);
			pPack->pAsk = (decltype (pPack->pAsk))pAsk;
			nRet = pF((pPacketHead)(pPack->pAsk), pPack, &argP);
		} else {
			packetHead* pRet = nullptr;
			auto fRet = pF(pPack, pRet, &argP);
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

				rSerMgr.pushPackToLoop (pRN->ubyDesServId, pRet);
			}
		}
	} while (0);
	return nRet;
}

ISession* server:: getSession(SessionIDType sid)
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

int server::processNetPackFun(ISession* session, packetHead* pack)
{
	int nRet = procPacketFunRetType_del;
	do {
		auto pN = P2NHead (pack);
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		auto& rMgr =  rSerMgr;
		auto& rConfig = tSingleton<argConfig>::single ();
		auto sid = session->id();
		pack->sessionID = sid;
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			packetHead* pNew = nullptr;
			fromNetPack (pN, pNew);
			if (pNew) {
				pNew->sessionID = sid;
				auto nR = processAllGatePack (session, pNew);
				if (!(nR & procPacketFunRetType_doNotDel)) {
					freePack (pNew);
				}
			} else {
				nRet = processAllGatePack (session, pack);
			}
			break;
		}
		auto myHandle = id ();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);

		loopHandleType dPId;
		loopHandleType dSId;
		fromHandle (pN->ubyDesServId, dPId, dSId);

		bool bIsRet = NIsRet(pN);
		bool bNeetRet = NNeetRet(pN);
		if (myPId == dPId) {
			auto pB = (ubyte*)N2User(pN);
			packetHead* pNew = nullptr;
			fromNetPack (pN, pNew);
			auto pProcPack = pack;
			if (pNew) {
				pNew->sessionID = sid;
				pProcPack = pNew;
			}
			if (mySId == dSId) {
				auto nR = processOtherAppToMePack(session, pProcPack);
				if ((procPacketFunRetType_doNotDel & nR)) {
					if (pProcPack == pack) {
						nRet =  procPacketFunRetType_doNotDel;
					}
				} else {
					if (pNew) {
						freePack  (pNew);
					}
				}
			} else {
				if (!bIsRet && bNeetRet) {
					pProcPack->sessionID = sid;
					pProcPack->loopId = myHandle;
				} else {
					pProcPack->sessionID = EmptySessionID;
					pProcPack->loopId = c_emptyLoopHandle;
				}
				rSerMgr.pushPackToLoop (pN->ubyDesServId, pProcPack);
				if (pProcPack == pack) {
					nRet =  procPacketFunRetType_doNotDel;
				}
			}
			break;
		}
		auto appNetType = rConfig.appNetType ();
		if (appNetType != appNetType_gate) {
			rError(" I' am not gate can not forward my appNetType = "<<appNetType<<" pack = "<<*pack);
			freePack  (pack);
			break;
		}
		nRet = forward (pack);
		break;
	} while (0);
	return nRet;
}

void server::onAcceptSession(ISession* session, void* userData)
{
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	rMgr.logicOnAccept (id(), session->id(), *((uqword*)userData));
}

void server::onConnect(ISession* session, void* userData)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto sid = session->id();
	auto pBuff = (uqword*)userData;
	rMgr.logicOnConnect (id(), sid, *(pBuff + 2));
}

void server::onClose(ISession* session)
{
}

void server::onWritePack(ISession* session, packetHead* pack)
{
	auto pN = P2NHead(pack);
	if (pN->uwMsgID) {
		int a = 0; 
		a++;
	}
	freePack (pack); 
}

int server::initMid(const char* szName, ServerIDType id, serverNode* pNode,
		frameFunType funOnFrame, void* argS)
{
	int nRet = 0;
	auto& rConfig = tSingleton<argConfig>::single ();
	do {
		int nR = init (pNode);
		if (nR) {
			nRet = 2;
			mError ("init return error nR = "<<nR);
			break;
		}
		setRoute (pNode->route);
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
		auto packSaveTag = rConfig.savePackTag ();
		if (0 == packSaveTag) {
			m_pImpPackSave_map = std::make_unique <impPackSave_map> ();
			m_packSave = m_pImpPackSave_map.get();
		}
		setAutoRun (pNode->autoRun);
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
			myAssert (!pNode->route || pNode->listenerNum || 1 == pNode->connectorNum);
			for (decltype (pNode->connectorNum)i = 0; i < pNode->connectorNum; i++) {
				myAssert (nIndexT < c_tempStackSize - c_perLoopNum);
				uqword* pBuff = pTemp + nIndexT;
				auto& ep = connectEP [i];
				auto& info = pNode->connectEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				uword uwDef = info.bDef ? 1 : 0;
				if (pNode->route) {
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
				memcpy(pBuff + c_userQuewordNum, &(info.userData), sizeof (info.userData));
				mInfo("connect endpoint ip = "<<ep.ip<<" port = "<<ep.port<<" userData = "<<ep.userData<<" pBuff = "<<pBuff);
				nIndexT += c_perLoopNum;
			}
			nR = initNet (listerEP, pNode->listenerNum,
					connectEP, pNode->connectorNum, nullptr, 0);
			if (nR) {
				nRet = 2;
				mError ("initNet return error nR = "<<nR);
				break;
			}
		}
	} while(0);
	return nRet;
}

void server:: showFps ()
{
    do {
		auto& fps = fpsC ();
		auto dFps = fps.update (m_frameNum);
		mInfo(" FPS : "<<dFps);
    } while (0);
}

fpsCount&  server:: fpsC ()
{
    return m_fpsC;
}

ServerIDType server::id()
{
	return m_id;
}

const char* server::name()
{
	return m_name.get();
}
	
void server::clean()
{
}

int server:: onLoopBegin()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int server:: onLoopEnd()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int server::onLoopFrame()
{
	int nRet = 0;
	// m_timerMgr.onFrame ();
	do {
			auto pNet = tcpServer ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
	}while(0);
	m_frameNum++;
	return nRet;
}

bool server::regMsg(uword uwMsgId, procRpcPacketFunType pFun)
{
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
	return bRet;
}

bool server::removeMsg(uword uwMsgId)
{
	bool bRet = m_MsgMap.erase(uwMsgId);
	return bRet;
}

procRpcPacketFunType server::findMsg(uword uwMsgId)
{
	procRpcPacketFunType pRet = nullptr;
	auto it = m_MsgMap.find(uwMsgId);
	if (m_MsgMap.end () != it) {
		pRet = it->second;
	}
	return pRet;
}

iPackSave*     server:: getIPackSave ()
{
	return m_packSave;
}

ISession*  server:: defSession ()
{
    return m_defSession;
}

void  server:: setDefSession (ISession* v)
{
    m_defSession = v;
}

int  server:: regRoute (ServerIDType objServer, SessionIDType sessionId, udword onlyId)
{
    int  nRet = 0;
    do {
		// auto& rMap = serverSessionS ();
		// auto inRet = rMap.insert (std::make_pair(objServer, sessionId));
		loopHandleType objPId;
		loopHandleType objSId;
		fromHandle (objServer, objPId, objSId);
		auto& rSet =  processSessionS ();
		uqword key = objPId;
		key <<=32;
		key |= sessionId;
		auto it = rSet.find (key);
		if (rSet.end() != it) {
			auto seid = (SessionIDType)(*it);
			auto pOldS = getSession (seid);
			if (pOldS) {
				pOldS->close();
			}
			rSet.erase(it);
		}
		rSet.insert (key);
    } while (0);
    return nRet;
}

server::processSessionMap&  server:: processSessionS ()
{
    return m_processSessionS;
}

bool  server:: canUpRoute ()
{
    return m_canUpRoute;
}

void  server:: setCanUpRoute (bool v)
{
    m_canUpRoute = v;
}

bool  server:: canDownRoute ()
{
    return m_canDownRoute;
}

void  server:: setCanDownRoute (bool v)
{
    m_canDownRoute = v;
}

bool  server:: route ()
{
    return m_route;
}

void  server:: setRoute (bool v)
{
    m_route = v;
}
*/
