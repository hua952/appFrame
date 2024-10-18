#include "routeWorker.h"
#include "strFun.h"
#include "logicWorkerMgr.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"
#include "tSingleton.h"
#include "logicFrameConfig.h"

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
	auto pSend = (pPacketHead)(pAsk->pAsk);
	pPacketHead pNew = nullptr;
	auto pSN = P2NHead(pSend);
	workerMgr.toNetPack ( pSN, pNew);
	if (!pNew) {
		pNew = nClonePack (pSN);
	}
	pNew->loopId = pSend->loopId;
	pNew->sessionID = pSend->sessionID;
	pRoute->sendPackToRemoteAskProc(pNew, *pU);
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
	pRet->pAsk = 0;
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
	pRet->pAsk = 0;
	pRet->sessionID = EmptySessionID;
	auto pRN = P2NHead(pRet);
	auto udwLength = pN->udwLength;
	*pRN++ = *pN++;
	if (udwLength ) {
		memcpy (pRN, pN, udwLength);
	}
	return pRet;
}


int  routeWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet)
{
    int  nRet = 0;
    do {
		rRet.m_result = 1;
    } while (0);
    return nRet;
}

int routeWorker:: processNetPackFun(ISession* session, packetHead* pack)
{
    int nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& sMgr = logicWorkerMgr::getMgr();
		bool bProc = false;
		auto pN = P2NHead(pack);
		packetHead* pNew = nullptr;
		sMgr.fromNetPack (pN, pNew);
		if (pNew) {
			auto  nR = localProcessNetPackFun (session, pNew, bProc);
			if (!(nR & procPacketFunRetType_doNotDel)) {
				auto fnFreePack = sMgr.forLogicFun()->fnFreePack;
				fnFreePack (pNew);
			}
		} else {
			nRet = localProcessNetPackFun (session, pack, bProc);
		}
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

int  routeWorker:: localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc)
{
    int  nRet = procPacketFunRetType_del;
    do {
		bProc = false;
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto appGroupId = rConfig.appGroupId ();
		auto pN = P2NHead(pack);
		auto objAppG = pN->ubyDesServId;
		objAppG>>=8;
		if (appGroupId == objAppG) {
			auto objServerGroup = (ubyte)(pN->ubyDesServId);
			auto serverGroup = this->serverGroup ();
			bProc = true;
			pack->sessionID = session->id ();
			pack->loopId = serverId();
			recRemotePackForYouAskMsg  ask;
			auto pAsk = ask.pop ();
			pAsk->pAsk= (decltype(pAsk->pAsk))(pack);

			if (serverGroup == objServerGroup) {
				/*   发给路由线程的就本线程处理了, 本线程就是路由线程  */
				auto& sMgr = logicWorkerMgr::getMgr();
				sMgr.forLogicFun()->fnPushPackToServer (serverId(), pAsk);
			} else {
				pushPacketToLocalServer (pAsk, (ubyte)(pN->ubyDesServId));
			}
			nRet = procPacketFunRetType_doNotDel;
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

