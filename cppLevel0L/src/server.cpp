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
}

server::~server()
{
	if (m_pTcpServer) {
		auto delTcpServer = tSingleton <serverMgr>::single ().delTcpServerFn ();
		delTcpServer (m_pTcpServer);
		m_pTcpServer = nullptr;
	}
}
/*
server::allSessionMap&  server::  allSessionS ()
{
    return m_allSessionS;
}
*/
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
	auto& rTM = pArgS->first->tokenS ();
	rTM.erase (pArgS->second);
	return false;
}
/*
int server::judgeRetSend (ISession* session, packetHead* pack)
{
	int nRet = procPacketFunRetType_doNotDel;
	auto pN = P2NHead (pack);
	auto& rTM = tokenS ();
	if (NIsRet (pN)) {
		auto it = rTM.find (pN->dwToKen);
		auto fnFreePack = tSingleton<serverMgr>::single().getPhyCallback().fnFreePack;
		if (rTM.end () == it) {
			rWarn ("ret pack can not find token");
			fnFreePack (pack);
		} else {
			pN->dwToKen = it->second.oldToken;
			auto pITcp = getTcpServer ();
			auto pS = pITcp->getSession (it->second.sessionId);
			if (pS) {
				pS->send (pack);
			} else {
				rWarn ("ret pack can not find session");
				fnFreePack (pack);
			}
		}
	} else {
		tokenInfo info;
		info.oldToken = pN->dwToKen;
		info.sessionId = session->id ();
		auto newToken = nextToken ();
		rTM [newToken] = info;
		pN->dwToKen = newToken;
		auto& rTimeMgr = getTimerMgr ();
		std::pair<server*, NetTokenType> argS;
		argS.first = this;
		argS.second = newToken;
		rTimeMgr.addComTimer (5000, sDelTokenInfo, &argS, sizeof (argS));
		nRet = sendBySession (pack);
	}
	return nRet;
}
*/
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
			//rTrace (__FUNCTION__<<" 000 sessionNum = "<<rSessionS.size());
		} else {
			pSession = itL->second;
			//rTrace (__FUNCTION__<<" 111 sessionNum = "<<rSessionS.size());
		}
	} else {
		pSession = iter->second;
		//rTrace (__FUNCTION__<<" 222 sessionNum = "<<rSessionS.size());
	}
	if (pSession) {
		pSession->send (pack);
	} else {
		rWarn (__FUNCTION__<<" pSession is empty msgId = "<<pN->uwMsgID<<"  ubyDesServId ="<<(int)pN->ubyDesServId);
		auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
		auto fnFreePack = rMgr.fnFreePack;
		fnFreePack (pack);
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
		rTrace ("push logic msgId = "<<pN->uwMsgID<<" 	ubySrcServId = "<<
				(int)pN->ubySrcServId<<" ubyDesServId = "<<(int)pN->ubyDesServId);
		nRet = procPacketFunRetType_doNotDel;
		/*
		auto myHandle = pServer->myHandle ();
		nRet = processOncePack(myHandle, d);// call by level 0
											*/
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
	int nRet = procPacketFunRetType_del;
	auto pITcp = session->getServer ();
	auto pServer = (server*)(pITcp->userData());
	auto pN = P2NHead (pack);
	if (!NIsRet (pN)) {
		rTrace(__FUNCTION__<<" before change token = "<<pN->dwToKen);
		server::tokenInfo info;
		info.oldToken = pN->dwToKen;
		info.sessionId = session->id ();
		auto newToken = pServer->nextToken ();
		rTrace(__FUNCTION__<<" will change will token = "<<newToken);
		auto& rTM = pServer->tokenS ();
		rTM [newToken] = info;
		pN->dwToKen = newToken;
		auto& rTimeMgr = pServer->getTimerMgr ();
		std::pair<server*, NetTokenType> argS;
		argS.first = pServer;
		argS.second = newToken;
		auto& rMgr = tSingleton<serverMgr>::single ();
		auto timeOut = rMgr.packSendInfoTime ();
		rTimeMgr.addComTimer (5000, sDelTokenInfo, &argS, sizeof (argS));
		rTrace(" chang token oldToken = "<<info.oldToken<<" newToken = "<<newToken);
	}
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (pN->ubyDesServId, proId, serId);

	loopHandleType  myPId;
	loopHandleType  mySId;
	auto myH = pServer->myHandle ();
	fromHandle (myH, myPId, mySId);
	if (myPId == proId) {
		if (mySId == serId) {
			nRet = myProcessNetPackFun (pServer, session, pack);
		} else {
			auto& rMgr = tSingleton<serverMgr>::single();
			auto pServerS = rMgr.getServerS();
			auto pS = pServerS[serId];
			//rTrace(__FUNCTION__<<" before pushPack pack = "<<pack<<" serId = "<<(int)serId<<" msgId = "<<pN->uwMsgID<<" length = "<<pN->udwLength);
			pS->pushPack (pack);
			nRet = procPacketFunRetType_doNotDel;
		}
	} else {
		/*
		   auto& rMgr = tSingleton<serverMgr>::single();
		   auto pOut = rMgr.getOutServer ();
		   myAssert (pOut);
		   pOut->pushPack (pack);
		   nRet = procPacketFunRetType_doNotDel;
		   */
		pServer->pushPack (pack);
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
	auto pN = P2NHead (pack);
	if (pN->uwMsgID < framMsgIdBegin) {
		auto pITcp = session->getServer ();
		auto pServer = (server*)(pITcp->userData ());
		auto myHand = pServer->myHandle ();
		onWriteOncePack (myHand, pack);
	} else {
		auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
		auto fnFreePack = rMgr.fnFreePack;
		fnFreePack (pack);
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

	//rTrace (__FUNCTION__<<" token= "<<pN->dwToKen<< " myHandle = "<<(int)pU->myHandle);
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
			for (decltype (pMyNode->connectorNum)i; i < pMyNode->connectorNum; i++) {
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
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono:: milliseconds(1));
	}
	auto& os = std::cout;
	os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
}

bool server::pushPack (packetHead* pack)
{
	//rTrace (__FUNCTION__<<" pack = "<<pack);
	return m_slistMsgQue.pushPack (pack);
}

bool server::procProx(packetHead* pack)
{
	bool bRet = true;
	auto pN = P2NHead (pack);
	loopHandleType ubyPId;
	loopHandleType ubySId;
	fromHandle (pN->ubyDesServId, ubyPId, ubySId);
	loopHandleType  myPId;
	loopHandleType  mySId;
	auto myHand = myHandle ();
	fromHandle (myHand, myPId, mySId);
	/*
	rTrace(__FUNCTION__<<" pack = "<<pack<<"  msgId = "<<pN->uwMsgID<<"  ubySrcServId ="<<(int)pN->ubySrcServId
			<<"  ubyDesServId ="<<(int)pN->ubyDesServId<<" myHandle = "<<(int)myHand<<" token = "<<pN->dwToKen);
			*/
	if (myPId == ubyPId) {
		myAssert (mySId == ubySId);
		if (mySId == ubySId) {
			bRet = false;
		} else {
			rWarn ("mySId != ubySId msgId = "<<pN->uwMsgID<<" token = "<<pN->dwToKen);
			auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
			auto fnFreePack = rMgr.fnFreePack;
			fnFreePack (pack);
		}
	} else {
		//sendBySession (pack);
		auto& rTM = tokenS ();
		auto fnFreePack = tSingleton<serverMgr>::single().getPhyCallback().fnFreePack;

		if (NIsRet (pN)) {
			auto it = rTM.find (pN->dwToKen);
			if (rTM.end () == it) {
				rTrace("ret pack can not find token token = "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID);
			} else {
				pN->dwToKen = it->second.oldToken;
				auto pITcp = getTcpServer ();
				auto pS = pITcp->getSession (it->second.sessionId);
				if (pS) {
					pS->send (pack);
				} else {
					rWarn ("ret pack can not find session");
					fnFreePack (pack);
				}
				rTM.erase (it);
			}
		} else {
			auto& rSessionS =  serverSessionMap();
			auto iter = rSessionS.find (pN->ubyDesServId);
			ISession* pSession = nullptr;
			if (rSessionS.end () == iter) {
				auto lowH = toHandle (ubyPId, 0);
				auto upH = toHandle (ubyPId, LoopMark);
				auto itL = rSessionS.lower_bound (lowH);
				auto itU = rSessionS.upper_bound (upH);
				if (itL == itU) {
					pSession = defSession ();
					//rTrace (__FUNCTION__<<" 000 sessionNum = "<<rSessionS.size());
				} else {
					pSession = itL->second;
					//rTrace (__FUNCTION__<<" 111 sessionNum = "<<rSessionS.size());
				}
			} else {
				pSession = iter->second;
				//rTrace (__FUNCTION__<<" 222 sessionNum = "<<rSessionS.size());
			}
			auto& rMgr = tSingleton<serverMgr>::single();
			if (pSession) {
				tokenInfo info;
				info.oldToken = pN->dwToKen;
				info.sessionId = pSession->id ();
				auto newToken = nextToken ();
				rTM [newToken] = info;
				pN->dwToKen = newToken;
				auto& rTimeMgr = getTimerMgr ();
				std::pair<server*, NetTokenType> argS;
				argS.first = this;
				argS.second = newToken;
				auto timeOut = rMgr.packSendInfoTime ();
				rTimeMgr.addComTimer (timeOut, sDelTokenInfo, &argS, sizeof (argS));
				pSession->send (pack);
			} else {
				rWarn (__FUNCTION__<<" pSession is empty msgId = "<<pN->uwMsgID<<"  ubyDesServId ="<<(int)pN->ubyDesServId);
				auto fnFreePack = tSingleton<serverMgr>::single().getPhyCallback().fnFreePack;
				fnFreePack (pack);
			}
		}
	}
	return bRet;
}

cTimerMgr&  server:: getTimerMgr ()
{
	return m_timerMgr;
}

bool server::onFrame()
{
	bool bExit = false;
	auto pNet = m_pTcpServer;
	m_timerMgr.onFrame ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
	auto nQuit = OnLoopFrame(m_loopHandle); // call by level 0
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
		auto fnFreePack = rMgr.fnFreePack;
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
				/*
				rTrace (__FUNCTION__<<" before call Logice fun msgId = "<<pN->uwMsgID
						<<"  ubyDesServId ="<<(int)pN->ubyDesServId);
						*/
				nRet = processOncePack(m_loopHandle, d);// call by level 0
			}
			if (procPacketFunRetType_doNotDel == nRet) {
				p->pNext = n;
				n->pPer = p;
				//rTrace(__FUNCTION__<<" for not del pack d"<<" p = "<<p<<" n = "<<n<<" pH = "<<pH);
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
		//rTrace (__FUNCTION__<<" end while");
		n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			fnFreePack (d);
		}
	}
	return bExit;
}

NetTokenType	 server::nextToken ()
{
    return m_nextToken++;
}
server::tokenMap& server::tokenS ()
{
    return m_tokenS;
}

ITcpServer* server:: getTcpServer ()
{
	return m_pTcpServer;
}


