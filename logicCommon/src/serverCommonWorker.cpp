#include "serverCommonWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"

serverCommonWorker:: serverCommonWorker ()
{
}

serverCommonWorker:: ~serverCommonWorker ()
{
}

int  serverCommonWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
	int   nRet = 0;
	do {

		serverIdType	ubyDesServId = appGroupId;
		ubyDesServId <<= 8;
		ubyDesServId |= threadGroupId;
		auto pN = P2NHead(pPack);
		if (c_emptyLoopHandle == ubyDesServId && NIsToAllGate(pN)) {
			pN->ubyDesServId = ubyDesServId;
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
				forLogicFun->fnPushPackToServer (routeGroup.beginId + i, pSend);
			}
		} else {
			nRet = logicWorker::sendPacket(pPack, appGroupId, threadGroupId);
		}
	} while (0);
	return nRet;
}

