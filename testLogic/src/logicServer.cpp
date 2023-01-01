#include <iostream>
#include "msgPmpID.h"
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "gen/loopHandleS.h"

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

static int OnManualListAsk(packetHead* pSP)
{
	//std::cout<<"OnManualListAsk"<<std::endl;
	/*
	auto pSN = P2NHead(pSP);
	manualListRet ret;
	ret.toPack();
	auto pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId = pSN->ubySrcServId;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	*/
	gDebug ("rec ManualListAsk");
	auto pSN = P2NHead(pSP);
	regretAsk ret;
	ret.toPack();
	auto pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId = TestServerHandle;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	return procPacketFunRetType_del;
}

static int OnGiveUpCli(packetHead* pSP)
{
	return procPacketFunRetType_exitNow;
}

static int OnGiveUpSer(packetHead* pSP)
{
	return procPacketFunRetType_exitAfterLoop;
}
 
static int OnMoveRet(packetHead*)
{
	int nRet = procPacketFunRetType_del;
	gDebug ("OnMoveRet");
	return nRet;
}

static int OnManualListRet(packetHead*)
{
	int nRet = procPacketFunRetType_del;
	gDebug ("OnManualListRet");
	moveAsk ask;
	ask.toPack ();
	auto pSend = ask.pop();
	auto pN = P2NHead (pSend);
	pN->ubySrcServId = ThreadClientHandle;
	pN->ubyDesServId = AppTestServerHandle;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pSend);
	return  nRet;
}

static int OnFrameSer(void* pArgS)
{
	//std::cout<<"OnFrameSer"<<std::endl;
	return procPacketFunRetType_del;
}

static int OnRegretRet (packetHead* pSP)
{
	gInfo ("OnRegretRet ");
	auto pSN = P2NHead(pSP);
	manualListRet ret;
	ret.toPack();
	auto pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId =  ThreadClientHandle;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);

	return procPacketFunRetType_del;
}

static int OnFrameCli(void* pArgS)
{
	/*
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
		auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
		giveUpAsk serE;
		auto pSr = serE.pop();

		auto pNSr = P2NHead(pSr);
		pNSr->ubyDesServId = logicServerMgr::s_SerId;
		pNSr->ubySrcServId = logicServerMgr::s_CliId;
		fun(pSr);
		std::cout<<"Bey bey"<<std::endl;
		return procPacketFunRetType_exitAfterLoop;
	}
	*/
	return procPacketFunRetType_del;
}

static bool sSendMsg (void* pUserData)
{
	gDebug ("Begin To Send");
	manualListAsk  ask;
	auto pack = ask.pop();
	auto pNH = P2NHead(pack);
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	pNH->ubyDesServId = ThreadServerHandle;
	pNH->ubySrcServId = ThreadClientHandle;
	fun(pack);
	return true;
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
	fnCreateLoop ("ThreadServer", ThreadServerHandle, nullptr, OnFrameSer, nullptr);
	auto TestServerH = ThreadServerHandle;
	myAssert (c_emptyLoopHandle	 != TestServerH);
	auto fnRegMsg = pForLogic->fnRegMsg;
	fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_manualListAsk), OnManualListAsk);
	fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_regretRet), OnRegretRet);
	fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_giveUpAsk), OnGiveUpSer);
	fnCreateLoop ("ThreadClient", ThreadClientHandle, nullptr, OnFrameCli, nullptr);
    auto TestClientH = ThreadClientHandle;
	myAssert (c_emptyLoopHandle	 != TestClientH);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_manualListRet), OnManualListRet);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_moveRet), OnMoveRet);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_giveUpAsk), OnGiveUpCli);
	auto fnAddComTimer = pForLogic->fnAddComTimer;
	gInfo (" before fnAddComTimer testLogic");
	fnAddComTimer (ThreadClientHandle, 10000, 10000,  sSendMsg, nullptr, 0);
	gInfo (" At then end of afterLoad testLogic");
}
