#include <iostream>
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"

logicServer::logicServer()
{
}

logicServer::~logicServer()
{
}

int logicServer::init(const char* name, ServerIDType id)
{
	auto nameL = strlen(name);
	m_name = std::make_unique<char[]>(nameL + 1);
	strcpy(m_name.get(), name);
	m_id = id;
	return 0;
}

ServerIDType logicServer::id()
{
	return m_id;
}

const char*  logicServer::name()
{
	return m_name.get();
}

int logicServer::OnServerFrame()
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

static int OnFrameSer(void* pArgS)
{
	//std::cout<<"OnFrameSer"<<std::endl;
	return 0;
}

static int OnFrameCli(void* pArgS)
{
	std::cout<<"OnFrameCli"<<std::endl;
	std::string strWord;
	std::cin>>strWord;
	if (strWord == "send")
	{
		std::cout<<"Begin To Send"<<std::endl;
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
		std::cout<<"Bey bey"<<std::endl;
	}
	return 0;
}

void logicServerMgr::afterLoad(const ForLogicFun* pForLogic)
{
	std::cout<<"In afterLoad"<<std::endl;
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto TestServerH = fnCreateLoop ("TestServer", OnFrameSer, nullptr);
	myAssert (c_emptyLoopHandle	 != TestServerH);
	auto fnRegMsg = pForLogic->fnRegMsg;
	fnRegMsg (TestServerH, CChessMsgID_manualListAsk, OnManualListAsk);
	auto TestClientH = fnCreateLoop ("TestClient", OnFrameCli, nullptr);
	myAssert (c_emptyLoopHandle	 != TestClientH);
	fnRegMsg (TestClientH, CChessMsgID_manualListRet, OnManualListRet);
}
