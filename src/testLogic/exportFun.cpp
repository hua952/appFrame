#include <iostream>
#include "exportFun.h"
#include "msg.h"
#include "CChessMsgID.h"

static int OnFrame(void* pArgS)
{
	return 0;
}

static int OnManualListAsk(packetHead*)
{
	std::cout<<"OnManualListAsk"<<std::endl;
	return 0;

}
static int OnManualListRet(packetHead*)
{
	std::cout<<"OnManualListRet"<<std::endl;
	return 0;
}

//typedef  int (*regMsgFT)(loopHandleType handle, uword uwMsgId, procPacketFunType pFun); // call by level 2

void  testThreadS(const ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	auto hThreadS = fnCreateLoop ("TestServer", OnFrame, nullptr);
	fnRegMsg (hThreadS, CChessMsgID_manualListAsk, OnManualListAsk);
	auto hThreadC= fnCreateLoop ("TestClient", OnFrame, nullptr);
	fnRegMsg (hThreadC, CChessMsgID_manualListRet, OnManualListRet);
}

void  afterLoad(const ForLogicFun* pForLogic)
{
	std::cout<<"In afterLoad"<<std::endl;
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}

