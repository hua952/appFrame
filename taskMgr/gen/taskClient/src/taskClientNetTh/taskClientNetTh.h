#ifndef _taskClientNetTh_h__
#define _taskClientNetTh_h__

#include <memory>
#include "mainLoop.h"
#include "SingleConnectorWorker.h"


class taskClientNetTh:public SingleConnectorWorker
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
