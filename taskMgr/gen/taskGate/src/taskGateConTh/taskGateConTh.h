#ifndef _taskGateConTh_h__
#define _taskGateConTh_h__

#include <memory>
#include "mainLoop.h"
#include "gateCommonWorker.h"


class taskGateConTh:public gateCommonWorker
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
