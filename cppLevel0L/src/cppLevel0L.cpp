#include <iostream>
#include <memory>
#include "cppLevel0L.h"
#include "tSingleton.h"
#include "serverMgr.h"

int initFun (int cArg, const char* argS[])
{
	tSingleton<serverMgr>::createSingleton();
	auto& rMgr = tSingleton<serverMgr>::single();
	int nRet = rMgr.initFun(cArg, argS);
	return nRet;
}
