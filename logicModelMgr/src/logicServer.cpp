#include <iostream>
#include <string>
#include "msgPmpID.h"
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "CChessRpc.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "loopHandleS.h"
#include "strFun.h"
#include "comFun.h"
#include "rpcInfo.h"
#include "modelLoder.h"

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

static int OnGiveUpCli(packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	return procPacketFunRetType_exitNow;
}

static int OnGiveUpSer(packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
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

static int OnMoveRet (packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gInfo ("OnMoveRet");
	return nRet;
}

static int OnRegretAsk (packetHead* pSP, pPacketHead& pRet, procPacketArg* pArg)
{
	gInfo ("OnRegretAsk");
	auto pSN = P2NHead(pSP);
	regretRet ret;
	ret.toPack();
	pRet = ret.pop();
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

void logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	m_ForLogicFun = *pForLogic;
	auto pForMsg = &m_ForLogicFun;
	//setForMsgModuleFunS (pForMsg);
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	/*
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	rFunS.fnLogMsg = pForLogic->fnLogMsg;
	rFunS.fnGetIRpcInfoMgr = pForLogic->fnGetIRpcInfoMgr;
	*/
	regRpcS (&rFunS);
	std::unique_ptr<char[]> myPath;
	auto nGet = getMyPath (myPath);
	std::string strBase = myPath.get();
	do {
		std::string strDll = strBase;
		strDll += "testLogic.dll";
		//gTrace ("before load dll : "<<strDll);
		auto hDll = loadDll (strDll.c_str());
		if (!hDll) {
			gError ("load dll fail fileName = "<<strDll.c_str());
			break;
		}
		auto onLoad = (afterLoadFunT)(getFun(hDll, "afterLoad"));
		if (!onLoad) {
			gError ("get fun afterLoad fail");
			break;
		}
		//gTrace ("before call onLoad = "<<onLoad);
		onLoad (pForMsg);
		//gTrace ("after call onLoad");
		{
		std::string strServer = strBase;
		strServer += "testLogicB.dll";
		auto hDllServer = loadDll (strServer.c_str());
		if (!hDllServer) {
			gError ("load dll fail fileName = "<<strServer.c_str());
			break;
		}
		auto onLoad = (afterLoadFunT)(getFun(hDllServer, "afterLoad"));
		if (!onLoad) {
			gError ("get fun afterLoad fail");
			break;
		}
		//gTrace ("before call onLoad = "<<onLoad);
		onLoad (pForMsg);
		//gTrace ("after call onLoad");
		}
	} while (0);
	/*
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
	*/
}
