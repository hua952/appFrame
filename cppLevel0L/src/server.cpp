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
#include "framMsgId.h"

server::server()
{
	m_loopHandle = c_emptyLoopHandle;
	m_pTcpServer = nullptr;
	m_defSession = nullptr;
	m_nextToken  = 0;
	m_curCallNum = 0;
	m_sleepSetp = 10;
}

server::~server()
{
	if (m_pTcpServer) {
		auto delTcpServer = tSingleton <serverMgr>::single ().delTcpServerFn ();
		delTcpServer (m_pTcpServer);
		m_pTcpServer = nullptr;
	}
}

server::serverSessionMapT&  server::  serverSessionMap()
{
	return m_serverSessionS;
}

ISession*  server::  defSession ()
{
    return m_defSession;
}

void server:: setDefSession (ISession*  va)
{
    m_defSession = va;
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

static bool sDelTokenInfo(void* pUserData)
{
	auto pArgS = (std::pair<server*, NetTokenType>*) pUserData;
	auto pI = pArgS->first->getTokenSave ();
	pI->netTokenPackErase (pArgS->second);
	return false;
}

int server::sendBySession(packetHead* pack)
{
	int nRet = procPacketFunRetType_doNotDel;
	auto pN = P2NHead (pack);
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (pN->ubyDesServId, proId, serId);
	auto& rSessionS =  serverSessionMap();
	auto iter = rSessionS.find (serId);
	ISession* pSession = nullptr;
	if (rSessionS.end () == iter) {
		auto lowH = toHandle (proId, 0);
		auto upH = toHandle (proId, LoopMark);
		auto itL = rSessionS.lower_bound (lowH);
		auto itU = rSessionS.upper_bound (upH);
		if (itL == itU) {
			pSession = defSession ();
		} else {
			pSession = itL->second;
		}
	} else {
		pSession = iter->second;
	}
	if (pSession) {
		pSession->send (pack);
	} else {
		rWarn (__FUNCTION__<<" pSession is empty msgId = "<<pN->uwMsgID<<"  ubyDesServId ="<<(int)pN->ubyDesServId);
		if (NIsRet (pN)) {
			PUSH_FUN_CALL
			rTrace ("Befor free pack = "<<pack);
			freePack (pack); //fnFreePack (pack);
			POP_FUN_CALL //
		}
	}
	return nRet;
}

static int  myProcessNetPackFun(server* pServer, ISession* session, packetHead* packet)
{
	int nRet = procPacketFunRetType_del;
	auto& rMap = pServer->netMsgProcMap ();
	auto pN = P2NHead (packet);
	if (pN->uwMsgID < framMsgIdBegin) {
		pServer->pushPack (packet); // logice msg
		rTrace ("push logic packet "<<*packet);
		nRet = procPacketFunRetType_doNotDel;
	} else {
		auto iter = rMap.find (pN->uwMsgID);
		if (rMap.end() == iter) {
			rError ("Not find net proc fun msgId = "<<pN->uwMsgID);
		} else {
			auto fun = iter->second;
			nRet = fun (pServer, session, packet);
		}
	}
	return nRet;
}

static int sProcessNetPackFun(ISession* session, packetHead* pack)
{
	rTrace("Net rec pack"<<*pack);
	int nRet = procPacketFunRetType_del;
	auto pITcp = session->getServer ();
	auto pServer = (server*)(pITcp->userData());
	auto pN = P2NHead (pack);
	
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (pN->ubyDesServId, proId, serId);

	loopHandleType  myPId;
	loopHandleType  mySId;
	auto myH = pServer->myHandle ();
	fromHandle (myH, myPId, mySId);
	if (myPId == proId) {
		if (!NIsRet (pN)) {
			rTrace(__FUNCTION__<<" before change token = "<<pN->dwToKen);
			tokenInfo info;
			info.oldToken = pN->dwToKen;
			info.sessionId = session->id ();
			auto newToken = pServer->nextToken ();

			pN->dwToKen = newToken;
			auto pI =  pServer->getTokenSave ();
			auto nR = pI->netTokenPackInsert (newToken, info);
			myAssert (0 == nR);
			auto& rTimeMgr = pServer->getTimerMgr ();
			std::pair<server*, NetTokenType> argS;
			argS.first = pServer;
			argS.second = newToken;
			auto& rMgr = tSingleton<serverMgr>::single ();
			auto timeOut = rMgr.packSendInfoTime ();
			rTimeMgr.addComTimer (5000, sDelTokenInfo, &argS, sizeof (argS));
			rTrace(" chang token oldToken = "<<info.oldToken<<" newToken = "<<newToken);
		}
		if (mySId == serId) {
			nRet = myProcessNetPackFun (pServer, session, pack);
		} else {
			auto& rMgr = tSingleton<serverMgr>::single();
			auto pServerS = rMgr.getServerS();
			auto pS = rMgr.getServer (pN->ubyDesServId);// pServerS[serId];
			pS->pushPack (pack);
			nRet = procPacketFunRetType_doNotDel;
		}
	} else {
		   auto& rMgr = tSingleton<serverMgr>::single();
		   auto pOut = rMgr.getOutServer ();
		   myAssert (pOut);
		   pOut->pushPack (pack);
		   nRet = procPacketFunRetType_doNotDel;
	}
	return nRet;
}

static void sOnAcceptSession(ISession* session, uqword userData)
{
	//server* pServer = (server*)userData;
}

static void sOnConnect(ISession* session, uqword userData)
{
	rInfo (__FUNCTION__<<" userData = "<<userData);
	auto pITcp = session->getServer ();
	myAssert (pITcp);
	do {
		if (!pITcp) {
			rWarn ("pITcp empty");
			break;
		}

		loopHandleType* pBuff = (loopHandleType*)(&userData);
		bool bDef = pBuff[0] != 0;
		auto pServer = (server*)(pITcp->userData ());
		myAssert (pServer);
		if (bDef) {
			pServer->setDefSession (session);
		}
		auto& rMap = pServer->serverSessionMap ();
		auto objH = pBuff[2];
		rMap [objH] = session;
		bool bRegHandle = 0 != pBuff[3];
		if (bRegHandle) {
			auto& rServerMgr = tSingleton<serverMgr>::single();
			auto& rCallback = rServerMgr.getPhyCallback ();
			auto allocFun = rCallback.fnAllocPack;
			auto pSend = allocFun (sizeof (regMyHandleAskPack));
			if (pSend) {
				auto pN = P2NHead (pSend);
				pN->ubyDesServId = objH;
				auto myH = pBuff[1];
				pN->ubySrcServId = myH;
				pN->uwMsgID = toFramMsgId(enFramMsgId_regMyHandleAsk);
				pN->dwToKen = pServer->nextToken ();
				auto pU = (regMyHandleAskPack*)(N2User (pN));
				pU->myHandle = myH;
				auto  fnSendPackToLoop = rCallback.fnSendPackToLoop;
				fnSendPackToLoop (pSend);
			}
		}
	} while (0);
}

static void sOnClose(ISession* session)
{
}

static void sOnWritePack(ISession* session, packetHead* pack)
{
	rTrace ("Net send pack"<<*pack);
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
	if (pN->uwMsgID < framMsgIdBegin) {
		auto pITcp = session->getServer ();
		auto pServer = (server*)(pITcp->userData ());
		auto myHand = pServer->myHandle ();
		if (NIsRet (pN)) {
			PUSH_FUN_CALL
			rTrace ("Befor free pack "<<*pack);
			freePack (pack); // fnFreePack (pack);
			POP_FUN_CALL
		}
	} else {
		PUSH_FUN_CALL
		rTrace ("Befor free pack = "<<pack);
		freePack (pack); // fnFreePack (pack);
		POP_FUN_CALL
	}
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

int sOnRegMyHandleAsk(server* pServer, ISession* session, packetHead* packet)
{
	int nRet = procPacketFunRetType_del;
    auto & rMap = pServer->serverSessionMap ();
	auto pN = P2NHead (packet);
	auto pU = (regMyHandleAskPack*)(N2User (pN));

	rMap [pU->myHandle] = session;
	auto& rServerMgr = tSingleton<serverMgr>::single ();
	auto& rCallback = rServerMgr.getPhyCallback ();
	auto allocFun = rCallback.fnAllocPack;
	auto pSend = allocFun (sizeof (regMyHandleRetPack));
	if (pSend) {
		auto pRN = P2NHead (pSend);
		pRN->uwMsgID = toFramMsgId(enFramMsgId_regMyHandleRet);
		pRN->dwToKen = pN->dwToKen;
		pRN->ubyDesServId = pN->ubySrcServId;
		pRN->ubySrcServId = pN->ubyDesServId;
		auto pRU = (regMyHandleRetPack*)(N2User (pRN));
		pRU->myHandle = pU->myHandle;
		pRU->result = 0;
		auto  fnSendPackToLoop = rCallback.fnSendPackToLoop;
		fnSendPackToLoop (pSend);
	}
	return nRet;
}

int sOnRegMyHandleRet(server* pServer, ISession* session, packetHead* packet)
{
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead (packet);
	auto pU = (regMyHandleRetPack*)(N2User (pN));
	rTrace (__FUNCTION__<<" token= "<<pN->dwToKen<<" result = "<<pU->result<<" myHandle = "<<(int)pU->myHandle);
	return nRet;
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
		m_pTokenSave_map = std::make_unique<tokenSave_map> ();
		m_iTokenSave = m_pTokenSave_map.get ();
		setSleepSetp (pMyNode->sleepSetp);
		if (pMyNode->listenerNum || pMyNode->connectorNum) {
			endPoint listerEP [c_onceServerMaxListenNum];
			for (decltype (pMyNode->listenerNum)i = 0; i < pMyNode->listenerNum; i++) {
				auto& ep = listerEP[i];
				auto& info = pMyNode->listenEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				ep.port = info.port;
				ep.userData = (uqword)this;
			}
			endPoint  connectEP [c_onceServerMaxConnectNum];
			for (decltype (pMyNode->connectorNum)i = 0; i < pMyNode->connectorNum; i++) {
				auto& ep = connectEP [i];
				auto& info = pMyNode->connectEndpoint[i];
				strNCpy (ep.ip, sizeof (ep.ip), info.ip);
				uword uwDef = info.bDef ? 1 : 0;
				ep.port = info.port;
				loopHandleType* pBuff = (loopHandleType*)(&ep.userData);
				pBuff[0] = uwDef;
				pBuff[1] = handle;
				pBuff[2] = info.targetHandle;
				pBuff[3] = info.bRegHandle ? 1:0;
				rTrace ("connect endpoint ip = "<<ep.ip<<" port = "<<ep.port<<" userData = "<<ep.userData);
			}
			auto crFun = tSingleton<serverMgr>::single().createTcpServerFn();
			callbackS cb;
			cb.procPackfun = sProcessNetPackFun;
			cb.acceptFun = sOnAcceptSession;
			cb.connectFun = sOnConnect;
			cb.closeFun = sOnClose;
			cb.onWritePackFun = sOnWritePack;
			m_pTcpServer = crFun (&cb, listerEP, pMyNode->listenerNum,
					connectEP, pMyNode->connectorNum, nullptr, 0);
			if (!m_pTcpServer) {
				nRet = 2;
				break;
			}
			m_pTcpServer->setUserData (this);
			auto& rMsgMap = netMsgProcMap();
			rMsgMap [toFramMsgId (enFramMsgId_regMyHandleAsk)] = sOnRegMyHandleAsk;
			rMsgMap [toFramMsgId (enFramMsgId_regMyHandleRet)] = sOnRegMyHandleRet;
		}
	} while (0);
	return nRet;
}

thread_local  loopHandleType  server::s_loopHandleLocalTh = c_emptyLoopHandle;
bool server::start()
{
	m_pTh =std::make_unique<std::thread>(server::ThreadFun, this);
	return true;
}

void server::join()
{
	myAssert (m_pTh);
	m_pTh->join();
}

void server::run()
{
	s_loopHandleLocalTh = m_loopHandle;
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		if (m_sleepSetp) {
			std::this_thread::sleep_for(std::chrono::microseconds (m_sleepSetp));
		}
	}
	auto& os = std::cout;
	os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
}

bool server::pushPack (packetHead* pack)
{
	return m_slistMsgQue.pushPack (pack);
}

bool server::procProx(packetHead* pack)
{
	PUSH_FUN_CALL
	bool bRet = true;
	auto pN = P2NHead (pack);
	loopHandleType desPId;
	loopHandleType desSId;
	fromHandle (pN->ubyDesServId, desPId, desSId);

	loopHandleType srcPId;
	loopHandleType srcSId;
	fromHandle (pN->ubySrcServId, srcPId, srcSId);

	loopHandleType  myPId;
	loopHandleType  mySId;
	auto myHand = myHandle ();
	fromHandle (myHand, myPId, mySId);
	if (myPId == desPId) {
		if (mySId == desSId) {
			bRet = false;
		} else {
			rWarn ("mySId != desSId msgId = "<<pN->uwMsgID<<" token = "<<pN->dwToKen<<" mySId = "
					<<(int)mySId<<" ubySId = "<<(int)desSId);
			auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
			if (NIsRet (pN)) {
				PUSH_FUN_CALL
				rTrace ("befor free pack = "<<pack);
				freePack(pack);
				POP_FUN_CALL
			}
		}
	} else {
		auto pI =  getTokenSave ();
		if (NIsRet (pN)) {
			if (desPId != myPId) {
				auto pF = pI->netTokenPackFind (pN->dwToKen);
				if (!pF) {
					rTrace("ret pack can not find token token = "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID);
				} else {
					pN->dwToKen = pF->oldToken;
					auto pITcp = getTcpServer ();
					auto pS = pITcp->getSession (pF->sessionId);
					if (pS) {
						pS->send (pack);
					} else {
						rWarn ("ret pack can not find session");
						PUSH_FUN_CALL
							rTrace ("Befor free pack = "<<pack);
						freePack(pack);
						POP_FUN_CALL
					}
					pI->netTokenPackErase (pN->dwToKen);
				}
			}
		} else {
			auto& rSessionS =  serverSessionMap();
			auto iter = rSessionS.find (pN->ubyDesServId);
			ISession* pSession = nullptr;
			if (rSessionS.end () == iter) {
				auto lowH = toHandle (desPId, 0);
				auto upH = toHandle (desPId, LoopMark);
				auto itL = rSessionS.lower_bound (lowH);
				auto itU = rSessionS.upper_bound (upH);
				if (itL == itU) {
					pSession = defSession ();
				} else {
					pSession = itL->second;
				}
			} else {
				pSession = iter->second;
			}
			auto& rMgr = tSingleton<serverMgr>::single();
			if (pSession) {
				if (myPId != srcPId) {
					tokenInfo info;
					info.oldToken = pN->dwToKen;
					info.sessionId = pSession->id ();
					auto newToken = nextToken ();
					pI->netTokenPackInsert (newToken, info); // rTM [newToken] = info;
					pN->dwToKen = newToken;
					auto& rTimeMgr = getTimerMgr ();
					std::pair<server*, NetTokenType> argS;
					argS.first = this;
					argS.second = newToken;
					auto timeOut = rMgr.packSendInfoTime ();
					rTrace ("change token oldToken = "<<info.oldToken<<"newToken = "<<newToken
							<<"msgId = "<<pN->uwMsgID<<" pack = "<<pack);
					rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
				}
				pSession->send (pack);
			} else {
				rWarn (__FUNCTION__<<" pSession is empty msgId = "<<pN->uwMsgID<<"  ubyDesServId ="<<(int)pN->ubyDesServId);
			}
		}
	}
	POP_FUN_CALL
	return bRet;
}

cTimerMgr&  server:: getTimerMgr ()
{
	return m_timerMgr;
}

bool server::onFrame()
{
	PUSH_FUN_CALL
	bool bExit = false;
	auto pNet = m_pTcpServer;
	m_timerMgr.onFrame ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
	auto myHandle = m_loopHandle;
	auto nQuit = OnLoopFrame(myHandle); // call by level 0
	if (procPacketFunRetType_exitNow == nQuit) {
		bExit = true;
	} else {
		if (procPacketFunRetType_exitAfterLoop == nQuit) {
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
			bool bProc = procProx (d);
			if (bProc) {
				nRet = procPacketFunRetType_doNotDel; // will release by send
			} else {
				auto pN = P2NHead (d);
				nRet = processOncePack(m_loopHandle, d);// call by level 0
			}
			if (procPacketFunRetType_doNotDel == nRet) {
				p->pNext = n;
				n->pPer = p;
			} else {
				if (procPacketFunRetType_exitNow == nRet) {
					bExit = true;
					break;
				}
				if (procPacketFunRetType_exitAfterLoop == nRet) {
					bExit = true;
				}
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
	POP_FUN_CALL
	return bExit;
}

NetTokenType	 server::nextToken ()
{
    return m_nextToken++;
}

iTokenSave*  server::getTokenSave ()
{
	return m_iTokenSave;
}

ITcpServer* server:: getTcpServer ()
{
	return m_pTcpServer;
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

