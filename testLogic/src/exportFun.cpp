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
#include "gLog.h"

//typedef  int (*regMsgFT)(loopHandleType handle, uword uwMsgId, procPacketFunType pFun); // call by level 2

//const int c_SerNum =  logicServerMgr::serverIdS_Num;
static const char* serNameS[] = {"ThreadServer", "ThreadClient"};
static const auto c_SerNum = sizeof (serNameS) / sizeof (serNameS[0]);
int   getServerS (const char** pBuff, int nBuffNum)
{
	int nRet = 0;
	for (auto i = 0; i < c_SerNum && i < nBuffNum; i++)
	{
		pBuff[i] = serNameS[i];
	}
	nRet = c_SerNum;
	return nRet;
}

static int GetServerIdByName(const char* szName)
{
	int nRet = 0;
	for (; nRet < c_SerNum; nRet++)
	{
		auto p = serNameS [nRet];
		if (strcmp (p, szName) == 0)
		{
			return nRet;
		}
	}
	return nRet;
}

void  regMsgS(regMsgFT fnRegMsg)
{
	
}

void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	//auto nInit = initLog ("logic", "logicLog", 0, true);
	//myAssert (0 == nInit);
	auto& rForMsg = getForMsgModuleFunS();
	rForMsg = *pForLogic;
	//std::cout<<"OKOKOKOKOK"<<std::endl;
	//gTrace ("At then begin of   afterLoad");
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
	//std::cout<<"In   beforeUnload"<<std::endl;
}

/*
ForMsgModuleFunS& getForMsgModuleFunS()
{
	static ForMsgModuleFunS s_ForMsgModuleFunS;
	return  s_ForMsgModuleFunS;
}
*/
