#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "myAssert.h"
#include "logicWorker.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "gLog.h"
#include "taskClientWorkerMgr.h"

dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic, int nDefArgC, char** defArgS, char* taskBuf, int taskBufSize)
{
	setForMsgModuleFunS (pForLogic);
	tSingleton<taskClientWorkerMgr>::createSingleton();
	auto &rMgr = tSingleton<taskClientWorkerMgr>::single();
	logicWorkerMgr::s_mgr = &rMgr;
	return rMgr.initLogicWorkerMgr (nArgC, argS, pForLogic, nDefArgC, defArgS, taskBuf, taskBufSize);
}

int onLoopBegin	(serverIdType	fId)
{
	int nRet = 1;
	auto &rMgr = tSingleton<taskClientWorkerMgr>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		nRet = pS->onLoopBeginBase	();
	}
	return nRet;
}

int onFrameLogic	(serverIdType	fId)
{
	int nRet = procPacketFunRetType_del;
	auto &rMgr = tSingleton<taskClientWorkerMgr>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		if (pS->willExit()) {
			nRet = procPacketFunRetType_exitNow;
		} else {
			nRet = pS->onLoopFrameBase();
		}
	}
	return nRet;
}

void onLoopEnd	(serverIdType	fId)
{
	auto &rMgr = tSingleton<taskClientWorkerMgr>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->onLoopEndBase();
	}
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}
int   onRecPacket(serverIdType	fId, packetHead* pack)
{
	return tSingleton<taskClientWorkerMgr>::single().processOncePack (fId, pack);
}
