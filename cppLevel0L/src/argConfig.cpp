#include "argConfig.h"
#include "strFun.h"
#include <map>
#include <set>
#include <string>
#include "myAssert.h"
#include "mainLoop.h"
#include "serverMgr.h"
#include "tSingleton.h"

argConfig:: argConfig ()
{
	m_modelNum = 0;
}

argConfig:: ~argConfig ()
{
}

int  argConfig:: afterAllArgProc ()
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
			m_serverGroups = std::make_unique<serverRunInfo>(groupNum);
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
				tags[workerGroupId] = true;

				serverRunInfo& workerGroup = m_serverGroups.get() [workerGroupId];
				workerGroup.beginId = curBeginId;
				workerGroup.runNum = workerNum;
				workerGroup.autoRun = workerAutoRun;
				workerGroup.sleepSetp = sleepSetp;
				curBeginId += workerNum;
			}
		}
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
		auto nR = strR (pBuf, '*', pRetS, c_retMaxNum);
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
    } while (0);
    return nRet;
}

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

