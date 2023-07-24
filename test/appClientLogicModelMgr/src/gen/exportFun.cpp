#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "CChessRpc.h"
#include "logicServer.h"
#include "tSingleton.h"
#include "cLog.h"

void  afterLoad (int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(nArgC, argS, pForLogic);
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}