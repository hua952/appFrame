#include <vector>
#include "gateRouteWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"

gateRouteWorker:: gateRouteWorker ()
{
}

gateRouteWorker:: ~gateRouteWorker ()
{
}

int  gateRouteWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		int nR = 0;	
		endPoint endP[2];
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto appIndex = rConfig.appIndex ();
		auto gateNodeNum = rConfig.gateNodeNum ();
		if (appIndex >= gateNodeNum) {
			nRet = 1;
			break;
		}
		myAssert (appIndex < gateNodeNum);
		auto gateNodes = rConfig.gateNodes ();
		auto& gateNode = gateNodes[appIndex];
		myAssert (gateNode.endPointNum <= 2);
		auto serverGroup = this->serverGroup ();
		auto serverGroupNum = rConfig.serverGroupNum ();
		myAssert (serverGroup < serverGroupNum);
		auto serverGroups = rConfig.serverGroups ();
		auto& rRoutGroup = serverGroups[serverGroup];
		auto serId = this->serverId();
		myAssert (serId >= rRoutGroup.beginId);
		auto serIndex = serId - rRoutGroup.beginId;
		myAssert (serIndex < rRoutGroup.runNum);
		for (decltype (gateNode.endPointNum) i = 0; i < gateNode.endPointNum; i++) {
			strNCpy (endP[i].ip, sizeof(endP[i].ip), gateNode.endPoints[i].first.get());
			endP[i].port = gateNode.endPoints[i].second + serIndex;
			endP[i].ppIConnector = nullptr;
			endP[i].userDataLen = 0;
			endP[i].userData = nullptr;
		}
	
		nR = initNet (endP, gateNode.endPointNum, nullptr, 0, nullptr, 0);
		if (nR) {
			gError("initNet error nR = "<<nR<<" endpoint[0] = "<<endP[0].ip<<" : "<<endP[0].port);
			nRet = 1;
			break;
		}
		nR = logicWorker:: onLoopBeginBase();
		if (nR) {
			nRet = 2;
			break;
		}
    } while (0);
    return nRet;
}

static int regAppRouteAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	sendPackToRemoteRetMsg ret;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	pRet = ret.pop();
	auto pU = (regAppRouteRet*)(P2User(pRet));
	auto pAU = (regAppRouteAsk*)(P2User(pAsk));
	pU->m_result = (ubyte)(pGate->regAppRoute (pAU->m_appGrupId, pAsk->sessionID));

	return nRet;
}

int   gateRouteWorker:: regAppRoute (ubyte group, SessionIDType sessionId)
{
    int   nRet = 0;
    do {
		auto& rAppMap = m_appMap;
		uword fk = group;
		fk<<=8;
		auto endK = fk;
		endK += 254;

		auto lower = rAppMap.lower_bound(fk); 
		auto upper = rAppMap.upper_bound(endK);
		ubyte last = 0;
		for (auto it = lower; it != upper; it++) {
			last = (ubyte)(it->first);
		}
		last++;
		auto key = fk;
		key += last;
		auto pSession = getSession (sessionId);
		myAssert (pSession);
		if (!pSession) {
			nRet = 1;
			break;
		}
		rAppMap[key] = pSession;
    } while (0);
    return nRet;
}

int  gateRouteWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet)
{
    int  nRet = 0;
    do {
		auto& rAppMap = m_appMap;
		auto pN = P2NHead(pPack);
		ISession* pSession = nullptr;
		if (NIsRet(pN)) {
			pSession = getSession (pPack->sessionID);
			if (!pSession) {
				logicWorkerMgr::getMgr().forLogicFun ()->fnFreePack(pPack);
				nRet = 1;
				break;
			}
		} else {
			uword fk = pN->ubyDesServId;
			fk&=0xff;
			auto endK = fk;
			endK += 254;
			auto lower = rAppMap.lower_bound(fk); 
			auto upper = rAppMap.upper_bound(endK);
			std::vector<ISession*> vec;
			for (auto it = lower; it != upper; it++) {
				vec.push_back(it->second);
			}
			myAssert (!vec.empty());
			if (vec.empty()) {
				rRet.m_result = 1;
				break;
			}
			auto n = rand()%vec.size();
			pSession = vec[n];
		}
		pSession->send (pPack);
		rRet.m_result = 0;
    } while (0);
    return nRet;
}

int  gateRouteWorker:: localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc)
{
    int  nRet = 0;
    do {
		auto baseProc = false;
		auto nR = routeWorker::localProcessNetPackFun(session, pack, baseProc);
		if (baseProc) {
			nRet = nR;
			break;
		}
		auto pN = P2NHead(pack);
		myAssert (c_emptyLoopHandle == pN->ubyDesServId);

    } while (0);
    return nRet;
}

