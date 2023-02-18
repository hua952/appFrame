#include <iostream>
#include "msgPmpID.h"
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "tSingleton.h"
#include "loopHandleS.h"

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

static int OnManualListAsk(packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	gDebug ("rec ManualListAsk");
	/*
	auto pSN = P2NHead(pSP);
	regretAsk ret;
	ret.toPack();
	pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId = TestServerHandle;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	*/
	manualListRet  ret;
	pRet = ret.pop ();
	return procPacketFunRetType_del;
}

static int OnGiveUpCli(packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	return procPacketFunRetType_exitNow;
}

static int OnGiveUpSer(packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	return procPacketFunRetType_exitAfterLoop;
}
 
static int OnMoveRet(packetHead* pack, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gDebug ("OnMoveRet");
	return nRet;
}

static int OnManualListRet(packetHead* pack, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gDebug ("OnManualListRet");
	regretAsk ask;
	auto packNew = ask.pop();
	auto pNH = P2NHead(packNew);
	auto& rSMgr = tSingleton<logicServerMgr>::single ();
	auto fnNextToken = getForMsgModuleFunS ().fnNextToken;
	pNH->dwToKen = fnNextToken (ThreadClientHandle);
	gTrace (" get token info handle = "<<ThreadClientHandle<<" token = "<<pNH->dwToKen);
	//gTrace(" 222222");
	auto fun = getForMsgModuleFunS().fnSendPackToLoop;
	//gTrace(" 333333");
	pNH->ubyDesServId = TestServerHandle;
	pNH->ubySrcServId = ThreadClientHandle;
	//gTrace ("Will call send fun");
	fun(packNew);
	/*
	moveAsk ask;
	ask.toPack ();
	auto pSend = ask.pop();
	auto pN = P2NHead (pSend);
	auto& rSMgr = tSingleton<logicServerMgr>::single ();
	pN->ubySrcServId = ThreadClientHandle;
	pN->ubyDesServId = AppTestServerHandle;
	auto fnNextToken = rSMgr.getForLogicFun ().fnNextToken;
	pN->dwToKen = fnNextToken (pArg->handle);
	auto fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pSend);
	*/
	return  nRet;
}

static int OnFrameSer(void* pArgS)
{
	//gTrace ("At the begin"); 
	return procPacketFunRetType_del;
}

static int OnRegretRet (packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	gInfo ("OnRegretRet ");
	/*
	auto pSN = P2NHead(pSP);
	manualListRet ret;
	ret.toPack();
	pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId =  ThreadClientHandle;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	*/
	moveAsk ask;
	ask.toPack ();
	auto pSend = ask.pop();
	auto pN = P2NHead (pSend);
	auto& rSMgr = tSingleton<logicServerMgr>::single ();
	pN->ubySrcServId = ThreadClientHandle;
	pN->ubyDesServId = AppTestServerHandle;
	auto fnNextToken = getForMsgModuleFunS().fnNextToken;
	pN->dwToKen = fnNextToken (pArg->handle);
	auto fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pSend);
	return procPacketFunRetType_del;
}

static int OnFrameCli(void* pArgS)
{
	//gTrace ("At the begin"); 
	return procPacketFunRetType_del;
}

static bool sSendMsg (void* pUserData)
{
	gDebug ("Begin To Send");
	manualListAsk  ask;
	auto pack = ask.pop();
	auto pNH = P2NHead(pack);
	//auto pTemp = (uqword)(pUserData);
	auto pTh = *(loopHandleType*)(pUserData);
	//gTrace(" aaaaaaa pTh = "<<pTh);
	auto& rSMgr = tSingleton<logicServerMgr>::single ();
	auto fnNextToken = getForMsgModuleFunS ().fnNextToken;
	//gTrace(" 111111 fnNextToken = "<<fnNextToken );
	pNH->dwToKen = fnNextToken (pTh);
	//gTrace(" 222222");
	auto fun = getForMsgModuleFunS().fnSendPackToLoop;
	//gTrace(" 333333");
	pNH->ubyDesServId = ThreadServerHandle;
	pNH->ubySrcServId = ThreadClientHandle;
	//gTrace ("Will call send fun");
	fun(pack);
	return true;
}
/*
ForLogicFun&  logicServerMgr::getForLogicFun ()
{
	return m_ForLogicFun;
}
*/
void logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	//setForMsgModuleFunS (pForLogic);
	auto& rForMsg = getForMsgModuleFunS();
	rForMsg = *pForLogic;
	gDebug ("In logicServerMgr::afterLoad testLogic");
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	gTrace ("will call fnCreateLoop");
	fnCreateLoop ("ThreadServer", ThreadServerHandle, nullptr, OnFrameSer, nullptr);
	gTrace ("after call fnCreateLoop");
	auto TestServerH = ThreadServerHandle;
	myAssert (c_emptyLoopHandle	 != TestServerH);
	auto fnRegMsg = pForLogic->fnRegMsg;
	fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_manualListAsk), OnManualListAsk);
	//fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_regretRet), OnRegretRet);
	//fnRegMsg (TestServerH, CChess2FullMsg(CChessMsgID_giveUpAsk), OnGiveUpSer);
	fnCreateLoop ("ThreadClient", ThreadClientHandle, nullptr, OnFrameCli, nullptr);
    auto TestClientH = ThreadClientHandle;
	myAssert (c_emptyLoopHandle	 != TestClientH);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_manualListRet), OnManualListRet);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_regretRet), OnRegretRet);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_moveRet), OnMoveRet);
	//fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_giveUpAsk), OnGiveUpCli);
	auto fnAddComTimer = pForLogic->fnAddComTimer;
	gInfo (" before fnAddComTimer testLogic");
	fnAddComTimer (TestClientH, 500, 10000,  sSendMsg, &TestClientH, sizeof(TestClientH));
	gInfo (" At then end of afterLoad testLogic");
}
