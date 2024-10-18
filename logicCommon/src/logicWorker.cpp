#include <cmath>
#include "logicWorker.h"
#include "logicWorkerMgr.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"

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
	/*   该函数目前只发Ask包     */
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
			auto objId = c_emptyLocalServerId;
			auto routeGroupId = rConfig.routeGroupId ();
			if (c_emptyLoopHandle == pPack->loopId) {
				auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
				ubyte ubyI = 0;
				if (routeGroup.runNum) {
					ubyI = rand() % routeGroup.runNum;
				}
				objId = routeGroup.beginId + ubyI;
			} else {
				objId = pPack->loopId;
			}
			
			auto forLogicFun  = rMgr.forLogicFun ();
			auto pN = P2NHead(pPack);
			if (!NIsRet(pN)) {
				pN->dwToKen = forLogicFun->fnNextToken (serverId ());
			}

			pN->ubySrcServId = rConfig.appGroupId ();
			pN->ubySrcServId <<= 8;
			pN->ubySrcServId |= serverId ();
			pN->ubyDesServId = appGroupId;
			pN->ubyDesServId <<= 8;
			pN->ubyDesServId |= threadGroupId;

			sendPackToRemoteAskMsg ask;
			auto pAsk = ask.pop ();
			pAsk->pAsk = (decltype(pAsk->pAsk))(pPack);

			pAsk->loopId = serverId ();
			auto pAskN = P2NHead(pAsk);
			pAskN->ubySrcServId = serverId ();
			pAskN->ubyDesServId = routeGroupId;

			forLogicFun->fnPushPackToServer (objId, pAsk);
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

int  logicWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		nRet = onLoopBegin ();
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopEndBase()
{
    int  nRet = 0;
    do {
		nRet = onLoopEnd ();
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopFrameBase()
{
    int  nRet = 0;
    do {
		nRet = onLoopFrame ();
    } while (0);
    return nRet;
}


static int sendPackToRemoteRetProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;

	auto pU = (sendPackToRemoteRet*)(P2User(pRet));
	myAssert (0 == pU->m_result);
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);

	auto nR = pServer->onSendPackToRemoteRet ((pPacketHead )(pAsk->pAsk));
	if (procPacketFunRetType_doNotDel & nR) {
		pAsk->pAsk = 0;
	}
	return nRet;
}

static int recRemotePackForYouAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto nR = pServer->onRecRemotePackNtf ((pPacketHead )(pAsk->pAsk));
	if (procPacketFunRetType_doNotDel & nR) {
		pAsk->pAsk = 0;
	}
	return nRet;
}


static bool sDelToken (void* pU)
{
	auto ppU = (void**)pU;
    auto pT = (logicWorker*)(ppU[0]);
	auto token = (NetTokenType)(ppU[1]);
	pT->delSendPack (token);
	return false;
}

int   logicWorker:: onRecRemotePackNtf (packetHead* pPack)
{
    int   nRet = procPacketFunRetType_del;
    do {
		auto&  workerMgr = logicWorkerMgr::getMgr();
		auto forLogicFun = workerMgr.forLogicFun ();
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto fnFindMsg = forLogicFun->fnFindMsg;
		auto pN = P2NHead(pPack);
		auto procFun = fnFindMsg(serverId(), pN->uwMsgID);
		procPacketArg arg;
		arg.handle = serverId ();
		if (!fnFindMsg) {
			break;
		}

		if (NIsRet(pN)) {
			auto& rTokenMap = m_tokenMap;
			auto it = rTokenMap.find (pN->dwToKen);
			if (rTokenMap.end () != it) {
				nRet = procFun(it->second, pPack, &arg);
			}
		} else {
			packetHead* pRet = nullptr;
			nRet = procFun(pPack, pRet, &arg);
			if (pRet) {
				pRet->loopId = pPack->loopId;
				pRet->sessionID = pPack->sessionID;
				auto pRN = P2NHead(pRet);
				pRN->dwToKen = pN->dwToKen;
				pRN->ubyDesServId = pN->ubySrcServId;

				sendPackToRemoteAskMsg ask;
				auto pAsk = ask.pop ();
				pAsk->pAsk = (decltype(pAsk->pAsk))(pRet);

				pAsk->loopId = serverId ();
				auto pAskN = P2NHead(pAsk);
				pAskN->ubySrcServId = serverId ();
				pAskN->ubyDesServId = rConfig.routeGroupId ();
				forLogicFun->fnPushPackToServer(pPack->loopId, pAsk);
			}
		}
    } while (0);
    return nRet;
}

int logicWorker:: onSendPackToRemoteRet (packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
    do {
		auto pN = P2NHead(pPack);
		if (NIsRet(pN)) {
			break;
		}
		m_tokenMap[pN->dwToKen] = pPack;
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto sept = rConfig.delSaveTokenTime ();
		void* args[2];
		args[0] = this;
		args[1] = (void*)(pN->dwToKen);
		addTimer (sept, sDelToken, args, sizeof(args));
		nRet = procPacketFunRetType_doNotDel;
    } while (0);
	return nRet;
}

int   logicWorker:: recPacketProcFun (ForLogicFun* pForLogic)
{
    int   nRet = 0;
    do {
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnRegMsg = sMgr.forLogicFun ()->fnRegMsg;
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sendPackToRemoteRet), sendPackToRemoteRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_recRemotePackForYouAsk), recRemotePackForYouAskProcFun);

    } while (0);
    return nRet;
}

void   logicWorker:: delSendPack (NetTokenType  token)
{
    do {
		auto it = m_tokenMap.find (token);
		if (m_tokenMap.end() != it) {
			auto& smgr = logicWorkerMgr::getMgr();
			smgr.forLogicFun()->fnFreePack (it->second);
			m_tokenMap.erase(it);
		}
    } while (0);
}

