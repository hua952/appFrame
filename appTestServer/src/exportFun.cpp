#include "exportFun.h"
#include "logicServerMgr.h"
#include "tSingleton.h"
#include "msg.h"

void  afterLoad(ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
}
