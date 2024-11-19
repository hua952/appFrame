#include "serverRouteWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"


serverRouteWorker:: serverRouteWorker ()
{
}

serverRouteWorker:: ~serverRouteWorker ()
{
}

int  serverRouteWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto netNum = rConfig.netNum ();
		myAssert (netNum);
		m_connectors = std::make_unique<ISession*[]>(netNum);
		auto endPoints = std::make_unique<endPoint []>(netNum);

		auto serverGroup = this->serverGroup ();
		auto serverGroupNum = rConfig.serverGroupNum ();
		myAssert (serverGroup < serverGroupNum);
		auto serverGroups = rConfig.serverGroups ();
		auto& rRoutGroup = serverGroups[serverGroup];

		auto serId = this->serverId();
		myAssert (serId >= rRoutGroup.beginId);
		auto serIndex = serId - rRoutGroup.beginId;
		myAssert (serIndex < rRoutGroup.runNum);

		auto gateNodeNum = rConfig.gateNodeNum ();
		myAssert (serIndex < gateNodeNum);
		auto gateNodes = rConfig.gateNodes ();
		auto& gateNode = gateNodes[serIndex];
		myAssert (gateNode.endPointNum <= 2);
		auto& rEndPoint = gateNode.endPoints[gateNode.endPointNum - 1];  /*    服务器因连最后一个endpoint    */
		for (decltype (netNum) i = 0; i < netNum; i++) {
			strNCpy (endPoints[i].ip, sizeof(endPoints[i].ip), rEndPoint.first.get());
			endPoints[i].port = rEndPoint.second + i;
			endPoints[i].ppIConnector = &(m_connectors.get()[i]);
			endPoints[i].userDataLen = 0;
			endPoints[i].userData = nullptr;
		}
	
		auto nR = initNet (nullptr, 0, endPoints.get(), netNum, nullptr, 0);
		if (nR) {
			gError("initNet error nR = "<<nR<<" endpoint[0] = "<<endPoints[0].ip<<" : "<<endPoints[0].port);
			nRet = 1;
			break;
		}
		for (decltype (netNum) i = 0; i < netNum; i++) {
			regAppRouteAskMsg ask;
			auto pAsk = ask.pop();
			auto pN = P2NHead(pAsk);
			
			auto pU =(regAppRouteAsk*)N2User(pN);
			pU->m_appGrupId = rConfig.appGroupId ();
			auto sessionId = m_connectors[i]->id();
			pAsk->loopId = serverId();
			pAsk->sessionID = sessionId;

			serverIdType	ubyDesServId = c_emptyLoopHandle;
			auto serverG = (ubyte)(ubyDesServId );
			ubyDesServId >>= 8;
			auto appG = (ubyte)(ubyDesServId);
			sendPacket(pAsk, appG, serverG);

			// sendBroadcastPack (pAsk/*, sessionId*/);
			// sendPacket (pAsk, rConfig.gateAppGroupId (), rConfig.gateRouteServerGroupId (), m_connectors[i]->id());
			gDebug("send regAppRouteAskMsg sessionId is : "<<sessionId<<" my id is : " <<(int)(serverId())<<" serIndex = "<<(int)serIndex<<" packHead is : "<<*pAsk);
		}
		nR = routeWorker:: onLoopBeginBase();
		if (nR) {
			nRet = 2;
			break;
		}
    } while (0);
    return nRet;
}

int  serverRouteWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession)
{
    int  nRet = 0;
    do {
		auto&  workerMgr = logicWorkerMgr::getMgr();
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto netNum = rConfig.netNum ();
		auto pN = P2NHead(pPack);
		ISession* pS = nullptr;
		rRet.m_result = 0;
		if (NIsRet(pN)) {
			pS = getSession (pPack->sessionID);
		} else {
			if (objSession == EmptySessionID) {
				if (serverRouteWorkerState_unReg == m_state) {
					m_unRegSendV->push_back(pPack);
					gTrace( " when unReg state rec sendPackToRemoteAsk pack is : "<<*pPack);
					break;
				}
				auto netI = 0;
				if (netNum > 1) {
					netI = rand() % netNum;
				}
				pS = m_connectors[netI];
			} else {
				pS = getSession (objSession);
			}
		}
		myAssert (pS);
		if (pS) {
			pS->send (pPack);
		} else {
			gWarn(" can not find session sessionID = "<<objSession<<" pack is : "<<*pPack);
			auto fnFreePack = workerMgr.forLogicFun()->fnFreePack;
			fnFreePack (pPack);
		}
    } while (0);
    return nRet;
}


static int regAppRouteRetProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pSer = dynamic_cast<serverRouteWorker*>(pServer);
	myAssert (pSer);
	pSer->onRecRegAppRet ();
	return nRet;
}

void   serverRouteWorker:: onRecRegAppRet ()
{
    do {
		m_regRetNum++;
		gDebug(" rec  regAppRouteRet cur regRetNum is "<<(int)(m_regRetNum));
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto netNum = rConfig.netNum();
		if (m_regRetNum == netNum) {
			gInfo(" all reg ret ");
			m_state = serverRouteWorkerState_Reged;
			auto& rV = *(m_unRegSendV.get());
			for (auto it = rV.begin (); it != rV.end (); it++) {
				auto netI = 0;
				if (netNum > 1) {
					netI = rand() % netNum;
				}
				auto pS = m_connectors[netI];
				pS->send(*it);
			}
			m_unRegSendV.reset();
		}
    } while (0);
}

void   serverRouteWorker:: sendHeartbeat ()
{
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto netNum = rConfig.netNum();
		for (decltype (netNum) i = 0; i < netNum; i++) {
			heartbeatAskMsg msg;
			auto pack = msg.pop();
			auto pS = m_connectors[i];
			pS->send(pack);
		}
    } while (0);
}

int   serverRouteWorker:: recPacketProcFun (ForLogicFun* pForLogic)
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
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_regAppRouteRet), regAppRouteRetProcFun);
    } while (0);
    return nRet;
}

/*
int   serverRouteWorker:: sendBroadcastPack (packetHead* pack)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto gateNodeNum = rConfig.gateNodeNum ();
		auto routeGroupId = rConfig.routeGroupId ();
		auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
		myAssert (gateNodeNum == routeGroup.runNum);

		auto nIndex = serverId ();
		nIndex -= routeGroup.beginId;
		auto pSN = P2NHead(pack);
		for (decltype (routeGroup.runNum) i = 0; i < routeGroup.runNum; i++) {
			auto serIndex = nIndex + i;
			serIndex %= routeGroup.runNum;
			serIndex += routeGroup.beginId;
			
			auto pAsk = nClonePack(pSN);
			pAsk->loopId = serIndex;
			auto pAN = P2NHead(pAsk);
			pAN->dwToKen = newToken ();
			serverIdType	ubyDesServId = c_emptyLoopHandle;
			auto serverG = (ubyte)(ubyDesServId );
			ubyDesServId >>= 8;
			auto appG = (ubyte)(ubyDesServId);
			sendPacket(pAsk, appG, serverG);
		}
    } while (0);
    return nRet;
}
*/

int   serverRouteWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
	int   nRet = 0;
	do {
		auto pN = P2NHead(pPack);
		if (c_emptyLoopHandle == pN->ubyDesServId && NIsToAllGate(pN)) {
			auto& rConfig = tSingleton<logicFrameConfig>::single();
			auto gateNodeNum = rConfig.gateNodeNum ();
			auto routeGroupId = rConfig.routeGroupId ();
			auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
			myAssert (gateNodeNum == routeGroup.runNum);

			auto& sMgr = logicWorkerMgr::getMgr();
			auto forLogicFun = sMgr.forLogicFun();
			for (decltype (routeGroup.runNum) i = 0; i < routeGroup.runNum; i++) {

				auto pAsk = routeGroup.runNum - 1 == i ? pPack : nClonePack(pN);
				auto pAN = P2NHead(pAsk);
				pAN->dwToKen = newToken ();

				sendPackToRemoteAskMsg send;
				auto pSend = send.pop ();
				pSend->packArg = (decltype(pAsk->packArg))(pAsk);
				pSend->loopId = serverId ();
				auto pSendN = P2NHead(pSend);
				auto pSendU= (sendPackToRemoteAsk*)(N2User(pSendN));
				pSendU->objSessionId = EmptySessionID;
				pSendN->ubySrcServId = serverId ();
				pSendN->ubyDesServId = routeGroupId;
				forLogicFun->fnPushPackToServer (i, pSend);	
			}
		} else {
			nRet = logicWorker::sendPacket(pPack, appGroupId, threadGroupId);
		}
	} while (0);
	return nRet;
}

