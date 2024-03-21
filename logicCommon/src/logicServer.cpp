#include "logicServer.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include <map>
#include "msg.h"

serverNode&  logicServer:: serverInfo ()
{
	return m_serverInfo;
}

const char*  logicServer:: serverName ()
{
	return m_serverName.get ();
}

int logicServer::onFrameFun ()
{
	// return willExit()?procPacketFunRetType_exitNow : procPacketFunRetType_del;
	return procPacketFunRetType_del;
}

void  logicServer::setServerName (const char* v)
{
	strCpy (v, m_serverName);
}

logicServerMgr::logicServerMgr ()
{
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
/*
logicServer**  logicServerMgr:: serverS ()
{
	return m_serverS.get ();
}
*/
static int OnFrameCli(void* pArgS)
{
	auto pS = (logicServer*)pArgS;
	return pS->onFrameFun ();
}
 
int  logicServerMgr:: procArgS (int nArgC, char** argS)
{
	int nRet;
	return nRet;
}

logicServer::logicServer ()
{
	m_willExit = false;
	m_serverId = c_emptyLoopHandle;
}

int logicServer::onServerInitGen(ForLogicFun* pForLogic)
{
	int nRet = 0;
	return nRet;
}

int logicServer:: sendMsg (CMsgBase& rMsg)
{
    int  nRet = 0;
    do {
		//rMsg.zip();
		// rMsg.toPack();
		auto pack = rMsg.getPack();
		auto nR = sendPack (pack);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

int logicServer:: sendToServer (CMsgBase& rMsg, loopHandleType handle)
{
    int  nRet = 0;
    do {
		// rMsg.zip();
		// rMsg.toPack();
		auto pack = rMsg.getPack();
		auto nR = sendPackToServer  (pack, handle);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

int logicServer:: sendPack (packetHead* pack)
{
    int  nRet = 0;
    do {
		auto fnGetDefProcServerId =  getForMsgModuleFunS ().fnGetDefProcServerId;
		auto pN = P2NHead (pack);
		auto handle = fnGetDefProcServerId (pN->uwMsgID);
		myAssert (c_emptyLoopHandle != handle);
		nRet = sendPackToServer (pack, handle);
    } while (0);
    return nRet;
}

serverIdType   logicServer:: serverId ()
{
	return m_serverId;
}

void  logicServer:: setServerId (serverIdType nId)
{
	m_serverId = nId;
}

int logicServer:: sendPackToServer (packetHead* pack, loopHandleType handle)
{
    int nRet = 0;
    do {
		auto pN = P2NHead (pack);
		pN->ubySrcServId = serverId ();
		pN->ubyDesServId = handle;
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoop;
		nRet = fnSendPackToLoop (pack);
    } while (0);
    return nRet;
}

int   logicServer:: sendMsgToSomeServer(CMsgBase& rMsg, serverIdType* pSerS, udword serverNum)
{
    int  nRet = 0;
    do {
		// rMsg.zip();
		// rMsg.toPack();
		auto pack = rMsg.getPack();
		auto nR = sendPackToSomeServer(pack, pSerS, serverNum);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

int   logicServer:: sendPackToSomeServer(packetHead* pack, serverIdType* pSerS, udword serverNum)
{
    int  nRet = 0;
    do {
		auto pN = P2NHead (pack);

		pN->ubySrcServId = serverId ();
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoop;
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			if (pSerS[i] == pN->ubySrcServId) {
				continue;
			}
			auto p = allocPacket (pN->udwLength);
			*p = *pack;
			auto pNN = P2NHead (p);
			memcpy (pNN, pN, pN->udwLength + NetHeadSize);
			pNN->ubyDesServId = pSerS[i];
			nRet = fnSendPackToLoop (p);
		}
		auto fnFreePack = getForMsgModuleFunS ().fnFreePack;
		fnFreePack (pack);
    } while (0);
    return nRet;
}

int logicServer:: sendToAllGateServer (CMsgBase& rMsg)
{
    int nRet = 0;
    do {
		auto pack = rMsg.pop();
		auto pNN = P2NHead (pack);
		pNN->ubySrcServId = serverId ();
		pNN->ubyDesServId = c_emptyLoopHandle;
		auto fnSendPackToLoopForChannel = getForMsgModuleFunS ().fnSendPackToLoopForChannel;
		fnSendPackToLoopForChannel (pack);
    } while (0);
    return nRet;
}

int    logicServer:: createChannel (channelKey& rCh, serverIdType srcSer, SessionIDType seId)
{
	int    nRet = 0;
	do {
		auto& rChS = channelS ();
		channelValue sidS;
		uqword uqwSe = srcSer;
		uqwSe <<= 32;
		uqwSe |= seId;
		sidS.insert(uqwSe);
		// auto& rCh = *((channelKey*)rAsk.m_chKey);
		auto inRet = rChS.insert(std::make_pair(rCh, sidS));
		myAssert (inRet.second);
		if (!inRet.second) {
			nRet = 1;
		}
	} while (0);
	return nRet;
}

int logicServer:: sendPackToSomeLocalServer(packetHead* pack, serverIdType* pSerS, udword serverNum)
{
    int nRet = 0;
    do {
		auto fnFreePack = getForMsgModuleFunS ().fnFreePack;
		auto& es =  exitHandleS ();
		if (!es.empty()) {
			fnFreePack (pack);
			break;
		}
		auto pN = P2NHead (pack);
		pN->ubySrcServId = serverId ();
		loopHandleType ph, lh;
		fromHandle (pN->ubySrcServId, ph, lh);
		// auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoop;
		auto pBuff = std::make_unique<serverIdType[]>(serverNum);
		decltype (serverNum) sendNum = 0;
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			loopHandleType sp, sl;
			auto serH = pSerS[i];
			if (serH == pN->ubySrcServId) {
				continue;
			}
			fromHandle (serH, sp, sl);
			if (sp != ph) {
				continue;
			}
			pBuff[sendNum++] = serH;
			es.insert(serH);
		}
		if (sendNum) {
			sendPackToSomeServer (pack, pBuff.get(), sendNum);
		} else {
			fnFreePack (pack);
		}
    } while (0);
    return nRet;
}

logicServer::exitHandleSet&  logicServer:: exitHandleS ()
{
    return m_exitHandleS;
}

int logicServer:: sendMsgToSomeLocalServer(CMsgBase& rMsg, serverIdType* pSerS, udword serverNum)
{
    int nRet = 0;
    do {
		//rMsg.zip();
		// rMsg.toPack();
		auto pack = rMsg.getPack();
		auto nR = sendPackToSomeLocalServer(pack, pSerS, serverNum);
		if (!nR) {
			rMsg.pop ();
		}
    } while (0);
    return nRet;
}

logicServer::channelMap& logicServer:: channelS ()
{
    return m_channelS;
}

void  logicServer:: addTimer(udword udwSetp, ComTimerFun pF, void* pUsrData)
{
    do {
		addTimer (udwSetp, udwSetp, pF, pUsrData);
    } while (0);
}

static bool sTimerProc (void* pUserData)
{
	auto pB = (void**)pUserData;
	auto pF = (ComTimerFun)(pB[0]);
	return pF(pB[1]);
}

void  logicServer:: addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData)
{
    do {
		void* pB[2];
		pB[0] = (void*)pF;
		pB[1] = pUserData;
		addTimer (firstSetp, udwSetp, sTimerProc, pB, sizeof (pB));
    } while (0);
}

void  logicServer:: addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength)
{
    do {
		auto fnAddComTimer =  getForMsgModuleFunS ().fnAddComTimer;
		auto handle = serverId ();
		fnAddComTimer (handle, firstSetp, udwSetp, pF, pUserData, udwLength);
    } while (0);
}

void logicServer::addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength)
{
	addTimer (udwSetp, udwSetp, pF, pUserData, udwLength);
}

logicServer::channelSendMap&  logicServer:: channelSendS ()
{
    return m_channelSendS;
}

bool logicServer::cmpChannelKey::operator ()(const channelKey& k1,const channelKey& k2)const
{
	return k1.key<k2.key?true:k2.key<k1.key?false:k1.value<k2.value;
}

dword logicServerMgr::afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	return 0;
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

void  logicServer:: logicOnAcceptSession(SessionIDType sessionId, uqword userData)
{
    do {
    } while (0);
}

void  logicServer:: logicOnConnect(SessionIDType sessionId, uqword userData)
{
    do {

    } while (0);
}

void logicServer:: sLogicOnAcceptSession(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
    do {
		/*
		auto pThis = logicServer::findServer(fId);
		pThis->logicOnAcceptSession(sessionId, userData);
		*/
    } while (0);
}

void logicServer:: onLoopBegin()
{
}

void logicServer:: onLoopEnd()
{
}

void logicServer:: sLogicOnConnect(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
    do {
		/*
		auto pThis = logicServer::findServer(fId);
		pThis->logicOnConnect(sessionId, userData);
		*/
    } while (0);
}

bool  logicServer:: willExit ()
{
    return m_willExit;
}

void  logicServer:: setWillExit (bool v)
{
    m_willExit = v;
}

logicServerMgr::logicMuServerPairS*  logicServerMgr :: logicMuServerPairSPtr ()
{
    return m_muServerPairS;
}

