#ifndef _taskClientConTh_h__
#define _taskClientConTh_h__

#include <memory>
#include "mainLoop.h"
#include "logicWorker.h"


class taskClientConTh:public logicWorker
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
