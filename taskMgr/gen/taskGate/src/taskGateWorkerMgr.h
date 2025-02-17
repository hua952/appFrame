#ifndef _taskGateWorkerMgr_h__
#define _taskGateWorkerMgr_h__

#include <memory>
#include "mainLoop.h"
#include "logicWorkerMgr.h"
#include "taskGateConTh.h"
#include "taskGateNetTh.h"


class taskGateWorkerMgr:public logicWorkerMgr
{
public:
	int initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS) override;
    std::pair<std::unique_ptr<taskGateConTh[]>, loopHandleType>      m_taskGateConTh;
    std::pair<std::unique_ptr<taskGateNetTh[]>, loopHandleType>      m_taskGateNetTh;

private:
};
#endif
