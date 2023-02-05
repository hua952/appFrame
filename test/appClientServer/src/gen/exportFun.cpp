#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "myAssert.h"
#include "logicServer.h"
#include "tSingleton.h"
#include "gLog.h"

void  afterLoad(const ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}

ForMsgModuleFunS& getForMsgModuleFunS()
{
	static ForMsgModuleFunS s_ForMsgModuleFunS;
	return  s_ForMsgModuleFunS;
}