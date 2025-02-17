#ifndef _taskServerConTh_h__
#define _taskServerConTh_h__

#include <memory>
#include "mainLoop.h"
#include "serverCommonWorker.h"


class taskServerConTh:public serverCommonWorker
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
