#include "logicServer.h"
#include <cstring>

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

void logicServer:: afterLoad(const ForLogicFun* pForLogic)
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
