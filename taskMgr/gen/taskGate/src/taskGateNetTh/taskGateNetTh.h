#ifndef _taskGateNetTh_h__
#define _taskGateNetTh_h__

#include <memory>
#include "mainLoop.h"
#include "gateRouteWorker.h"


class taskGateNetTh:public gateRouteWorker
{
public:
	int onWorkerInitGen(ForLogicFun* pForLogic) override;
	int onWorkerInit(ForLogicFun* pForLogic) override;
	int onLoopBegin() override;
	int onLoopEnd() override;
	int onLoopFrame() override;
	int onCreateChannelRet(const channelKey& rKey, udword result) override;
	
private:
};
#endif
