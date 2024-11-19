#include "gateCommonWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"

gateCommonWorker:: gateCommonWorker ()
{
}

gateCommonWorker:: ~gateCommonWorker ()
{
}
int   gateCommonWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
    int   nRet = 0;
    do {
		serverIdType	ubyDesServId = appGroupId;
		ubyDesServId <<= 8;
		ubyDesServId |= threadGroupId;
		if (c_emptyLoopHandle == ubyDesServId) {
			auto& rMgr = logicWorkerMgr::getMgr();
			auto forLogicFun  = rMgr.forLogicFun ();
			auto& rConfig = tSingleton<logicFrameConfig>::single();
			auto routeGroupId = rConfig.routeGroupId ();
			auto& routeGroup = rConfig.serverGroups ()[routeGroupId];
			myAssert (routeGroup.runNum);
			auto objId = routeGroup.beginId;
			if (routeGroup.runNum > 1) {
				objId += rand() % routeGroup.runNum;
			}
			auto pN = P2NHead(pPack);

			pN->ubySrcServId = rConfig.appGroupId ();
			pN->ubySrcServId <<= 8;
			pN->ubySrcServId |= serverId ();

			pN->ubyDesServId = c_emptyLoopHandle;
			pPack->loopId = serverId ();
			forLogicFun->fnPushPackToServer (objId, pPack);
		} else {
			nRet = logicWorker::sendPacket(pPack, appGroupId, threadGroupId);
		}
    } while (0);
    return nRet;
}

