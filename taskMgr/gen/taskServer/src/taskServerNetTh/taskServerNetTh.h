#ifndef _taskServerNetTh_h__
#define _taskServerNetTh_h__

#include <memory>
#include "mainLoop.h"
#include "serverRouteWorker.h"


class taskServerNetTh:public serverRouteWorker
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
