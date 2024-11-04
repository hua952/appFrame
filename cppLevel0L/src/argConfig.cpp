#include "argConfig.h"
#include "strFun.h"
#include <map>
#include <set>
#include <string>
#include "myAssert.h"
#include "mainLoop.h"
#include "tSingleton.h"

argConfig:: argConfig ()
{
	// m_modelNum = 0;
}

argConfig:: ~argConfig ()
{
}

int  argConfig:: procGateNode()
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

int  argConfig:: procWorkers ()
{
	int  nRet = 0;
	do {
		auto retSplit = stringSplit (runWorkNum (), '+');
		auto groupNum = 0;
		if (retSplit.size ()) {
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
			serverIdType  curI = 0;
			for (auto it = worksRet.begin (); it != worksRet.end (); it++) {
				serverIdType  workerGroupId = curI++;
				serverIdType  workerNum{0};
				int           workerAutoRun = 1;
				udword		  sleepSetp = 2;
				// auto temp = *it;
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
		}
	} while (0);
	return nRet;
}

int  argConfig:: afterAllArgProc ()
{
    int  nRet = 0;
	do {
		int nR = procGateNode ();
		if (nR) {
			nRet = 1;
			break;
		}

		nR = procWorkers ();	
		if (nR) {
			nRet = 2;
			break;
		}
		/*
		break;
		struct tempInfo
		{
			serverIdType first;
			bool second;
			bool route;
		};
		using tempServerS = std::map<serverIdType, tempInfo>;
		std::map<std::string, tempServerS> modMap;
		auto szModelS = modelS ();
		std::unique_ptr<char[]> buf;
		strCpy (szModelS, buf);
		auto pBuf = buf.get();
		const auto c_retMaxNum = 64;
		auto retS = std::make_unique<char* []>(c_retMaxNum);
		auto pRetS = retS.get();
		nR = strR (pBuf, '*', pRetS, c_retMaxNum);
    	myAssert (nR < c_retMaxNum);
		if (nR >= c_retMaxNum) {
			nRet = 1;
			break;
		}
		myAssert (2 == nR || 0 == nR);
		if (0 == nR) {
			break;
		}
		if (2 != nR) {
			nRet = 5;
			break;
		}
		auto modelMgrname = pRetS[0];
		setModelMgrName (modelMgrname);
		for (decltype (nR) i = 1; i < nR; i++) {
			auto c_retSerMaxNum = 64;
			auto retSerS = std::make_unique<char* []>(c_retSerMaxNum);
			auto pRetSerS = retSerS.get();
			auto pM = pRetS[i];
			auto nSerR = strR(pM, '+', pRetSerS, c_retSerMaxNum);
			myAssert (nSerR < c_retSerMaxNum);
			if (nSerR >= c_retSerMaxNum) {
				nRet = 2;
				break;
			}
			if (!nSerR) {
				continue;
			}
			auto mapRet = modMap.insert (std::make_pair(modelMgrname, tempServerS ()));
			myAssert (mapRet.second);
			if (!mapRet.second) {
				nRet = 3;
				break;
			}
			auto& serverS = mapRet.first->second;
			for (decltype (nSerR) j = 0; j < nSerR; j++) {
				auto pA = pRetSerS[j];
				const auto c_argMaxRetNum = 8;
				char* retArgS[c_argMaxRetNum];
				auto nRArgS = strR(pA, '-', retArgS, c_argMaxRetNum);
				myAssert (nRArgS < c_argMaxRetNum);
				if (nRArgS >= c_argMaxRetNum) {
					nRet = 4;
					break;
				}
				auto serverId = (serverIdType)(atoi(retArgS[0]));
				tempInfo info;
				auto thTmpId = (ubyte)(serverId);
				auto nLevel = thTmpId / c_onceServerLevelNum;
				
				info.first = 1;
				info.second = true;
				info.route = false;
				if (nRArgS > 1) {
					info.first = (decltype (thTmpId)) (atoi(retArgS[1]));
				}
				if (nRArgS > 2) {
					info.second = atoi(retArgS[2]);
				}
				if (nRArgS > 3) {
					info.route = atoi(retArgS[3]);
				}
				myAssert (info.first <= c_levelMaxOpenNum[nLevel]);
				auto insRet = serverS.insert(std::make_pair(serverId, info));
				myAssert (insRet.second);
				if (!insRet.second) {
					nRet = 5;
					break;
				}
			}
			if (nRet) {
				break;
			}
		}
		if (nRet) {
			break;
		}
		auto& rMgr = tSingleton<serverMgr>::single ();
		auto muServerPairS = rMgr.muServerPairSPtr ();
		auto cur = 0;
		m_modelNum = (decltype(m_modelNum ))(modMap.size());
		myAssert (m_modelNum <= 1);
		m_modelS = std::make_unique<stModel[]>(m_modelNum);
		for (auto it = modMap.begin (); it != modMap.end (); it++) {
			auto& rMod = m_modelS[cur++];
			strCpy (modelMgrname, rMod.modelName);
			auto& rServerS = it->second;
			rMod.serverTemNum = (decltype(rMod.serverTemNum))(rServerS.size());
			rMod.serverS = std::make_unique<stServer[]>(rMod.serverTemNum);
			decltype(rMod.serverTemNum) n = 0;
			for (auto ite = rServerS.begin (); ite != rServerS.end (); ite++) {
				auto& rS = rMod.serverS[n++];
				rS.serverTmpId = ite->first;
				rS.openNum = ite->second.first;
				rS.autoRun = ite->second.second;
				rS.route = ite->second.route;
			}
		}
		*/
    } while (0);
    return nRet;
}
/*
argConfig::stModel*  argConfig:: allModelS ()
{
    return m_modelS.get();
}

serverIdType   argConfig:: modelNum ()
{
    return m_modelNum;
}

const char*  argConfig:: modelMgrName ()
{
    return m_modelMgrName.get ();
}

void  argConfig:: setModelMgrName (const char* v)
{
    strCpy (v, m_modelMgrName);
}
*/
int  argConfig:: serverGroupNum ()
{
    return m_serverGroupNum;
}

void  argConfig:: setServerGroupNum (int v)
{
    m_serverGroupNum = v;
}

const argConfig::serverRunInfo*   argConfig:: serverGroups ()
{
	return m_serverGroups.get();
}

ubyte  argConfig:: gateNodeNum ()
{
    return m_gateNodeNum;
}

void  argConfig:: setGateNodeNum (ubyte v)
{
    m_gateNodeNum = v;
}

argConfig::gateNode*  argConfig:: gateNodes ()
{
    return m_gateNodes.get();
}

