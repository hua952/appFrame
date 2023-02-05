#include "logicServerMgr.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "msgPmpID.h"
#include "gLog.h"
#include "strFun.h"
#include "gen/loopHandleS.h"

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

static int OnMoveAsk (packetHead* pSP, procPacketArg* pArg)
{
	gInfo (__FUNCTION__);
	auto pSN = P2NHead(pSP);
	moveRet ret;
	ret.toPack();
	auto pRet = ret.pop();
	auto pN = P2NHead(pRet);
	pN->ubySrcServId = pSN->ubyDesServId;
	pN->ubyDesServId = pSN->ubySrcServId;
	pN->dwToKen = pSN->dwToKen;
	auto& fun = getForMsgModuleFunS().fnSendPackToLoop;
	fun(pRet);
	return procPacketFunRetType_del;
}

ForMsgModuleFunS& getForMsgModuleFunS()
{
	static ForMsgModuleFunS s_ForMsgModuleFunS;
	return  s_ForMsgModuleFunS;
}
/*
struct serverNode
{
	udword                  udwUnuse;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
	ubyte	                ubyUnuse;
	serverEndPointInfo		listenEndpoint [c_onceServerMaxListenNum];
	serverEndPointInfo		connectEndpoint [c_onceServerMaxConnectNum];
};
*/

void  logicServerMgr::afterLoad(const ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	rFunS.fnLogMsg = pForLogic->fnLogMsg;
	gDebug ("In afterLoad appTestServer");
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
	gInfo (" At then end of afterLoad serverB ");
}
