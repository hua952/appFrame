#include "logicServerMgr.h"
#include "strFun.h"
#include "logicServer.h"

dword logicServerMgr::afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	dword nRet = 0;
	do {
		auto nR = getCurModelPath (m_homeDir);
		myAssert (0 == nR);
		auto bR = upDir (m_homeDir.get ());
		myAssert (bR);
		nRet = afterLoadLogic (nArgC, argS, pForLogic);
	} while (0);
	return nRet;
}

dword  logicServerMgr:: afterLoadLogic(int nArgC, char** argS, ForLogicFun* pForLogic)
{
    dword  nRet = 0;
    do {
    } while (0);
    return nRet;
}

void           logicServerMgr:: setNetServerTmp (serverIdType serverId)
{
    m_netServerTmp = serverId;
}

int     logicServerMgr:: initModelS (const char* szModelS)
{
    int     nRet = 0;
    do {
		std::unique_ptr<char[]>	bufModelS;
		strCpy(szModelS, bufModelS);
		auto  pBuf = bufModelS.get();
		const auto c_retMaxNum = 32;
		auto retS = std::make_unique<char* []>(c_retMaxNum);
		auto pRetS = retS.get();
		auto nR = strR (pBuf, '*', pRetS, c_retMaxNum);
    	myAssert (nR < c_retMaxNum);
		if (nR >= c_retMaxNum) {
			nRet = 1;
			break;
		}
		myAssert (2 == nR);
		auto szServerS = pRetS[1];

		const auto c_serRetMaxNum = 64;
		auto retBuf = std::make_unique<char* []>(c_serRetMaxNum);
		auto serRetS = retBuf.get();
		nR = strR(szServerS, '+', serRetS, c_serRetMaxNum);
		myAssert (nR);
		struct tempSerInfo
		{
			loopHandleType  open;
			bool            autoRun;
		};
		using  tempSerInfoMap = std::map<loopHandleType, tempSerInfo>;
		tempSerInfoMap  tempMap;
		for (decltype (nR) i = 0; i < nR; i++) {
			auto serS = serRetS [i];
			const auto c_argMaxRet = 6;
			auto argBuf = std::make_unique<char* []>(c_argMaxRet);
			auto pArgS = argBuf.get();
			auto nRR = strR(serS, '-', pArgS, c_argMaxRet);
			myAssert (nRR < c_argMaxRet);
			auto tmpId = (loopHandleType)(atoi(pArgS[0]));
			auto inRet = tempMap.insert(std::make_pair(tmpId, tempSerInfo()));
			myAssert(inRet.second);
			if (!inRet.second) {
				nRet = 3;
				break;
			}
			auto& info = inRet.first->second;
			info.open = (loopHandleType)(atoi(pArgS[1]));
			info.autoRun= (bool)(atoi(pArgS[2]));
		}
		if (nRet) {
			break;
		}
		for (auto it = tempMap.begin (); it != tempMap.end (); it++) {
			loopHandleType	level, onceLv, onceIndex;
			getLvevlFromSerId (it->first, level, onceLv, onceIndex);
			auto& rBig = m_muServerPairS[level];
			rBig.second++;
		}
		for (loopHandleType i = 0; i < c_serverLevelNum; i++) {
			auto  maxN = c_onceServerLevelNum /  c_levelMaxOpenNum[i];
			myAssert (m_muServerPairS[i].second < maxN);
			m_muServerPairS[i].first = std::make_unique<logicServerPair[]>(m_muServerPairS[i].second);
		}
		for (auto it = tempMap.begin (); it != tempMap.end (); it++) {
			loopHandleType	level, onceLv, onceIndex;
			getLvevlFromSerId (it->first, level, onceLv, onceIndex);
			auto& rBig = m_muServerPairS[level];
			auto& rOnce = rBig.first[onceLv];
			rOnce.second = it->second.open;
			rOnce.first = std::make_unique<logicServer*[]>(rOnce.second);
		}
    } while (0);
    return nRet;
}

logicServerMgr::logicServerPair*  logicServerMgr:: getServerArray (serverIdType	serverId)
{
	logicServerPair*   nRet = 0;
	do {
		decltype(serverId) ubyLv, onceLv, onceIndex;
		getLvevlFromSerId (serverId, ubyLv, onceLv, onceIndex);
		auto& rBigLv = m_muServerPairS[ubyLv];
		if (onceLv >= rBigLv.second) {
			break;
		}
		auto& rSA = rBigLv.first[onceLv];
		nRet = &rSA;
	} while (0);
	return nRet;
}

logicServer*   logicServerMgr:: findServer(serverIdType	fId)
{
    logicServer*   nRet = nullptr;
    do {
		decltype(fId) ubyLv, onceLv, onceIndex;
		getLvevlFromSerId (fId, ubyLv, onceLv, onceIndex);
		auto& rBigLv = m_muServerPairS[ubyLv];
		if (onceLv >= rBigLv.second) {
			break;
		}
		auto& rSA = rBigLv.first[onceLv];
		if (onceIndex >= rSA.second) {
			break;
		}
		nRet = rSA.first[onceIndex]; 
    } while (0);
    return nRet;
}

logicServerMgr::logicServerMgr ()
{
	m_netServerTmp = c_emptyLoopHandle;
	m_pForLogicFun = nullptr;
	for (decltype (c_serverLevelNum) i = 0; i < c_serverLevelNum; i++) {
		m_muServerPairS[i].second = 0;
	}
}

logicServerMgr::~logicServerMgr ()
{
}

ForLogicFun& logicServerMgr:: forLogicFunSt ()
{
    return *m_pForLogicFun;
}

ubyte  logicServerMgr::serverNum ()
{
	return  m_serverNum;
}

void   logicServerMgr::setServerNum (ubyte ubyNum)
{
	m_serverNum = ubyNum;
}

logicServerMgr::logicMuServerPairS*  logicServerMgr :: logicMuServerPairSPtr ()
{
    return m_muServerPairS;
}

serverIdType logicServerMgr:: netServerTmp ()
{
    return m_netServerTmp;
}

logicServer**  logicServerMgr :: getNetServerS (uword& num)
{
	logicServer**  pRet = nullptr;
	do {
		auto tmpId = netServerTmp ();
		if (c_emptyLoopHandle == tmpId) {
			break;
		}
		loopHandleType ubyLv,  onceLv, onceIndex ;
		getLvevlFromSerId (tmpId, ubyLv, onceLv, onceIndex);
		auto pAS = logicMuServerPairSPtr ();
		auto& rMu = pAS[ubyLv];
		if (onceLv >= rMu.second) {
			break;	
		}
		auto& rS = rMu.first[onceLv];
		if (rS.second) {
			pRet = rS.first.get ();
			num = rS.second;
		}
	} while (0);
    return pRet;
}

int   logicServerMgr:: initMsgDefProc (loopHandleType* pBuf, int num)
{
    int    nRet = 0;
    do {
		s_Mgr = this;
		std::map<msgIdType, loopHandleType> tempMap;
		myAssert(num %2 == 0);
		decltype (num) parNum = num / 2;
		for (decltype (parNum) i = 0; i < parNum; i++) {
			auto cur = 2 * i;
			tempMap[pBuf[cur]] = pBuf[cur + 1];
		}
		auto tempBuf = std::make_unique<msgDefProcMap::NodeType[]>(tempMap.size());
		auto pTempBuf = tempBuf.get();
		int nC = 0;
		for (auto it = tempMap.begin (); it != tempMap.end (); it++) {
			auto& rDef = pTempBuf[nC++];
			rDef.key = it->first;
			rDef.value = it->second;
		}
		m_msgDefProcS = std::make_unique<msgDefProcMap> (pTempBuf, nC);
    } while (0);
    return nRet;
}

loopHandleType   logicServerMgr:: getMsgDefProcTmp (msgIdType msgId)
{
    loopHandleType   nRet = c_emptyLoopHandle;
    do {
		auto pMap =  m_msgDefProcS.get();
		auto pF = pMap->find (msgId);
		if (!pF) {
			break;
		}
		nRet = *pF;
    } while (0);
    return nRet;
}

logicServerMgr* logicServerMgr::s_Mgr = nullptr;

logicServerMgr&  logicServerMgr:: getMgr()
{
	return *s_Mgr;
}
 
int  logicServerMgr:: procArgS (int nArgC, char** argS)
{
	int nRet;
	return nRet;
}

const char*  logicServerMgr:: homeDir ()
{
    return m_homeDir.get ();
}

void  logicServerMgr:: setHomeDir (const char* v)
{
    strCpy (v, m_homeDir);
}

