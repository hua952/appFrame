#include <iostream>
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
#include "gen/loopHandleS.h"

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

void logicServerMgr::afterLoad(const ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	rFunS.fnLogMsg = pForLogic->fnLogMsg;
	gDebug ("In logicServerMgr::afterLoad testLogic");
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	auto fnAddComTimer = pForLogic->fnAddComTimer;
	gInfo (" before fnAddComTimer testLogic");
	gInfo (" At then end of afterLoad testLogic");
}