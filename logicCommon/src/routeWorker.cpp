#include "routeWorker.h"
#include "strFun.h"
#include "logicWorkerMgr.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalChannel.h"
#include "internalMsgGroup.h"
#include "tSingleton.h"
#include "logicFrameConfig.h"
#include "gLog.h"

static int sendPackToRemoteAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	sendPackToRemoteRetMsg ret;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pRoute = dynamic_cast<routeWorker*>(pServer);
	pRet = ret.pop();
	auto pU = (sendPackToRemoteRet*)(P2User(pRet));
	auto pSend = (pPacketHead)(pAsk->packArg);
	pPacketHead pNew = nullptr;
	auto pSN = P2NHead(pSend);
	workerMgr.toNetPack ( pSN, pNew);
	if (!pNew) {
		pNew = nClonePack (pSN);
	}
	// pAsk->packArg = (decltype(pAsk->packArg))(pNew);
	pNew->loopId = pSend->loopId;
	pNew->sessionID = pSend->sessionID;
	auto pAU = (sendPackToRemoteAsk*)(P2User(pAsk));
	pRoute->sendPackToRemoteAskProc(pNew, *pU, pAU->objSessionId);
	return nRet;
}
/*
static int sendToAllGateAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pRoute = dynamic_cast<routeWorker*>(pServer);
	myAssert (pRoute);
	sendToAllGateRetMsg ret;
	pRet = ret.pop();
	auto pU = (sendPackToRemoteRet*)(P2User(pRet));
	pU->m_result = 0;
	auto pSend = (pPacketHead)(pAsk->packArg);
	// pPacketHead pNew = nullptr;
	auto pSN = P2NHead(pSend);
	pSN->ubyDesServId = c_emptyLoopHandle;
	
	pRoute->sendBroadcastPack (pSend);
	return nRet;
}
*/
static int exitAppNtfByNetProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_exitNow;
	auto pU = (exitAppNtfByNet*)(P2User(pAsk));
	gInfo(" rec exitAppNtfByNet exitType = "<<(int)(pU->exitType));
	if (pU->exitType == 1) {
		nRet = procPacketFunRetType_exitAfterLoop;
	}

	auto& sMgr = logicWorkerMgr::getMgr();
	auto fnRegMsg = sMgr.forLogicFun()->fnRegMsg;
	auto allServerNum = sMgr.allServerNum ();
	for (decltype (allServerNum) i = 0; i < allServerNum; i++) {
		exitAppNtfMsg  msg;
		auto pExitPack = msg.pop();
		auto pEN = P2NHead(pExitPack);
		pEN->ubySrcServId = pArg->handle;
		pEN->ubyDesServId = i;
		sMgr.forLogicFun()->fnPushPackToServer (i, pExitPack);
	}
	return nRet;
}

routeWorker:: routeWorker ()
{
}

routeWorker:: ~routeWorker ()
{
}

int  routeWorker:: recPacketProcFun (ForLogicFun* pForLogic)
{
    int   nRet = 0;
    do {
		int nR = 0;
		nR = logicWorker::recPacketProcFun (pForLogic);
		if (nR) {
			nRet = 1;
			break;
		}
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnRegMsg = sMgr.forLogicFun()->fnRegMsg;
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sendPackToRemoteAsk), sendPackToRemoteAskProcFun);
		// fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sendToAllGateAsk), sendToAllGateAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_exitAppNtfByNet), exitAppNtfByNetProcFun);
    } while (0);
    return nRet;
}

static packetHead* sallocPacket(udword udwS)
{
	auto& sMgr = logicWorkerMgr::getMgr();
	auto fnAllocPack = sMgr.forLogicFun ()->fnAllocPack;
	
	packetHead* pRet = (packetHead*)(fnAllocPack(AllPacketHeadSize + udwS));
	
	pNetPacketHead pN = P2NHead(pRet);
	memset(pN, 0, NetHeadSize);
	pN->udwLength = udwS;
	pRet->packArg = 0;
	return pRet;
}

static packetHead* sallocPacketExt(udword udwS, udword ExtNum)
{
	auto pRet =  sallocPacket (udwS + sizeof(packetHead) * ExtNum);
	if (ExtNum) {
		pNetPacketHead pN = P2NHead(pRet);
		NSetExtPH(pN);
	}
	return pRet;
}

packetHead* nClonePack(netPacketHead* pN)
{
	udword extNum = NIsExtPH(pN)?1:0;
	auto pRet = sallocPacketExt (pN->udwLength, extNum);
	pRet->packArg = 0;
	pRet->sessionID = EmptySessionID;
	pRet->loopId = c_emptyLocalServerId;
	auto pRN = P2NHead(pRet);
	auto udwLength = pN->udwLength;
	*pRN++ = *pN++;
	if (udwLength ) {
		memcpy (pRN, pN, udwLength);
	}
	return pRet;
}


int  routeWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession)
{
    int  nRet = 0;
    do {
		rRet.m_result = 1;
    } while (0);
    return nRet;
}

int routeWorker::processNetPackFun(ISession* session, packetHead* pack)
{
    int nRet = procPacketFunRetType_del;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& sMgr = logicWorkerMgr::getMgr();
		auto pN = P2NHead(pack);
		if (internal2FullMsg(internalMsgId_heartbeatAsk) == pN->uwMsgID) {
			gTrace(" rec heartbeatAsk");
			nRet |= procPacketFunRetType_stopBroadcast;
			break;
		}
		if (internal2FullMsg(internalMsgId_heartbeatRet) == pN->uwMsgID) {
			gTrace(" rec heartbeatRet");
			nRet |= procPacketFunRetType_stopBroadcast;
			break;
		}
		/*
		packetHead* pNew = nullptr;
		sMgr.fromNetPack (pN, pNew);
		if (pNew) {
			nRet = localProcessNetPackFun (session, pNew);
			if (nRet & procPacketFunRetType_doNotDel) {
				nRet &= ~procPacketFunRetType_doNotDel; // not delete pNew, pack must to be delete
			} else {
				auto fnFreePack = sMgr.forLogicFun()->fnFreePack;
				fnFreePack (pNew); // delete both pNew and pack
			}
		} else {
			nRet = localProcessNetPackFun (session, pack);
		}
		*/
		nRet = localProcessNetPackFun (session, pack);
    } while (0);
    return nRet;
}

void  routeWorker:: onWritePack(ISession* session, packetHead* pack)
{
    do {
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnFreePack = sMgr.forLogicFun()->fnFreePack;
		fnFreePack (pack);
    } while (0);
}

int  routeWorker:: localProcessNetPackFun(ISession* session, packetHead* pack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnFreePack = sMgr.forLogicFun()->fnFreePack;
		auto appGroupId = rConfig.appGroupId ();
		auto pN = P2NHead(pack);
		auto objAppG = pN->ubyDesServId;
		objAppG>>=8;
		if (appGroupId == objAppG) {
			nRet |= procPacketFunRetType_stopBroadcast;
			packetHead* pNew = nullptr;
			if (pN->udwLength) {
				sMgr.fromNetPack (pN, pNew);
				if (pNew) {
					pack = pNew;
					pN = P2NHead(pack);
				}
			}
			ubyte objServerGroup = 0xff;
			bool desIsGroup = !NIsRet(pN) && !NIsDesOnce(pN);
			if (desIsGroup ) {
				objServerGroup = (ubyte)(pN->ubyDesServId);
			} else {
				auto objServerId = (ubyte)(pN->ubyDesServId);
				auto pObjServer = sMgr.findServer(objServerId);
				myAssert (pObjServer);
				if (!pObjServer) {
					gWarn("can net find server id is : "<<(int)(objServerGroup));
					if (pNew ) {
						fnFreePack (pNew);
						pNew = nullptr;
					}
					break;
				}
				objServerGroup  = pObjServer->serverGroup ();
			}
			/*
			auto objServerGroup = (ubyte)(pN->ubyDesServId);
			if (NIsRet(pN)) {
				auto pObjServer = sMgr.findServer(objServerGroup);
				myAssert (pObjServer);
				if (!pObjServer) {
					gWarn("can net find server id is : "<<(int)(objServerGroup));
					if (pNew ) {
						fnFreePack (pNew);
						pNew = nullptr;
					}
					break;
				}
				objServerGroup  = pObjServer->serverGroup ();
			}
			*/
			if (!pNew) {
				nRet |= procPacketFunRetType_doNotDel;
			}
			auto serverGroup = this->serverGroup ();
			pack->sessionID = session->id ();
			pack->loopId = serverId();
			recRemotePackForYouAskMsg  ask;
			auto pAsk = ask.pop ();
			pAsk->packArg = (decltype(pAsk->packArg))(pack);
			if (desIsGroup) {
				if (serverGroup == objServerGroup) {
					/*   发给路由线程的就本线程处理了, 本线程就是路由线程  */
					// gTrace(" rec to gate rout msg pack is"<<*pack);
					sMgr.forLogicFun()->fnPushPackToServer (serverId(), pAsk);
				} else {
					pushPacketToLocalServer (pAsk, (ubyte)(pN->ubyDesServId));
				}
			} else {
				sMgr.forLogicFun()->fnPushPackToServer ((ubyte)(pN->ubyDesServId), pAsk);
			}
			/*
			if (serverGroup == objServerGroup && desIsGroup) {
				// gTrace(" rec to gate rout msg pack is"<<*pack);
				sMgr.forLogicFun()->fnPushPackToServer (serverId(), pAsk);
			} else {
				if (NIsRet(pN)||NIsDesOnce(pN)) {
					sMgr.forLogicFun()->fnPushPackToServer ((ubyte)(pN->ubyDesServId), pAsk);
				} else {
					pushPacketToLocalServer (pAsk, (ubyte)(pN->ubyDesServId));
				}
			}
			*/
		}
    } while (0);
    return nRet;
}

int  routeWorker:: onLoopFrameBase()
{
    auto pNet = tcpServer ();
	if (pNet) {
		pNet->onLoopFrame ();
	}
	return logicWorker::onLoopFrameBase ();
}

static bool sSendHeartbeat (void* pData)
{
	auto pS = (routeWorker*)(pData);
	pS->sendHeartbeat ();
	return true;
}

int  routeWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto  heartbeatSetp = rConfig.heartbeatSetp ();
		addTimer(heartbeatSetp, sSendHeartbeat, this);

		auto routeGroupId = rConfig.routeGroupId ();
		auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
		if (serverId () == routeGroup.beginId) {
			channelKey ch;
			stringToChannel (GM_CHANNEL, ch);
			subscribeChannel (ch);
		}
		nRet = logicWorker::onLoopBeginBase();
    } while (0);
    return nRet;
}

