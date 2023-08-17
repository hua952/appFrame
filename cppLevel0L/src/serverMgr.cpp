#include <iostream>
#include <sstream>
#include <thread>
#include "serverMgr.h"
#include "strFun.h"
#include "loop.h"
#include "tSingleton.h"
#include "comFun.h"
#include "cppLevel0LCom.h"
#include "cArgMgr.h"

packetHead* allocPack(udword udwSize)
{
	PUSH_FUN_CALL
	auto pRet = (packetHead*)(new char [sizeof(packetHead) + sizeof(netPacketHead) + udwSize]);
	pRet->pAsk = 0;
	auto pN = P2NHead(pRet);
	pN->udwLength = udwSize;
	pN->uwTag = 0;
	POP_FUN_CALL
	//rTrace (" after allocPack pack = "<<pRet);
	return pRet;
}

void	freePack(packetHead* pack)
{
	PUSH_FUN_CALL
	lv0LogCallStack (27);
	if (pack->pAsk) {
		freePack ((packetHead*)(pack->pAsk));
		pack->pAsk = 0;
	}
	auto pN = P2NHead (pack);
	rTrace (" will delete pack "<<*pack);
	delete [] ((char*)(pack));
	POP_FUN_CALL
}

static int sendPackToLoop(packetHead* pack)
{
	int nRet = 0;
	auto pNH = P2NHead(pack);
	serverIdType ubySp;
	serverIdType ubySl;
	serverIdType ubyDp;
	serverIdType ubyDl;
	fromHandle (pNH->ubySrcServId, ubySp, ubySl);
	fromHandle (pNH->ubyDesServId, ubyDp, ubyDl);

	auto& rMgr = tSingleton<serverMgr>::single();
	server* pS = nullptr;
	rTrace (__FUNCTION__<<*pack);
	if (ubySp == ubyDp) {
		pS = rMgr.getServer (pNH->ubyDesServId); // pServerS[ubyDl];
		rTrace ("send pack did = "<<(int)ubyDl<<" pS = "<<pS);
	} else {
		pS = rMgr.getOutServer ();
		auto hs = pS->myHandle ();
		rTrace ("will send by up pS = "<<pS<<" handle = "<<(int)hs);
	}
	if (pS) {
		pS->pushPack (pack);
	}
	return nRet;
}
static int sPushPackToLoop (loopHandleType pThis, packetHead* pack)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	server* pS = nullptr;
	pS = rMgr.getServer (pThis); // pServerS[ubyDl];
	if (pS) {
		pS->pushPack (pack);
	} else {
		nRet = 1;
		rError ("can not find server handle = "<<pThis);
	}
	return nRet;
}

static void stopLoopS()
{
}

serverMgr::serverMgr()
{
	m_outNum = 1;
	m_createTcpServerFn = nullptr;
	m_delTcpServerFn = nullptr;
	m_packSendInfoTime = 5000;
}

udword  serverMgr::  delSendPackTime ()
{
    return m_delSendPackTime;
}
void  serverMgr::setDelSendPackTime (udword  va)
{
    m_delSendPackTime = va;
}
udword  serverMgr::  packSendInfoTime ()
{
    return m_packSendInfoTime;
}

void serverMgr:: setPackSendInfoTime (udword  va)
{
    m_packSendInfoTime = va;
}

server*  serverMgr::getOutServer()
{
	auto n = rand () % m_outNum;
	return g_serverS [n];
}

serverMgr::~serverMgr()
{
}

loopHandleType	serverMgr::procId()
{
	return m_procId;
}

loopHandleType	serverMgr::gropId()
{
	return m_gropId;
}


void serverMgr::setProcId(loopHandleType proc)
{
	m_procId = proc;
}

void			serverMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

serverIdType 	serverMgr::getServerNum()
{
	return g_ServerNum;
}

pserver* serverMgr::getServerS()
{
	return g_serverS.get();
}

PhyCallback&  serverMgr::getPhyCallback()
{
	return m_PhyCallback;
}


int serverMgr::procArgS(int nArgC, const char* argS[])
{
	for(int i = 1; i < nArgC; i++)
	{
		auto pC = argS[i];
		auto nL = strlen(pC);
		std::unique_ptr<char[]>	 name = std::make_unique<char[]>(nL + 1);
		strcpy(name.get(), pC);
		const int c_BuffNum = 3;
		char* buff[c_BuffNum];
		auto nR = strR(name.get(), '=', buff, c_BuffNum);
		if(2 == nR)
		{
			if(0 == strcmp(buff[0], "gropId")) {
				int nId = atoi (buff[1]);
				setGropId (nId);
			} else if(0 == strcmp(buff[0], "procId")) {
				int nId = atoi (buff[1]);
				setProcId(nId);
			} else if (0 == strcmp(buff[0], "netLib")) {
				auto nL = strlen (buff[1]);
				m_netLibName = std::make_unique <char[]>(nL + 1);
				auto p = m_netLibName.get();
				strNCpy (p, nL + 1, buff[1]);
			}
		}
	}
	return 0;
}
/*
static int sAddComTimer(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	int nL = pThis;
	rTrace ("at then begin of sAddComTimer pThis = "<<nL);
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	rTrace ("at then begin of sAddComTimer pTH = "<<pTH);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}
*/
static NetTokenType   sNextToken(loopHandleType pThis)
{
	NetTokenType   nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		nRet = pTH->nextToken ();
	}
	//rTrace (" 00000000000  handle = "<<pThis<<" ret token = "<<nRet);
	return nRet;
}


static void  sPushToCallStack (loopHandleType pThis, const char* szTxt)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->pushToCallStack (szTxt);
	}
}

static void  sPopFromCallStack (loopHandleType pThis)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->popFromCallStack ();
	}
}

static void  sLogCallStack (loopHandleType pThis, int nL)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->logCallStack (nL);
	}
}

server*   serverMgr:: getServer(loopHandleType handle)
{
	server* pRet = nullptr;
	/*
	loopHandleType  pid = 0;
	loopHandleType  sid = 0;
	fromHandle (handle, pid, sid);
	pRet = g_serverS[sid];
	*/
	for (auto i = 0; i < g_ServerNum; i++) {
		auto curH = g_serverS[i]->myHandle ();
		if (curH == handle) {
			pRet = g_serverS[i];
			break;
		}
	}
	return pRet;
}
static loopHandleType      getCurServerHandle ()
{
	return server::s_loopHandleLocalTh;
}

int serverMgr::initFun (int cArg, const char* argS[])
{
	//std::cout<<"start main"<<std::endl;
	int nRet = 0;
	tSingleton <cArgMgr>::createSingleton ();
	auto& rArgS = tSingleton<cArgMgr>::single ();
	rArgS.procArgS (cArg, argS);
	procArgS (cArg, argS);
	auto& rMgr = getPhyCallback();
	rMgr.fnSendPackToLoop = sendPackToLoop;
	rMgr.fnPushPackToLoop = sPushPackToLoop;
	rMgr.fnStopLoopS = stopLoopS;
	rMgr.fnAllocPack = allocPack;
	rMgr.fnFreePack = freePack;
	rMgr.fnLogMsg = logMsg;
	rMgr.fnNextToken = sNextToken;
	rMgr.fnGetCurServerHandle = getCurServerHandle; // Thread safety
	rMgr.fnPushToCallStack = sPushToCallStack;
	rMgr.fnPopFromCallStack = sPopFromCallStack;
	rMgr.fnLogCallStack = sLogCallStack;
	do {
		auto logLevel = rArgS.logLevel ();
		auto nInitLog = initLog ("appFrame", "appFrameLog.log", logLevel);
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			break;
		}
		rInfo ("Loger init OK");
		auto nInitMidFrame = InitMidFrame(cArg, argS, &rMgr);
		if (0 != nInitMidFrame) {
			nRet = 1;
			rError ("InitMidFrame error nRet =  "<<nInitMidFrame);
			break;
		}
		const auto c_maxLoopNum = 10;
		serverNode loopHandleS[c_maxLoopNum];
		auto proLoopNum =  getAllLoopAndStart(loopHandleS, c_maxLoopNum);
		rInfo ("initFun proLoopNum = "<<proLoopNum);
		if (proLoopNum > 0) {
			auto pNetLibName = m_netLibName.get();
			std::unique_ptr<char[]> binH;
			getCurModelPath(binH);
			std::string strPath = binH.get (); 
			binH.reset ();
			strPath += pNetLibName;
			auto nNetInit = initNetServer (strPath.c_str());
			if (nNetInit) {
				rError ("initNetServer error nNetInit = "<<nNetInit);
				break;
			}
			g_ServerNum = proLoopNum;
			g_serverS = std::make_unique<pserver[]>(proLoopNum);
			std::unique_ptr<server[]>	pServerImpS =  std::make_unique<server[]>(proLoopNum);
			auto pServerS = getServerS();
			auto pImpS = pServerImpS.get();
			for (int i = 0; i < proLoopNum; i++ ) {
				pServerS[i] = &pImpS[i];
				auto p = pServerS [i];
				p->init (&loopHandleS[i]);
			}
			for (int i = 0; i < proLoopNum; i++ ) {
				auto p = pServerS[i];
				p->start();
			}
			for (int i = 0; i < proLoopNum; i++ ) {
				auto p = pServerS [i];
				p->join();
			}
			loggerDrop ();
			std::cout<<"All server End"<<std::endl;
		}
	} while (0);
	return nRet;
}

createTcpServerFT  serverMgr::  createTcpServerFn ()
{
    return m_createTcpServerFn;
}

void  serverMgr::setCreateTcpServerFn (createTcpServerFT  va)
{
    m_createTcpServerFn = va;
}

delTcpServerFT  serverMgr::delTcpServerFn ()
{
    return m_delTcpServerFn;
}

void  serverMgr::setDelTcpServerFn (delTcpServerFT  va)
{
    m_delTcpServerFn = va;
}

ubyte  serverMgr::upNum ()
{
    return m_outNum;
}

void serverMgr::setUpNum (ubyte  va)
{
    m_outNum = va;
}

void         lv0PushToCallStack (const char* szTxt)
{
	sPushToCallStack (server::s_loopHandleLocalTh, szTxt);
}

void         lv0PopFromCallStack ()
{
	sPopFromCallStack(server::s_loopHandleLocalTh);
}

void         lv0LogCallStack (int nL)
{
	sLogCallStack (server::s_loopHandleLocalTh, nL);
}

