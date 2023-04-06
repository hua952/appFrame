#include "exportFun.h"
#include "logicServerMgr.h"
#include "tSingleton.h"
#include "msg.h"
#include <iostream>
#include <gLog.h>

void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	auto pFun = getLogMsgFun ();
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
}
