#include "logicServerMgr.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include "loopHandleS.h"

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

void  logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
}