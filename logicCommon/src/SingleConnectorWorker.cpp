#include "SingleConnectorWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"

SingleConnectorWorker:: SingleConnectorWorker ()
{
}

SingleConnectorWorker:: ~SingleConnectorWorker ()
{
}

int  SingleConnectorWorker:: getGateServerEndpoint(endPoint& endP)
{
    int  nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto gateNodeNum = rConfig.gateNodeNum ();
		auto gateNodes = rConfig.gateNodes ();
		myAssert (gateNodeNum);
		int n = 0;
		if (gateNodeNum > 1) {
			n = rand () % gateNodeNum;
		}
		strNCpy(endP.ip, sizeof(endP.ip), gateNodes[n].endPoints[0].first.get()); 
		endP.port = gateNodes[n].endPoints[0].second;
		endP.userDataLen = 0;
		endP.userData = nullptr;
    } while (0);
    return nRet;
}


void   SingleConnectorWorker:: sendHeartbeat ()
{
    do {
		heartbeatAskMsg msg;
		auto pack = msg.pop();
		m_session->send (pack);
    } while (0);
}

int  SingleConnectorWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		int nR = 0;	
		endPoint endP;
		nR = getGateServerEndpoint(endP);
		myAssert (!nR);
		endP.ppIConnector = &m_session;
		nR = initNet (nullptr, 0, &endP, 1, nullptr, 0);
		if (nR) {
			gError("initNet error nR = "<<nR<<" endpoint = "<<endP.ip<<" : "<<endP.port);
			nRet = 1;
			break;
		}
		nR = routeWorker:: onLoopBeginBase();
		if (nR) {
			nRet = 2;
			break;
		}
    } while (0);
    return nRet;
}

int  SingleConnectorWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession)
{
    int  nRet = 0;
    do {
		myAssert (m_session);
		m_session->send (pPack);
		rRet.m_result = 0;
    } while (0);
    return nRet;
}

