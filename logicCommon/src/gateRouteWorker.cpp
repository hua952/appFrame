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
	regAppRouteRetMsg ret;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	pRet = ret.pop();
	auto pU = (regAppRouteRet*)(P2User(pRet));
	auto pAU = (regAppRouteAsk*)(P2User(pAsk));
	pU->m_result = (ubyte)(pGate->regAppRoute (pAU->m_appGrupId, pAsk->sessionID));
	gInfo(" rec AppRouteAsk app group id is : "<<(int)(pAU->m_appGrupId));
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
		pSession->setUserData (&key, sizeof(key));
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
			pSession = getOnceAppSession(pN->ubyDesServId>>8);
			if (!pSession) {
				gWarn("can not find app session for pack :"<<*pPack);
				rRet.m_result = 1;
				break;
			}
		}
		pSession->send (pPack);
		rRet.m_result = 0;
    } while (0);
    return nRet;
}

ISession*   gateRouteWorker:: getOnceAppSession(ubyte appGroup)
{
	ISession*   nRet = nullptr;
	do {
		uword fk = appGroup;
		fk<<=8;
		auto endK = fk;
		endK += 254;
		auto& rAppMap = m_appMap;
		auto lower = rAppMap.lower_bound(fk); 
		auto upper = rAppMap.upper_bound(endK);
		std::vector<ISession*> vec;
		for (auto it = lower; it != upper; it++) {
			vec.push_back(it->second);
		}
		myAssert (!vec.empty());
		if (vec.empty()) {
			break;
		}
		auto n = rand()%vec.size();
		nRet = vec[n];
	} while (0);
	return nRet;
}

void  gateRouteWorker:: onClose(ISession* session)
{
    do {
		auto pKey = (uword*)(session->userData());
		if (pKey ) {
			auto key = *(pKey);
			auto& rAppMap = m_appMap;
			rAppMap.erase(key);
		}
    } while (0);
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

int   gateRouteWorker:: recPacketProcFun (ForLogicFun* pForLogic)
{
    int   nRet = 0;
    do {
		auto nR = routeWorker::recPacketProcFun (pForLogic);
		if (nR) {
			nRet = 1;
			break;
		}
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnRegMsg = sMgr.forLogicFun()->fnRegMsg;
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_regAppRouteAsk), regAppRouteAskProcFun);
    } while (0);
    return nRet;
}

static bool sDelToken(void* pU)
{
	auto ppT = (uqword*)(pU);
	auto pT = (gateRouteWorker*)(ppT[0]);
	auto token = (NetTokenType)(ppT[1]);

	auto& rMap = pT->getTokenMap ();
	auto erRet = rMap.erase(token);
	myAssert (!erRet);
	if (erRet) {
		gWarn(" token delete by timer token is : "<<token);
	}
	return false;
}

gateRouteWorker::tokenMap&   gateRouteWorker:: getTokenMap ()
{
    return m_tokenMap;
}
/*
ISession*  gateRouteWorker:: onRecHeadIsNeetForward(ISession* session, netPacketHead* pN)
{
    ISession*  nRet = nullptr;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rTokenMap = m_tokenMap;
		auto& sMgr = logicWorkerMgr::getMgr();

		if (c_emptyLoopHandle == pN->ubyDesServId) {
			break;
		}
		auto objG = pN->ubyDesServId;
		objG>>=8;
		auto myG = rConfig.appGroupId ();
		if (objG == myG) {
			break;
		}
		if (NIsRet(pN)) {
			auto it = rTokenMap.find (pN->dwToKen);
			if (rTokenMap.end() == it) {
				gWarn(" can not find old token netPacketHead is : "<<*pN);
				break;
			}
			pN->dwToKen = it->second.oldToken;
			nRet = getSession (it->second.sessionId);
		} else {
			auto pKey = (uword*)(session->userData());
			myAssert (!pKey);
			tokenSaveInfo info;
			info.oldToken = pN->dwToKen;
			info.sessionId = session->id();
			pN->dwToKen = newToken();
			auto inRet = rTokenMap.insert(std::make_pair(pN->dwToKen, info));
			myAssert (inRet.second);
			if (!inRet.second) {
				break;
			}
			nRet = getOnceAppSession (objG);
			if (!nRet) {
				break;
			}

			auto sept = rConfig.delSaveTokenTime ();
			uqword uqwA[2];
			uqwA[0] = (uqword)(this);
			uqwA[1] = pN->dwToKen;
			addTimer(sept, sDelToken, uqwA, sizeof(uqwA));
		}
    } while (0);
    return nRet;
}
*/

int  gateRouteWorker:: processNetPackFun(ISession* session, packetHead* pack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rTokenMap = m_tokenMap;
		auto& sMgr = logicWorkerMgr::getMgr();
		auto pN = P2NHead(pack);
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			break;
		}
		auto objG = pN->ubyDesServId;
		objG>>=8;
		auto myG = rConfig.appGroupId ();
		if (objG == myG) {
			nRet = routeWorker::processNetPackFun (session, pack);
			break;
		}
		ISession* pSendSession = nullptr;
		do {
			if (NIsRet(pN)) {
				auto it = rTokenMap.find (pN->dwToKen);
				if (rTokenMap.end() == it) {
					gWarn(" can not find old token netPacketHead is : "<<*pN);
					break;
				}
				pN->dwToKen = it->second.oldToken;
				pSendSession = getSession (it->second.sessionId);
				rTokenMap.erase(it);
			} else {
				auto pKey = (uword*)(session->userData());
				myAssert (!pKey);  /*   一定是client的session     */
				tokenSaveInfo info;
				info.oldToken = pN->dwToKen;
				info.sessionId = session->id();
				pN->dwToKen = newToken();
				auto inRet = rTokenMap.insert(std::make_pair(pN->dwToKen, info));
				myAssert (inRet.second);
				if (!inRet.second) {
					break;
				}
				pSendSession = getOnceAppSession (objG);
				if (!pSendSession) {
					break;
				}
				auto sept = rConfig.delSaveTokenTime ();
				uqword uqwA[2];
				uqwA[0] = (uqword)(this);
				uqwA[1] = pN->dwToKen;
				addTimer(sept, sDelToken, uqwA, sizeof(uqwA));
			}
			if (pSendSession) {
				pSendSession->send(pack);
				nRet = procPacketFunRetType_doNotDel;
			} else {
				gWarn(" can not find session to forward pack is : "<<*pack);
			}
		} while (0);
    } while (0);
    return nRet;
}

