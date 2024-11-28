#include <iostream>
#include <memory>
#include "cppLevel0L.h"
#include "tSingleton.h"
#include "workServer.h"
#include "workServerMgr.h"
#include "argConfig.h"

extern "C"
{
int initFun (int cArg, char** argS, int cDefArg, char** defArgS)
{
	tSingleton<workServerMgr>::createSingleton();
	auto& rMgr = tSingleton<workServerMgr>::single();
	int nRet = rMgr.initWorkServerMgr (cArg, argS, cDefArg, defArgS);
	return nRet;
}

int onPhyLoopBegin(loopHandleType pThis)
{
	auto& rSM = tSingleton<workServerMgr>::single ();
	auto pS = rSM.getServer(pThis);
	return pS ? pS->onLoopBegin() : true;
}

int onPhyLoopEnd(loopHandleType pThis)
{
	auto& rSM = tSingleton<workServerMgr>::single ();
	auto pS = rSM.getServer(pThis);
	return pS ? pS->onLoopEnd() : true;
}

bool onPhyFrame(loopHandleType pThis)
{
	auto& rSM = tSingleton<workServerMgr>::single ();
	auto pS = rSM.getServer(pThis);
	return pS ? pS->onFrame () : true;
}

int  onPhyGetRunThreadIdS (char* szBuf, int bufSize)
{
	auto& rSM = tSingleton<workServerMgr>::single ();
	return rSM.runThNum (szBuf, bufSize);
}

void afterAllLoopEndBeforeExitApp  ()
{
}

int getServerGroupInfo(uword serverG, ubyte* beginIndex, ubyte* runNum)
{
	auto& rConfig = tSingleton<argConfig>::single();
	auto serverGroupNum = rConfig.serverGroupNum ();
	int nRet = 0;
	do {
		if (serverG >= serverGroupNum) {
			nRet = 1;
			break;
		}
		auto serverGroups = rConfig.serverGroups ();
		*beginIndex = serverGroups[serverG].beginId;
		*runNum = serverGroups[serverG].runNum;
	} while (0);
	return nRet;
}
}
