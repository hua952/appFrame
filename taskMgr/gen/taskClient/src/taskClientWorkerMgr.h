#ifndef _taskClientWorkerMgr_h__
#define _taskClientWorkerMgr_h__

#include <memory>
#include "mainLoop.h"
#include "logicWorkerMgr.h"
#include "taskClientConTh.h"
#include "taskClientNetTh.h"


class taskClientWorkerMgr:public logicWorkerMgr
{
public:
	int initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS) override;
    std::pair<std::unique_ptr<taskClientConTh[]>, loopHandleType>      m_taskClientConTh;
    std::pair<std::unique_ptr<taskClientNetTh[]>, loopHandleType>      m_taskClientNetTh;

private:
};
#endif
