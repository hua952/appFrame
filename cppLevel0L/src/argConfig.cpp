#include "argConfig.h"
#include "strFun.h"
#include <map>
#include <set>
#include <string>
#include "myAssert.h"

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
		using tempServerS = std::map<std::string, serverIdType>;
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
		for (decltype (nR) i = 0; i < nR; i++) {
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

			for (decltype (nSerR) j = 1; j < nSerR; i++) {
				auto pA = pRetSerS[j];
				const auto c_argMaxRetNum = 4;
				char* retArgS[c_argMaxRetNum];
				auto nRArgS = strR(pA, '-', retArgS, c_argMaxRetNum);
				myAssert (nRArgS < c_argMaxRetNum);
				if (nRArgS = c_argMaxRetNum) {
					nRet = 4;
					break;
				}

				auto insRet = serverS.insert(std::make_pair(retArgS[0], 1));
				myAssert (insRet.second);
				if (!insRet.second) {
					nRet = 5;
					break;
				}
				if (nRArgS > 1) {
					insRet.first->second = (decltype (insRet.first->second)) (atoi(retArgS[1]));
				}
			}
			if (nRet) {
				break;
			}
		}
		if (nRet) {
			break;
		}
		m_modelNum = (decltype(m_modelNum))(modMap.size());
		m_modelS = std::make_unique<stModel []>(m_modelNum);
		decltype(m_modelNum) cur = 0;
		for (auto it = modMap.begin (); it != modMap.end (); it++) {
			auto& rMod = m_modelS[cur++];
			strCpy (it->first.c_str(), rMod.modelName);
			auto& rServerS = it->second;
			rMod.serverTemNum = (decltype(rMod.serverTemNum))(rServerS.size());
			rMod.serverS = std::make_unique<stServer[]>(rMod.serverTemNum);
			decltype(rMod.serverTemNum) n = 0;
			for (auto ite = rServerS.begin (); ite != rServerS.end (); ite++) {
				auto& rS = rMod.serverS[n++];
				strCpy(ite->first.c_str(), rS.serverName);
				rS.openNum = ite->second;
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

