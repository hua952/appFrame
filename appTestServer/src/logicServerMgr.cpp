#include "logicServerMgr.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "msgPmpID.h"
#include "gLog.h"
#include "strFun.h"
#include "loopHandleS.h"

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

static int OnMoveAsk (packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	gInfo ("rec MoveAsk ");
	auto pSN = P2NHead(pSP);
	moveRet ret;
	ret.toPack();
	pRet = ret.pop();
	auto pN = P2NHead(pRet);
	return procPacketFunRetType_del;
}

void  logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	serverNode node;
	node.listenerNum = 0;
	auto& ep = node.listenEndpoint [node.listenerNum++];
	strNCpy (ep.ip, sizeof (ep.ip), "127.0.0.1");
	ep.port = 11000;
	node.connectorNum = 0;
	fnCreateLoop ("appTestServer", AppTestServerHandle, &node, OnFrameCli, nullptr);
	auto TestClientH = AppTestServerHandle;
	myAssert (c_emptyLoopHandle	 != TestClientH);
	fnRegMsg (TestClientH, CChess2FullMsg(CChessMsgID_moveAsk), OnMoveAsk);
	gInfo (" At then end of afterLoad ");
}
