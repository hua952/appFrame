#include <iostream>
#include <memory>
#include "cppLevel0L.h"
#include "tSingleton.h"
#include "serverMgr.h"

int initFun (int cArg, char** argS, int cDefArg, char** defArgS)
{
	tSingleton<serverMgr>::createSingleton();
	auto& rMgr = tSingleton<serverMgr>::single();
	int nRet = rMgr.initFun(cArg, argS);
	return nRet;
}

int onPhyLoopBegin(loopHandleType pThis)
{
	return onMidLoopBegin (pThis);
}

int onPhyLoopEnd(loopHandleType pThis)
{
	return onMidLoopEnd (pThis);
}

bool onPhyFrame(loopHandleType pThis)
{
	auto& rSM = tSingleton<serverMgr>::single ();
	auto pS = rSM.getServer (pThis);
	return pS ? pS->onFrame () : true;
}

int  onPhyGetRunThreadIdS (char* szBuf, int bufSize)
{
	auto& rSM = tSingleton<serverMgr>::single ();
	return rSM.runThNum (szBuf, bufSize);
}
