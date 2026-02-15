#include <cmath>
#include <sstream>
#include "logicWorker.h"
#include "logicWorkerMgr.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"
#include "packet.h"
/*
static std::ostream& operator << (std::ostream& os, const packetHead& pack)
{
	auto pp = &pack;
	auto& p = *(P2NHead(pp));
    os<<"pAsk = "<<pack.packArg<<" sessionID = " <<pack.sessionID<<" loopId = "<<(int)(pack.loopId)<< " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pack = "<<pp;
    return os;
}

static std::ostream& operator << (std::ostream& os, const netPacketHead& pack)
{
	auto pp = &pack;
	auto& p = pack;
    os << " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pN = "<<pp;
    return os;
}
*/

logicWorker:: logicWorker ()
{
}

logicWorker:: ~logicWorker ()
{
}

bool logicWorker::cmpChannelKey::operator ()(const channelKey& k1,const channelKey& k2)const
{
	return k1.first<k2.first?true:k2.first<k1.first?false:k1.second<k2.second;
}

int          logicWorker:: sendMsg (CMsgBase& rMsg)
{
	auto pack = rMsg.pop();
    return sendPacket (pack);
}

int          logicWorker:: sendMsg (CMsgBase& rMsg, loopHandleType appGroupId, loopHandleType threadGroupId)
{

	auto pack = rMsg.pop();
    return sendPacket (pack, appGroupId, threadGroupId);
}

int   logicWorker:: sendPacket (packetHead* pPack)
{
    int   nRet = 0;
    do {
		auto& rMgr = logicWorkerMgr::getMgr();
		auto forLogicFun  = rMgr.forLogicFun ();
		ubyte appG;
		ubyte serverG;
		auto pN = P2NHead(pPack);
		auto bF = rMgr.findDefProc (pN->uwMsgID, appG, serverG);
		if (!bF) {
			gError("can not find def proc pack: "<<pPack);
			forLogicFun->fnFreePack (pPack);
			break;
		}
		nRet = sendPacket (pPack, appG, serverG);
    } while (0);
    return nRet;
}
/*
int   logicWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
	return sendPacket (pPack, appGroupId, threadGroupId, EmptySessionID);
}
*/
int   logicWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId/*, SessionIDType sessionId*/)
{
	/*   该函数目前只发Ask包     */
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& rMgr = logicWorkerMgr::getMgr();
		auto myAppGroupId = rConfig.appGroupId ();
		auto serverGroups = rConfig.serverGroups ();
		auto forLogicFun  = rMgr.forLogicFun ();

		auto pN = P2NHead(pPack);
		if (!NIsRet(pN)) {
			pN->dwToKen = forLogicFun->fnNextToken (serverId ());
		}
		if (myAppGroupId == appGroupId) {
			nRet = pushPacketToLocalServer (pPack, threadGroupId);
		} else {
			auto objId = c_emptyLocalServerId;
			auto routeGroupId = rConfig.routeGroupId ();
			auto& routeGroup= rConfig.serverGroups ()[routeGroupId];

			if (c_emptyLocalServerId == (ubyte)(pPack->loopId)) {
				ubyte ubyI = 0;
				if (routeGroup.runNum > 1) {
					ubyI = rand() % routeGroup.runNum;
				}
				objId = routeGroup.beginId + ubyI;
			} else {
				objId = pPack->loopId;
			}
			
			auto forLogicFun  = rMgr.forLogicFun ();
			
			pN->ubySrcServId = rConfig.appGroupId ();
			pN->ubySrcServId <<= 8;
			pN->ubySrcServId |= serverId ();
			
			pN->ubyDesServId = appGroupId;
			pN->ubyDesServId <<= 8;
			pN->ubyDesServId |= threadGroupId;

			sendPackToRemoteAskMsg ask;
			auto pAsk = ask.pop ();
			pAsk->packArg = (decltype(pAsk->packArg))(pPack);
			pAsk->loopId = serverId ();
			auto pAskN = P2NHead(pAsk);
			auto pAskU = (sendPackToRemoteAsk*)(N2User(pAskN));
			pAskU->objSessionId = pPack->sessionID;
			pAskN->ubySrcServId = serverId ();
			pAskN->ubyDesServId = routeGroupId;
			forLogicFun->fnPushPackToServer (objId, pAsk);
		}
    } while (0);
    return nRet;
}

int   logicWorker:: pushPacketToLocalServer(packetHead* pPack, loopHandleType threadGroupId)
{
	int   nRet = 0;
	do {
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto& rMgr = logicWorkerMgr::getMgr();
		auto serverGroups = rConfig.serverGroups ();
		auto serverGroupNum = rConfig.serverGroupNum ();
		auto forLogicFun  = rMgr.forLogicFun ();
		myAssert (threadGroupId < serverGroupNum);
		if (threadGroupId >= serverGroupNum) {
			gError("error thread group id threadGroupId : "<<threadGroupId<<" pack: "<<pPack);
			forLogicFun->fnFreePack (pPack);
			break;
		}
		auto& rGroup = serverGroups[threadGroupId];
		auto pN = P2NHead(pPack);
		pN->ubyDesServId = threadGroupId;
		ubyte ubyI = 0;
		if (rGroup.runNum > 1) {
			ubyI = rand() % rGroup.runNum;
		}
		pN->ubySrcServId = serverId ();
		pPack->loopId = serverId ();
		forLogicFun->fnPushPackToServer (rGroup.beginId + ubyI, pPack);
	} while (0);
	return nRet;
}

ubyte  logicWorker:: serverId ()
{
    return m_serverId;
}

void  logicWorker:: setServerId (ubyte v)
{
    m_serverId = v;
}

int   logicWorker:: processOncePack(packetHead* pPack)
{
    int   nRet = procPacketFunRetType_del;
    return nRet;
}
/*
int  logicWorker:: onLoopBegin()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopEnd()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopFrame()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}
*/
int  logicWorker:: onWorkerInitGen(ForLogicFun* pForLogic)
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorker:: onWorkerInit(ForLogicFun* pForLogic)
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

ubyte  logicWorker:: serverGroup ()
{
    return m_serverGroup;
}

void  logicWorker:: setServerGroup (ubyte v)
{
    m_serverGroup = v;
}

bool  logicWorker:: willExit ()
{
    return m_willExit;
}

void  logicWorker:: setWillExit (bool v)
{
    m_willExit = v;
}

void  logicWorker:: addTimer(udword udwSetp, ComTimerFun pF, void* pUsrData)
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

void  logicWorker:: addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData)
{
    do {
		void* pB[2];
		pB[0] = (void*)pF;
		pB[1] = pUserData;
		addTimer (firstSetp, udwSetp, sTimerProc, pB, sizeof (pB));
    } while (0);
}

void  logicWorker:: addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength)
{
    do {
		auto fnAddComTimer =  getForMsgModuleFunS ().fnAddComTimer;
		auto handle = serverId ();
		fnAddComTimer (handle, firstSetp, udwSetp, pF, pUserData, udwLength);
    } while (0);
}

void logicWorker::addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength)
{
	addTimer (udwSetp, udwSetp, pF, pUserData, udwLength);
}

int  logicWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		// nRet = onLoopBegin ();
        nRet = m_pIUserLogicWorker->onLoopBegin ();
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopEndBase()
{
    int  nRet = 0;
    do {
		// nRet = onLoopEnd ();
        nRet = m_pIUserLogicWorker->onLoopEnd ();
    } while (0);
    return nRet;
}

int  logicWorker:: onLoopFrameBase()
{
    int  nRet = 0;
    do {
		// nRet = onLoopFrame ();
        nRet = m_pIUserLogicWorker->onLoopFrame ();
		/*
		if (willExit ()) {
			nRet |= procPacketFunRetType_exitNow;
		}
		*/
    } while (0);
    return nRet;
}


logicWorker::createChannelMap&   logicWorker:: getCreateChannelMap () 
{
    return m_createChannelMap;
}

static int sendPackToRemoteRetProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;

	auto pU = (sendPackToRemoteRet*)(P2User(pRet));
	myAssert (0 == pU->m_result);
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pBeSend = (pPacketHead )(pAsk->packArg);
	auto nR = pServer->onSendPackToRemoteRet (pBeSend);

	if (procPacketFunRetType_doNotDel & nR) {
		pAsk->packArg = 0;
	} else {
		auto forLogicFun = workerMgr.forLogicFun ();
		auto fnFreePack = forLogicFun->fnFreePack;
		fnFreePack (pBeSend);
	}
	return nRet;
}

static int recRemotePackForYouAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pBeSend = (pPacketHead )(pAsk->packArg);
	auto nR = pServer->onRecRemotePackNtf (pBeSend);
	if (procPacketFunRetType_doNotDel & nR) {
		pAsk->packArg = 0;
	} else {
		auto forLogicFun = workerMgr.forLogicFun ();
		auto fnFreePack = forLogicFun->fnFreePack;
		fnFreePack (pBeSend);
	}
	return nRet;
}

static int recCreateChannelRetProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pU = (createChannelRet*)(P2User(pRet));

	auto pAU = (createChannelAsk*)(P2User(pAsk));

	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto& rCh = *(logicWorker::channelKey*)(pAU->channel);
	auto& rCM = pServer->getCreateChannelMap ();
	uword uwN = 0;
	auto it = rCM.find (rCh);
	if (rCM.end() != it) {
		uwN = it->second;
	}
	rCM[rCh] = ++uwN;

	auto& rConfig = tSingleton<logicFrameConfig>::single ();
	auto routeGroupId = rConfig.routeGroupId ();
	auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
	if (uwN == routeGroup.runNum) {
		rCM.erase(rCh);
		if (pAU->m_sendToMe) {
			pServer->subscribeChannel (rCh);
		}
		pServer->onCreateChannelRet (rCh, pU->m_result);
	}
	gDebug(" rec 	CreateChannelRet result = "<<pU->m_result<<" gateIndex = "<<pU->m_gateIndex<<" uwN = "<<uwN);
	return nRet;
}

static int recDeleteChannelRetProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pU = (deleteChannelRet*)(P2User(pRet));
	gDebug(" rec 	DeleteChannelRet result = "<<pU->m_result);
	return nRet;
}
static int recListenChannelRetProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pU = (subscribeChannelRet*)(P2User(pRet));
	gDebug(" rec 	ListenChannelRet result = "<<pU->m_result);
	return nRet;
}
static int recSayToChannelRetProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pU = (sayToChannelRet*)(P2User(pRet));
	gTrace(" rec SayToChannelRet result = "<<pU->m_result);
	return nRet;
}
static int recLeaveChannelRetProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pU = (leaveChannelRet*)(P2User(pRet));
	gDebug(" rec leaveChannelRet result = "<<pU->m_result);
	return nRet;
}

static int recSendToAllGateRetProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto forLogicFun = workerMgr.forLogicFun ();
	auto fnFreePack = forLogicFun->fnFreePack;
	auto pSend = (pPacketHead)(pAsk->packArg);
	fnFreePack (pSend);
	pAsk->packArg = 0;
	auto pU = (leaveChannelRet*)(P2User(pRet));
	gDebug(" rec leaveChannelRet result = "<<pU->m_result);
	return nRet;
}

static int exitAppNtfProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_exitNow;
	auto pU = (exitAppNtf*)(P2User(pAsk));
	if (pU->exitType == 1) {
		nRet = procPacketFunRetType_exitAfterLoop;
	}
	return nRet;
}

static bool sDelToken (void* pU)
{
	auto ppU = (void**)pU;
    auto pT = (logicWorker*)(ppU[0]);
	auto token = (NetTokenType)((uqword)(ppU[1]));
	pT->delSendPack (token);
	return false;
}

int   logicWorker:: onRecRemotePackNtf (packetHead* pPack)
{
    int   nRet = procPacketFunRetType_del;
    do {
		auto&  workerMgr = logicWorkerMgr::getMgr();
		auto forLogicFun = workerMgr.forLogicFun ();
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		auto fnFindMsg = forLogicFun->fnFindMsg;
		auto fnFreePack = forLogicFun->fnFreePack;
		auto pN = P2NHead(pPack);
		auto procFun = fnFindMsg(serverId(), pN->uwMsgID);
		procPacketArg arg;
		arg.broadcast = false;
		arg.handle = serverId ();
		if (!procFun) {
			gWarn("can not find proc fun msgID is : "<<pN->uwMsgID);
			break;
		}

		if (NIsRet(pN)) {

			gTrace(" rec ret pack by net pack is : "<<*pPack);
			auto& rTokenMap = m_tokenMap;
			auto it = rTokenMap.find (pN->dwToKen);
			if (rTokenMap.end () == it) {
				gWarn(" net pack can not find send token my id is "<<(int)(serverId())<<" pack is :"<<*pPack);
				break;
			} else {
				nRet = procFun(it->second, pPack, &arg);
				fnFreePack (it->second);
				rTokenMap.erase (it);
			}
		} else {
			packetHead* pRet = nullptr;
			nRet = procFun(pPack, pRet, &arg);
			if (pRet) {
				pRet->loopId = pPack->loopId;
				pRet->sessionID = pPack->sessionID;
				auto pRN = P2NHead(pRet);
				pRN->dwToKen = pN->dwToKen;
				pRN->ubyDesServId = pN->ubySrcServId;

				sendPackToRemoteAskMsg ask;
				auto pAsk = ask.pop ();
				pAsk->packArg = (decltype(pAsk->packArg))(pRet);

				pAsk->loopId = serverId ();
				auto pAskN = P2NHead(pAsk);
				pAskN->ubySrcServId = rConfig.appGroupId ();
				pAskN->ubySrcServId <<=8;
				pAskN->ubySrcServId += serverId ();

				pAskN->ubyDesServId = rConfig.appGroupId ();
				pAskN->ubyDesServId <<=8;
				pAskN->ubyDesServId += rConfig.routeGroupId ();
				forLogicFun->fnPushPackToServer(pPack->loopId, pAsk);
			}
		}
    } while (0);
    return nRet;
}

int logicWorker:: onSendPackToRemoteRet (packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
    do {
		auto pN = P2NHead(pPack);
		if (NIsRet(pN)) {
			break;
		}
		gTrace(" send ask pack by net pack is : "<<*pPack);
		m_tokenMap[pN->dwToKen] = pPack;
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto sept = rConfig.delSaveTokenTime ();
		void* args[2];
		args[0] = this;
		args[1] = (void*)(pN->dwToKen);
		addTimer (sept, sDelToken, args, sizeof(args));
		nRet = procPacketFunRetType_doNotDel;
    } while (0);
	return nRet;
}

int   logicWorker:: recPacketProcFun (ForLogicFun* pForLogic)
{
    int   nRet = 0;
    do {
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnRegMsg = sMgr.forLogicFun ()->fnRegMsg;
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sendPackToRemoteRet), sendPackToRemoteRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_recRemotePackForYouAsk), recRemotePackForYouAskProcFun);

		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_createChannelRet), recCreateChannelRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_deleteChannelRet), recDeleteChannelRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_subscribeChannelRet), recListenChannelRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sayToChannelRet), recSayToChannelRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_leaveChannelRet), recLeaveChannelRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sendToAllGateRet), recSendToAllGateRetProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_exitAppNtf), exitAppNtfProcFun);

    } while (0);
    return nRet;
}

void   logicWorker:: delSendPack (NetTokenType  token)
{
    do {
		auto it = m_tokenMap.find (token);
		if (m_tokenMap.end() != it) {
			auto& smgr = logicWorkerMgr::getMgr();
			smgr.forLogicFun()->fnFreePack (it->second);
			m_tokenMap.erase(it);
		}
    } while (0);
}

NetTokenType   logicWorker:: newToken()
{
    return logicWorkerMgr::getMgr().forLogicFun()->fnNextToken (serverId());
}
/*
int   logicWorker:: sendBroadcastPack (packetHead* pack)
{
    int   nRet = 0;
    do {

		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto gateNodeNum = rConfig.gateNodeNum ();
		auto routeGroupId = rConfig.routeGroupId ();
		auto& routeGroup= rConfig.serverGroups ()[routeGroupId];
		myAssert (gateNodeNum == routeGroup.runNum);

		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		pack->loopId = routeGroup.beginId;
		for (decltype (routeGroup.runNum) i = 1; i < routeGroup.runNum; i++) {
			auto pSend = nClonePack(P2NHead(pack));
			pack->loopId = routeGroup.beginId + i;
		}
		sendPacket(pack, appG, serverG);
    } while (0);
    return nRet;
}
*/

int   logicWorker:: createChannel (const channelKey& rKey)
{
	return createChannel (rKey, true);
}

int   logicWorker:: createChannel (const channelKey& rKey, bool sayToMe)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		createChannelAskMsg msg;
		auto pAsk = msg.pop();
		auto pN = P2NHead(pAsk);

		pN->ubySrcServId = rConfig.appGroupId ();
		pN->ubySrcServId <<= 8;
		pN->ubySrcServId |= serverId ();

		auto pU = (createChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		rCh = rKey;
		pU->m_sendToMe = sayToMe ? 1 : 0;
		NSetToAllGate(pN);
		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		sendPacket(pAsk, appG, serverG);

		// sendBroadcastPack (pAsk);
		// sendToAllGate (pAsk);

    } while (0);
    return nRet;
}

int   logicWorker:: deleteChannel (const channelKey& rKey)
{
    int   nRet = 0;
    do {
		deleteChannelAskMsg msg;
		auto pAsk = msg.pop();
		auto pN = P2NHead(pAsk);
		auto pU = (deleteChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		rCh = rKey;

		NSetToAllGate(pN);
		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		sendPacket(pAsk, appG, serverG);
		// sendBroadcastPack (pAsk);
		// sendToAllGate (pAsk);
    } while (0);
    return nRet;
}

int   logicWorker:: subscribeChannel (const channelKey& rKey)
{
    int   nRet = 0;
    do {
		subscribeChannelAskMsg msg;
		auto pAsk = msg.pop();
		auto pN = P2NHead(pAsk);
		auto pU = (subscribeChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		rCh = rKey;
		// sendBroadcastPack (pAsk);
		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		sendPacket(pAsk, appG, serverG);
    } while (0);
    return nRet;
}

int   logicWorker:: sayToChannel (const channelKey& rKey, packetHead* pack)
{
    int   nRet = 0;
    do {
		auto&  sMgr = logicWorkerMgr::getMgr();
		auto fnFreePack = sMgr.forLogicFun ()->fnFreePack;
		auto fnAllocPack  = sMgr.forLogicFun()->fnAllocPack;
		pPacketHead pNew = nullptr;
		auto pSN = P2NHead(pack);
		NSetDesOnce(pSN);
		sMgr.toNetPack (pSN, pNew);
		if (pNew) {
			fnFreePack (pack);
			pack = pNew;
			pSN = P2NHead(pack);
		}
		auto packLen = sizeof(*pSN) + pSN->udwLength;
		auto pZ = (sayToChannelAsk*)(0);
		auto uqwOff = (uqword)(pZ->pack);
		uqwOff += packLen;
		auto pAsk = (packetHead*)(fnAllocPack((udword)uqwOff));
		pAsk->loopId = c_emptyLocalServerId;
		pAsk->sessionID = EmptySessionID;
		auto pAskN = P2NHead(pAsk);
		pAskN->uwMsgID = internal2FullMsg(internalMsgId_sayToChannelAsk);
		auto pAskU = (sayToChannelAsk*)(N2User(pAskN));
		auto& rCh = *((channelKey*)(pAskU->channel));
		rCh = rKey;
		pAskU->packSize = packLen;
		memcpy(pAskU->pack, pSN, packLen);
		fnFreePack (pack);

		NSetToAllGate(pAskN);
		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		sendPacket(pAsk, appG, serverG);

		// sendBroadcastPack (pAsk);
		// sendToAllGate (pAsk);
		
    } while (0);
    return nRet;
}

int   logicWorker:: leaveChannel (const channelKey& rKey)
{
    int   nRet = 0;
	do {
		leaveChannelAskMsg msg;
		auto pAsk = msg.pop();
		auto pN = P2NHead(pAsk);
		auto pU = (leaveChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		rCh = rKey;

		serverIdType	ubyDesServId = c_emptyLoopHandle;
		auto serverG = (ubyte)(ubyDesServId );
		ubyDesServId >>= 8;
		auto appG = (ubyte)(ubyDesServId);
		sendPacket(pAsk, appG, serverG);
		// sendBroadcastPack (pAsk);
	} while (0);
    return nRet;
}

void*   logicWorker:: userData ()
{
	return m_userData;
}

void   logicWorker:: setUserData (void* v)
{
	m_userData = v;
}

void  logicWorker:: ntfOtherLocalServerExit(bool bIncMe)
{
    auto& sMgr = logicWorkerMgr::getMgr();
	auto allServerNum = sMgr.allServerNum ();
	for (decltype (allServerNum) i = 0; i < allServerNum; i++) {
        if (i == serverId() && !bIncMe) {
            continue;
        }
		exitAppNtfMsg  msg;
		auto pExitPack = msg.pop();
		auto pEN = P2NHead(pExitPack);
		pEN->ubySrcServId = serverId();
		pEN->ubyDesServId = i;
		sMgr.forLogicFun()->fnPushPackToServer (i, pExitPack);
	}
}

int   logicWorker:: sendToAllGate (packetHead* pack)
{
    int   nRet = 0;
    do {
		sendToAllGateAskMsg ask;
		auto pAsk = ask.pop();
		pAsk->packArg = (decltype(pAsk->packArg)) pack;
		auto pAN = P2NHead(pAsk);
		pAN->ubySrcServId = serverId ();

		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto routeGroupId = rConfig.routeGroupId ();
		auto myAppGroupId = rConfig.appGroupId ();
		sendPacket (pAsk, myAppGroupId, routeGroupId);
    } while (0);
    return nRet;
}

void  logicWorker:: stringToChannel (const char* szCh, channelKey& rCh)
{
	std::stringstream ss(szCh);
	ss>>std::hex>>rCh.first>>rCh.second;
}

IUserLogicWorker*   logicWorker:: getIUserLogicWorker ()
{
    return m_pIUserLogicWorker.get();
}

/*
IUserLogicWorker*  logicWorker:: pIUserLogicWorker ()
{
    return m_pIUserLogicWorker;
}

void  logicWorker:: setPIUserLogicWorker (IUserLogicWorker* v)
{
    m_pIUserLogicWorker = v;
}
*/
