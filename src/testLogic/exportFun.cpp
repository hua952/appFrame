#include <iostream>
#include "exportFun.h"
#include "msg.h"

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

