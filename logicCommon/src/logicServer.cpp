#include "logicServer.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include <map>
#include "msg.h"

serverNode&  logicServer:: serverInfo ()
{
	return m_serverInfo;
}

const char*  logicServer:: serverName ()
{
	return m_serverName.get ();
}

int logicServer::onFrameFun ()
{
	return procPacketFunRetType_del;
}

void  logicServer::setServerName (const char* v)
{
	strCpy (v, m_serverName);
}

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

ubyte  logicServerMgr::serverNum ()
{
	return  m_serverNum;
}

void   logicServerMgr::setServerNum (ubyte ubyNum)
{
	m_serverNum = ubyNum;
}

logicServer**  logicServerMgr:: serverS ()
{
	return m_serverS.get ();
}

static int OnFrameCli(void* pArgS)
{
	auto pS = (logicServer*)pArgS;
	return pS->onFrameFun ();
}
 
int  logicServerMgr:: procArgS (int nArgC, const char* argS[])
{
	int nRet;
	return nRet;
}

int logicServer::onServerInitGen(ForLogicFun* pForLogic)
{
	int nRet = 0;
	return nRet;
}

int logicServer:: sendMsg (CMsgBase& rMsg)
{
    int  nRet = 0;
    do {
		auto pack = rMsg.toPack ();
		auto nR = sendPack (pack);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

int logicServer:: sendToServer (CMsgBase& rMsg, loopHandleType handle)
{
    int  nRet = 0;
    do {
		auto pack = rMsg.toPack ();
		auto nR = sendPackToServer  (pack, handle);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

int logicServer:: sendPack (packetHead* pack)
{
    int  nRet = 0;
    do {
		auto fnGetDefProcServerId =  getForMsgModuleFunS ().fnGetDefProcServerId;
		auto pN = P2NHead (pack);
		auto handle = fnGetDefProcServerId (pN->uwMsgID);
		nRet = sendPackToServer (pack, handle);
    } while (0);
    return nRet;
}

serverIdType   logicServer:: serverId ()
{
    serverIdType   nRet = 0;
    do {
		nRet = serverInfo ().handle;
    } while (0);
    return nRet;
}

int logicServer:: sendPackToServer (packetHead* pack, loopHandleType handle)
{
    int nRet = 0;
    do {
		auto pN = P2NHead (pack);
		pN->ubySrcServId = serverId ();
		pN->ubyDesServId = handle;
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoop;
		nRet = fnSendPackToLoop (pack);
    } while (0);
    return nRet;
}

void  logicServer:: addTimer(udword udwSetp, ComTimerFun pF, void* pUsrData)
{
    do {
		addTimer (udwSetp, udwSetp, pF, pUsrData);
    } while (0);
}

static bool sTimerProc (void* pUserData)
{
	auto pB = (void**)pUserData;
	auto pF = (ComTimerFun)(pB[0]);
	return pF(pB[1]);
}

void  logicServer:: addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData)
{
    do {
		void* pB[2];
		pB[0] = pF;
		pB[1] = pUserData;
		addTimer (firstSetp, udwSetp, sTimerProc, pB, sizeof (pB));
    } while (0);
}

void  logicServer:: addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength)
{
    do {
		auto fnAddComTimer =  getForMsgModuleFunS ().fnAddComTimer;
		auto handle = serverId ();
		fnAddComTimer (handle, firstSetp, udwSetp, pF, pUserData, udwLength);
    } while (0);
}

void logicServer::addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength)
{
	addTimer (udwSetp, udwSetp, pF, pUserData, udwLength);
}

void  logicServerMgr::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
}
