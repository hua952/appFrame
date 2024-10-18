#include "logicFrameConfig.h"
#include "strFun.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "loop.h"
#include "mainLoop.h"

logicFrameConfig:: logicFrameConfig ()
{
}

logicFrameConfig:: ~logicFrameConfig ()
{
}

int logicFrameConfig :: serverGroupNum ()
{
    return m_serverGroupNum;
}

void  logicFrameConfig:: setServerGroupNum (int v)
{
    m_serverGroupNum = v;
}

const logicFrameConfig::serverRunInfo*  logicFrameConfig:: serverGroups ()
{
	return m_serverGroups.get();
}

int  logicFrameConfig:: procGateNode()
{
	int  nRet = 0;
	do {
		auto gateSplit = stringSplit (gateInfo(), '+');
		if (!gateSplit.empty()) {
			m_gateNodeNum = (ubyte)(gateSplit.size());
			m_gateNodes = std::make_unique<gateNode[]>(m_gateNodeNum);
			ubyte cur = 0;
			for (auto it = gateSplit.begin (); it != gateSplit.end (); it++) {
				auto& rGate = m_gateNodes[cur++];
				auto kvs = stringSplit (it->c_str(), '*');
				for (auto ite = kvs.begin (); ite != kvs.end (); ite++) {
					auto kv = stringSplit (ite->c_str(), ':');
					myAssert (2 == kv.size ());
					auto bC = false;
					auto bS = false;
					if (kv[0] == "forClientIp") {
						strCpy(kv[1].c_str(), rGate.endPoints[0].first);
						bC = true;
					} else if (kv[0] == "forServerIp") {
						strCpy(kv[1].c_str(), rGate.endPoints[1].first);
						bS = false;
					} else if (kv[0] == "startPort") {
						rGate.endPoints[0].second= (uword)(atoi(kv[1].c_str()));
						rGate.endPoints[1].second= (uword)(atoi(kv[1].c_str()));
					}
					if (bC) {
						rGate.endPointNum = 1;
					}
					if (!bC && bS) {
						strCpy(rGate.endPoints[1].first.get(), rGate.endPoints[0].first);
						rGate.endPoints[1].first.reset();
						rGate.endPointNum = 1;
					}
					if (bC && bS ) {
						if (0 == strcmp(rGate.endPoints[0].first.get(), rGate.endPoints[1].first.get())) {
							rGate.endPoints[1].first.reset();
							rGate.endPointNum = 1;
						} else {
							rGate.endPointNum = 2;
						}
					}
				}
			}
		}
	} while (0);
	return nRet;
}

int  logicFrameConfig:: procWorkers ()
{
	int  nRet = 0;
	do {

		auto retSplit = stringSplit (runWorkNum (), '+');

		if (!retSplit.size ()) {
			nRet = 1;
		}
		serverIdType routeGroupId = c_emptyLoopHandle;
		decltype (retSplit.size ()) groupNum = 0;
		if (retSplit.size () > 1) {
			auto routeRet = stringSplit (retSplit[1].c_str(), ':');
			myAssert (routeRet.size() == 2);
			std::stringstream ss (routeRet[1]);
			ss>>routeGroupId;
		}
		auto workerRet = stringSplit (retSplit.begin()->c_str(), ':');
		myAssert (workerRet.size() == 2);
		auto worksRet = stringSplit (workerRet[1].c_str(), '-');
		groupNum += worksRet.size();
		myAssert (routeGroupId < groupNum);
		m_serverGroups = std::make_unique<serverRunInfo[]>(groupNum);
		auto tags = std::make_unique<bool[]>(groupNum);
		for (decltype (groupNum) i = 0; i < groupNum; i++) {
			tags[i] = false;
		}
		m_serverGroupNum = groupNum;
		decltype (routeGroupId) curBeginId = 0;
		for (auto it = worksRet.begin (); it != worksRet.end (); it++) {
			serverIdType  workerGroupId = 0xffff;
			serverIdType  workerNum;
			int           workerAutoRun = 1;
			udword		  sleepSetp = 2;

			auto fourRet = stringToFourValue(it->c_str(), '*', workerGroupId, workerNum, workerAutoRun, sleepSetp);
			myAssert (fourRet);
			myAssert (workerGroupId < groupNum);
			myAssert (!tags[workerGroupId]);
			if (workerGroupId == routeGroupId) {
				auto appNetType = this->appNetType ();
				if(appNetType_client == appNetType) {
					workerNum = 1;
				} else {
					auto netNum = this->netNum ();
					if (appNetType_gate == appNetType) {
						workerNum = netNum;
					} else {
						workerNum = m_gateNodeNum;
					}
				}
			}
			tags[workerGroupId] = true;

			serverRunInfo& workerGroup = m_serverGroups.get() [workerGroupId];
			workerGroup.beginId = curBeginId;
			workerGroup.runNum = workerNum;
			workerGroup.autoRun = workerAutoRun;
			workerGroup.sleepSetp = sleepSetp;
			curBeginId += workerNum;
		}
		setRouteGroupId ((ubyte)routeGroupId);
	} while (0);
	return nRet;
}

int  logicFrameConfig:: afterAllArgProc ()
{
	int  nRet = 0;
	do {
		int nR = 0;
		nR = procGateNode ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = procWorkers ();
		if (nR) {
			nRet = 2;
			break;
		}
	} while (0);
    return nRet;
}


ubyte  logicFrameConfig:: gateNodeNum ()
{
    return m_gateNodeNum;
}

void  logicFrameConfig:: setGateNodeNum (ubyte v)
{
    m_gateNodeNum = v;
}

logicFrameConfig::gateNode* logicFrameConfig:: gateNodes ()
{
    return m_gateNodes.get();
}


ubyte  logicFrameConfig:: routeGroupId ()
{
    return m_routeGroupId;
}

void  logicFrameConfig:: setRouteGroupId (ubyte v)
{
    m_routeGroupId = v;
}

