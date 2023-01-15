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
#include "strFun.h"

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

static int OnGiveUpCli(packetHead* pSP)
{
	return procPacketFunRetType_exitNow;
}

static int OnGiveUpSer(packetHead* pSP)
{
	return procPacketFunRetType_exitAfterLoop;
}

static int OnFrameSer(void* pArgS)
{
	//std::cout<<"OnFrameSer"<<std::endl;
	return procPacketFunRetType_del;
}

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

static int OnMoveRet (packetHead* pSP)
{
	int nRet = procPacketFunRetType_del;
	gInfo ("OnMoveRet");
	return nRet;
}

static int OnRegretAsk (packetHead* pSP)
{
	gInfo ("OnRegretAsk");
	auto pSN = P2NHead(pSP);
	regretRet ret;
	ret.toPack();
	auto pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId = pSN->ubySrcServId;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	return procPacketFunRetType_del;
}
/*
 struct serverEndPointInfo
{
	char              ip[16];
	udword            udwUnuse;
	uword             port;
	ServerIDType	  targetHandle;
	bool              bDef;
};
*/
void logicServerMgr::afterLoad(const ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	rFunS.fnLogMsg = pForLogic->fnLogMsg;
	gDebug ("In afterLoad serverB ");
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	serverNode node;
	node.connectorNum = 0;
	auto& ep = node.connectEndpoint [node.connectorNum++];
	strNCpy (ep.ip, sizeof (ep.ip), "127.0.0.1");
	ep.port = 11000;
	ep.targetHandle = AppTestServerHandle;
	ep.bDef = true;
	ep.bRegHandle = false;
	node.listenerNum = 0;
	fnCreateLoop ("TestServer", TestServerHandle,  &node, OnFrameCli, nullptr);
	auto TestClientH = TestServerHandle;
	myAssert (c_emptyLoopHandle	 != TestClientH);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_regretAsk), OnRegretAsk);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_moveRet), OnMoveRet);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_giveUpAsk), OnGiveUpCli);
	gInfo (" At then end of afterLoad serverB ");
}
