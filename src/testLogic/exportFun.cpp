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

static int OnFrameSer(void* pArgS)
{
	//std::cout<<"OnFrameSer"<<std::endl;
	return 0;
}

static int OnFrameCli(void* pArgS)
{
	//std::cout<<"OnFrameCli"<<std::endl;
	std::string strWord;
	std::cin>>strWord;
	if (strWord == "send")
	{
		manualListAsk  ask;
		auto pack = ask.pop();
		auto pNH = P2NHead(pack);
		auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
		pNH->ubyDesServId = logicServerMgr::s_SerId;
		pNH->ubySrcServId = logicServerMgr::s_CliId;
		fun(pack);
	}
	else if (strWord == "exit")
	{
	}
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
/*
void  testThreadS(const ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	auto hThreadS = fnCreateLoop ("TestServer", OnFrame, nullptr);
	fnRegMsg (hThreadS, CChessMsgID_manualListAsk, OnManualListAsk);
	auto hThreadC= fnCreateLoop ("TestClient", OnFrame, nullptr);
	fnRegMsg (hThreadC, CChessMsgID_manualListRet, OnManualListRet);
}
*/
const int c_SerNum =  logicServerMgr::serverIdS_Num;
static const char* serNameS[c_SerNum] = {"TestServer", "TestClient"};
int   getServerS (const char** pBuff, int nBuffNum)
{
	int nRet = 0;
	if (nBuffNum >= c_SerNum)
	{
		for (auto i = 0; i < c_SerNum; i++)
		{
			pBuff[i] = serNameS[i];
		}
		nRet = c_SerNum;
	}
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
	auto nSerId = GetServerIdByName ("TestServer");
	fnRegMsg (nSerId, CChessMsgID_manualListAsk, OnManualListAsk);
	nSerId = GetServerIdByName ("TestClient");
	fnRegMsg (nSerId, CChessMsgID_manualListRet, OnManualListRet);
}

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

