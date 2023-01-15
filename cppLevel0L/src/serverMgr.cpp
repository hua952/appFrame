#include <iostream>
#include <sstream>
#include "serverMgr.h"
#include "strFun.h"
#include "loop.h"
#include "tSingleton.h"
#include "comFun.h"
#include "cppLevel0LCom.h"

static packetHead* allocPack(udword udwSize)
{
	auto pRet = (packetHead*)(new char [sizeof(packetHead) + sizeof(netPacketHead) + udwSize]);
	auto pN = P2NHead(pRet);
	pN->udwLength = udwSize;
	pN->uwTag = 0;
	return pRet;
}

static void	freePack(packetHead* pack)
{
	delete [] ((char*)(pack));
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
	/*
	rTrace (__FUNCTION__<<" msgID = "<<pNH->uwMsgID
			<<" ubySrcServId = "<<(int)(pNH->ubySrcServId)<<" ubyDesServId= "<<(int)(pNH->ubyDesServId)
			<<" SP = "<<(int)(ubySp)<<" DP = "<<(int)(ubyDp));
			*/
	if (ubySp == ubyDp) {
		auto pServerS = rMgr.getServerS();
		pS = pServerS[ubyDl];
		//rTrace ("send pack did = "<<(int)ubyDl<<" pS = "<<pS);
	} else {
		pS = rMgr.getOutServer ();
		//auto hs = pS->myHandle ();
		//rTrace ("will send by up pS = "<<pS<<" handle = "<<(int)hs);
	}
	if (pS) {
		//rTrace ("will pushPack");
		pS->pushPack (pack);
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

server*   serverMgr::      getServer(loopHandleType handle)
{
	server* pRet = nullptr;
	loopHandleType  pid = 0;
	loopHandleType  sid = 0;
	fromHandle (handle, pid, sid);
	pRet = g_serverS[sid];
	return pRet;
}
int serverMgr::initFun (int cArg, const char* argS[])
{
	std::cout<<"start main"<<std::endl;
	int nRet = 0;
	procArgS (cArg, argS);
	auto& rMgr = getPhyCallback();
	rMgr.fnSendPackToLoop = sendPackToLoop;
	rMgr.fnStopLoopS = stopLoopS;
	rMgr.fnAllocPack = allocPack;
	rMgr.fnFreePack = freePack;
	rMgr.fnLogMsg = logMsg;
	//rMgr.fnAddComTimer = sAddComTimer;// Thread safety
	do {
		auto nInitLog = initLog ("appFrame", "appFrameLog.log", 0);
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			break;
		}
		auto nInitMidFrame = InitMidFrame(cArg, argS, &rMgr);
		if (0 != nInitMidFrame) {
			nRet = 1;
			rError ("InitMidFrame error nRet =  "<<nInitMidFrame);
			break;
		}
		rInfo ("InitMidFrame end");
		const auto c_maxLoopNum = 10;
		serverNode loopHandleS[c_maxLoopNum];
		auto loopNum =  getAllLoopAndStart(loopHandleS, c_maxLoopNum);
		rInfo ("initFun loopNum = "<<loopNum);
		if (loopNum > 0) {
			auto pNetLibName = m_netLibName.get();
			rTrace ("will int net "<<pNetLibName);
			auto nNetInit = initNetServer (pNetLibName);
			if (nNetInit) {
				rError ("initNetServer error nNetInit = "<<nNetInit);
				break;
			}
			g_ServerNum = loopNum;
			g_serverS = std::make_unique<pserver[]>(loopNum);
			std::unique_ptr<server[]>	pServerImpS =  std::make_unique<server[]>(loopNum);
			auto pServerS = getServerS();
			auto pImpS = pServerImpS.get();
			for (int i = 0; i < loopNum; i++ ) {
				pServerS[i] = &pImpS[i];
				auto loopH = loopHandleS [i].handle;
				auto p = pServerS [i];
				p->init (&loopHandleS[i]);
			}
			for (int i = 0; i < loopNum; i++ ) {
				auto p = pServerS[i];
				p->start();
			}
			for (int i = 0; i < loopNum; i++ ) {
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
