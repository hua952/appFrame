#include "exportFun.h"
#include "logicServerMgr.h"
#include "tSingleton.h"

void  afterLoad(const ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
}
