#include <cmath>
#include "logicWorker.h"
#include "logicWorkerMgr.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"

logicWorker:: logicWorker ()
{
}

logicWorker:: ~logicWorker ()
{
}

int          logicWorker:: sendMsg (CMsgBase& rMsg)
{
	auto pack = rMsg.pop();
    return sendPacket (pack);
}

int          logicWorker:: sendMsg (CMsgBase& rMsg, loopHandleType appGroupId, loopHandleType threadGroupId)
{

	auto pack = rMsg.pop();
    return sendPacket (pack, appGroupId, threadGroupId);
}

int   logicWorker:: sendPacket (packetHead* pPack)
{
    int   nRet = 0;
    do {
		auto& rMgr = logicWorkerMgr::getMgr();
		auto forLogicFun  = rMgr.forLogicFun ();
		ubyte appG;
		ubyte serverG;
		auto pN = P2NHead(pPack);
		auto bF = rMgr.findDefProc (pN->uwMsgID, appG, serverG);
		if (!bF) {
			gError("can not find def proc pack: "<<pPack);
			forLogicFun->fnFreePack (pPack);
			break;
		}
		nRet = sendPacket (pPack, appG, serverG);
    } while (0);
    return nRet;
}

int   logicWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& rMgr = logicWorkerMgr::getMgr();
		auto myAppGroupId = rConfig.appGroupId ();
		auto serverGroups = rConfig.serverGroups ();
		auto forLogicFun  = rMgr.forLogicFun ();
		if (myAppGroupId == appGroupId) {
			nRet = pushPacketToLocalServer (pPack, threadGroupId);
		} else {
		}
    } while (0);
    return nRet;
}

int   logicWorker:: pushPacketToLocalServer(packetHead* pPack, loopHandleType threadGroupId)
{
	int   nRet = 0;
	do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& rMgr = logicWorkerMgr::getMgr();
		auto serverGroups = rConfig.serverGroups ();
		auto serverGroupNum = rConfig.serverGroupNum ();
		auto forLogicFun  = rMgr.forLogicFun ();
		myAssert (threadGroupId < serverGroupNum);
		if (threadGroupId >= serverGroupNum) {
			gError("error thread group id threadGroupId : "<<threadGroupId<<" pack: "<<pPack);
			forLogicFun->fnFreePack (pPack);
			break;
		}
		auto& rGroup = serverGroups[threadGroupId];
		auto pN = P2NHead(pPack);
		pN->ubyDesServId = threadGroupId;
		ubyte ubyI = 0;
		if (rGroup.runNum > 1) {
			ubyI = rand() % rGroup.runNum;
		}
		pN->ubySrcServId = serverId ();
		pPack->loopId = serverId ();
		forLogicFun->fnPushPackToServer (rGroup.beginId + ubyI, pPack);
	} while (0);
	return nRet;
}

ubyte  logicWorker:: serverId ()
{
    return m_serverId;
}

void  logicWorker:: setServerId (ubyte v)
{
    m_serverId = v;
}

int   logicWorker:: processOncePack(packetHead* pPack)
{
    int   nRet = procPacketFunRetType_del;
    return nRet;
}

int  logicWorker:: onLoopBegin()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopEnd()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopFrame()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onWorkerInitGen(ForLogicFun* pForLogic)
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onWorkerInit(ForLogicFun* pForLogic)
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

ubyte  logicWorker:: serverGroup ()
{
    return m_serverGroup;
}

void  logicWorker:: setServerGroup (ubyte v)
{
    m_serverGroup = v;
}

bool  logicWorker:: willExit ()
{
    return m_willExit;
}

void  logicWorker:: setWillExit (bool v)
{
    m_willExit = v;
}

void  logicWorker:: addTimer(udword udwSetp, ComTimerFun pF, void* pUsrData)
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

void  logicWorker:: addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData)
{
    do {
		void* pB[2];
		pB[0] = (void*)pF;
		pB[1] = pUserData;
		addTimer (firstSetp, udwSetp, sTimerProc, pB, sizeof (pB));
    } while (0);
}

void  logicWorker:: addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength)
{
    do {
		auto fnAddComTimer =  getForMsgModuleFunS ().fnAddComTimer;
		auto handle = serverId ();
		fnAddComTimer (handle, firstSetp, udwSetp, pF, pUserData, udwLength);
    } while (0);
}

void logicWorker::addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength)
{
	addTimer (udwSetp, udwSetp, pF, pUserData, udwLength);
}

