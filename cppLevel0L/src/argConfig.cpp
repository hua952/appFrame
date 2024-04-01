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
		myAssert (nR);
		if (nR) {
			setModelMgrName (pRetS[0]);
		}
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
			auto mapRet = modMap.insert (std::make_pair(pRetSerS[0], tempServerS ()));
			myAssert (mapRet.second);
			if (!mapRet.second) {
				nRet = 3;
				break;
			}
			auto& serverS = mapRet.first->second;

			for (decltype (nSerR) j = 1; j < nSerR; j++) {
				auto pA = pRetSerS[j];
				const auto c_argMaxRetNum = 8;
				char* retArgS[c_argMaxRetNum];
				auto nRArgS = strR(pA, '-', retArgS, c_argMaxRetNum);
				myAssert (nRArgS < c_argMaxRetNum);
				if (nRArgS >= c_argMaxRetNum) {
					nRet = 4;
					break;
				}
				tempInfo info;
				auto thTmpId = (ubyte)(atoi(retArgS[0]));
				auto nLevel = thTmpId / c_onceServerLevelNum;
				/*
				ServerIDType openNum = 1;
				bool autoRun = true;
				bool route = false;
				*/
				info.first = 1;
				info.second = true;
				info.route = false;
				if (nRArgS > 1) {
					info.first = (decltype (thTmpId)) (atoi(retArgS[1]));
				} else if (nRArgS > 2) {
					info.second = atoi(retArgS[2]);
				} if (nRArgS > 3) {
					info.route = atoi(retArgS[3]);
				}
				myAssert (info.first <= c_levelMaxOpenNum[nLevel]);
				auto insRet = serverS.insert(std::make_pair((serverIdType)(atoi(retArgS[0])), info));
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
			strCpy (it->first.c_str(), rMod.modelName);
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

