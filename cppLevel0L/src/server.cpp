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
#include "mArgMgr.h"

// #include "framMsgId.h"

int onLoopFrame(loopHandleType pThis);
int processOncePack(loopHandleType pThis, packetHead* pPack);

server::server()
{
	m_loopHandle = c_emptyLoopHandle;
	m_nextToken  = 0;
	m_curCallNum = 0;
	m_sleepSetp = 10;
	m_autoRun = true;

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

server::~server()
{
	
}

server::serverSessionMapT&  server::  serverSessionMap()
{
	return m_serverSessionS;
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

loopHandleType  server::myHandle ()
{
	return m_loopHandle;
}

loopHandleType server:: myProId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (m_loopHandle, proId, serId);
	return proId;
}

loopHandleType server:: myLoopId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (m_loopHandle, proId, serId);
	return serId;
}

server::netMsgMap&  server::netMsgProcMap()
{
	return m_netMsgMap;
}

int server::init(serverNode* pMyNode)
{
	int nRet = 1;
	do {
		if (!pMyNode) {
			nRet = 1;
			break;
		}
		auto handle = pMyNode->handle;
		m_loopHandle = handle;
		setSleepSetp (pMyNode->sleepSetp);
		setAutoRun (pMyNode->autoRun);
	} while (0);
	return nRet;
}

thread_local  loopHandleType  server::s_loopHandleLocalTh = c_emptyLoopHandle;
bool server::start()
{
	rTrace("startThread handle = "<<m_loopHandle<<" this = "<<this);
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
	s_loopHandleLocalTh = m_loopHandle;
	onMidLoopBegin(m_loopHandle);
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		
	}
	onMidLoopEnd(m_loopHandle);
	auto& os = std::cout;
	os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
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
	
	auto myHandle = m_loopHandle;
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
		auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto p = d->pPer;
			int nRet =  procPacketFunRetType_del;
			auto pN = P2NHead (d);
			nRet = ::processOncePack(m_loopHandle, d);// call by level 0
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
			auto pN = P2NHead (d);
			if (NIsRet (pN)) {  // ask pack will delete by midFrame
				PUSH_FUN_CALL
				rTrace ("Befor free pack = "<<d);
				freePack(d);
				POP_FUN_CALL
			}
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
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopBegin();
	rMgr.onLoopBegin(pThis);
	return nRet;
}

int onMidLoopEnd(loopHandleType pThis)
{
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopEnd();
	rMgr.onLoopEnd(pThis);
	return nRet;
}

int onLoopFrame(loopHandleType pThis)
{
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->onLoopFrame();
	return nRet;
}

int processOncePack(loopHandleType pThis, packetHead* pPack)
{
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->processOncePack(pPack);
	return nRet;
}

int  server:: clonePackToOtherNetThread (packetHead* pack)	
{
    int 	 nRet = 0;
    do {
		// auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rMgr = tSingleton<serverMgr>::single();
		// auto& rCS = rMgr.getPhyCallback();
		const auto c_Max = 16;
		loopHandleType buf[c_Max];
		auto upN = rMgr.getAllCanRouteServerS (buf, c_Max);
		auto myId = id ();
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		for (decltype (upN) i = 0; i < upN; i++) {
			auto objSer = buf[i];
			if (myId == objSer) {
				continue;
			}
			auto pS = sclonePack (pack);
			pS->sessionID = EmptySessionID;

			rSerMgr.pushPackToLoop (objSer, pS);
			// rCS.fnPushPackToLoop (objSer, pS);
		}
    } while (0);
    return nRet;
}

int server:: processAllGatePack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto pDS =   defSession ();
		// auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rMgr = tSingleton<serverMgr>::single();
		if (pDS) {
			packetHead* pNew = nullptr;
			auto toNetPack = rMgr.toNetPack ();
			auto pN = P2NHead(pPack);
			toNetPack (pN, pNew);
			if (pNew) {
				pDS->send (pNew);  /* 发压缩包 */
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
				procPacketArg argP;
				argP.handle = id ();
				nRet = pF(pPack, pRet, &argP);
				// myAssert(!pRet);
				if (pRet) {
					// auto fnFree = rMgr.getForLogicFun().fnFreePack;
					// fnFree (pRet);
					freePack  (pRet);
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

int server:: processOtherAppToMePack(ISession* session, packetHead* pPack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		// auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rMgr = tSingleton<serverMgr>::single();
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
		pPack->sessionID = session->id ();
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
			// auto freeFun = rCS.fnFreePack;
			if (bNeetRet) {
				auto toNetPack = rMgr.toNetPack ();
				packetHead* pNew = nullptr;
				toNetPack (P2NHead(pRet), pNew);
				if (pNew) {
					// freeFun (pRet);
					freePack (pRet);
					pRet = pNew;
				}
				pRet->sessionID = pPack->sessionID;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = pN->ubyDesServId;
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				session->send (pRet);
			} else {
				/* 发送Ask端不想处理Ret */
				// freeFun (pRet);
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
	// auto& rCS = tSingleton<loopMgr>::single().getPhyCallback();
	auto& rSerMgr = tSingleton<serverMgr>::single ();
	auto pN = P2NHead (pPack);
	auto bIsRet = NIsRet (pN);
	// auto freeFun = rCS.fnFreePack;

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
			if (pAskPack) {
				myAssert (pAskPack);
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
				// rCS.fnPushPackToLoop (pPack->loopId, pRet);
				rSerMgr.pushPackToLoop (pPack->loopId, pRet);
			} else {
				// freeFun (pRet);
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
		auto toNetPack = rMgr.toNetPack ();
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
			/*
			std::stringstream ss;
			std::unique_ptr<char[]> pOut;
			for (decltype (pSN->udwLength) i = 0; i < pSN->udwLength; i++) {
				ss<<std::hex<<(int)(pB[i])<<" ";
			}
			strCpy (ss.str().c_str(), pOut);
			auto szOut = ss.get();
			mInfo(" sendPackToSomeSession msgId = "<<pSN->uwMsgID<<" length = "<<pSN->udwLength<<" data = "<<szOut);
			*/
			pSe->send (pSend);
		}
		if (pNew) {
			/*
			auto fnFreePack = rMgr.getForLogicFun().fnFreePack;
			fnFreePack(pNew);
			*/
			freePack (pNew);
		}
    } while (0);
    return nRet;
}

int server:: forwardForOtherServer(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		// auto& rMgr = tSingleton<loopMgr>::single ();
		auto& rMgr = tSingleton<serverMgr>::single();
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
		std::pair<server*, NetTokenType> argS;
		argS.first = this;
		argS.second = newToken;
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto timeOut = rArgS.packTokenTime ();
		auto& rTimeMgr = getTimerMgr ();
		rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
    } while (0);
    return nRet;
}

int server:: forward(packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel;
    do {
		// auto& rMgr = tSingleton<loopMgr>::single ();
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		auto& rMgr = tSingleton<serverMgr>::single();
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
				// rCS.fnPushPackToLoop (realSer, pPack);
				rSerMgr.pushPackToLoop(realSer, pPack);
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
		std::pair<server*, NetTokenType> argS;
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
		myAssert (sPId == myPId); /* 该函数只处理首站发出的ask包 */

		// bool bSave = NNeetRet(pN);
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
			/*
			auto toNetPack = tSingleton<loopMgr>::single().toNetPack ();
			packetHead* pNew = nullptr;
			toNetPack (pN, pNew);
			if (pNew) {
				pSe->send (pNew);
				nRet = procPacketFunRetType_del;
			} else {
				pSe->send (pPack);
			}
			*/
			pSe->send (pPack);
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

int server::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto& rMgr = tSingleton<serverMgr>::single();
	// auto& rPho = tSingleton<loopMgr>::single();
	// auto& rCS = rPho.getPhyCallback();
	auto pN = P2NHead(pPack);
	bool bIsRet = NIsRet(pN);
	// auto getCurServerHandleFun = rCS.fnGetCurServerHandle;
	// auto cHandle = getCurServerHandleFun ();
	auto myHandle = id ();
	// myAssert (cHandle == myHandle);
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
					auto toNetPack = rMgr.toNetPack ();
					packetHead* pNew = nullptr;
					toNetPack (pN, pNew);
					if (pNew) {
						pS->send(pNew);
					} else {
						pS->send(pPack);
						nRet = procPacketFunRetType_doNotDel;
					}
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
		// auto& rCS = tSingleton<loopMgr>::single().getPhyCallback();
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		// auto sendFun = rCS.fnPushPackToLoop;
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
				// sendFun (pRN->ubyDesServId, pRet);
			}
		}
	} while (0);
	return nRet;
}

ISession*   server:: getServerSession(ServerIDType sid)
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
		// auto& rMgr =  tSingleton<loopMgr>::single();
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		auto& rMgr =  rSerMgr;
		// auto fnFree = rMgr.getForLogicFun().fnFreePack;
		auto sid = session->id();
		pack->sessionID = sid;
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			auto fromNetPack = rMgr.fromNetPack ();
			packetHead* pNew = nullptr;
			fromNetPack (pN, pNew);
			if (pNew) {
				pNew->sessionID = sid;
				auto nR = processAllGatePack (pNew);
				if (!(nR & procPacketFunRetType_doNotDel)) {
					// fnFree (pNew);
					freePack (pNew);
				}
			} else {
				nRet = processAllGatePack (pack);
			}
			break;
		}
		// auto fnPushPackToLoop = rMgr.getPhyCallback().fnPushPackToLoop;
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
			auto fromNetPack = rMgr.fromNetPack ();
			auto pB = (ubyte*)N2User(pN);
			/*
			std::stringstream ss;
			std::unique_ptr<char[]> pOut;
			for (decltype (pN->udwLength) i = 0; i < pN->udwLength; i++) {
				ss<<std::hex<<(int)(pB[i])<<" ";
			}
			strCpy(ss.str().c_str(), pOut);
			auto szOut = pOut.get();
			mInfo(" NetPackBeforeUnzip msgId = "<<pN->uwMsgID<<" length = "<<pN->udwLength<<" data = "<<szOut);
			*/
			packetHead* pNew = nullptr;
			fromNetPack (pN, pNew);
			auto pProcPack = pack;
			if (pNew) {
				pNew->sessionID = sid;
				pProcPack = pNew;
			}
			if (mySId == dSId) {
				auto nR = processOtherAppToMePack(session, pProcPack);  /*  发个本线程的收到就直接处理掉    */
				if ((procPacketFunRetType_doNotDel & nR)) {
					if (pProcPack == pack) {
						nRet =  procPacketFunRetType_doNotDel;
					}
				} else {
					if (pNew) {
						// fnFree (pNew);
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
				// fnPushPackToLoop (pN->ubyDesServId, pProcPack);
				rSerMgr.pushPackToLoop (pN->ubyDesServId, pProcPack);
				if (pProcPack == pack) {
					nRet =  procPacketFunRetType_doNotDel;
				}
			}
			break;
		}
		
		nRet = forward (pack);  /*   处理转发包(非本进程发出)   */
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

	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	rMgr.logicOnConnect (id(), sid, *(pBuff + 2));
}

void server::onClose(ISession* session)
{
}

void server::onWritePack(ISession* session, packetHead* pack)
{
	freePack (pack); 
	/*
	auto pFree = tSingleton<loopMgr>::single().getPhyCallback().fnFreePack;
	pFree (pack);
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

int server::initMid(const char* szName, ServerIDType id, serverNode* pNode,
		frameFunType funOnFrame, void* argS)
{
	int nRet = 0;
	do {
		int nR = init (pNode);
		if (nR) {
			nRet = 2;
			mError ("init return error nR = "<<nR);
		}
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

server::serverSessionMapMid&  server:: serverSessionS ()
{
    return m_serverSessionSMid;
}

ISession*  server:: defSession ()
{
    return m_defSession;
}

void  server:: setDefSession (ISession* v)
{
    m_defSession = v;
}
/*
NetTokenType	 server:: nextToken ()
{
    NetTokenType	 nRet = 0;
    do {
		auto& rMgr = tSingleton<serverMgr>::single();
		auto pF = rMgr.getPhyCallback().fnNextToken;
		nRet = pF(id());
    } while (0);
    return nRet;
}
*/
int  server:: regRoute (ServerIDType objServer, SessionIDType sessionId, udword onlyId)
{
    int  nRet = 0;
    do {
		auto& rMap = serverSessionS ();
		auto inRet = rMap.insert (std::make_pair(objServer, sessionId));
		// myAssert (inRet.second);
		loopHandleType objPId;
		loopHandleType objSId;
		fromHandle (objServer, objPId, objSId);
		auto& rSet =  processSessionS ();
		uqword key = objPId;
		key <<=32;
		key |= sessionId;
		auto setRet = rSet.insert (key);
		// myAssert (setRet.second);
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

