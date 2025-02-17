#ifndef _taskServerWorkerMgr_h__
#define _taskServerWorkerMgr_h__

#include <memory>
#include "mainLoop.h"
#include "logicWorkerMgr.h"
#include "taskServerConTh.h"
#include "taskServerNetTh.h"


class taskServerWorkerMgr:public logicWorkerMgr
{
public:
	int initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS) override;
    std::pair<std::unique_ptr<taskServerConTh[]>, loopHandleType>      m_taskServerConTh;
    std::pair<std::unique_ptr<taskServerNetTh[]>, loopHandleType>      m_taskServerNetTh;

private:
};
#endif
