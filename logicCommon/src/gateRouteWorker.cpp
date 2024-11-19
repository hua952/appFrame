#include <vector>
#include <sstream>
#include "gateRouteWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "gLog.h"
#include "internalRpc.h"
#include "internalMsgId.h"
#include "internalMsgGroup.h"
#include "internalChannel.h"

bool gateRouteWorker::cmpSessionChannelKey::operator ()(const sessionChannelKey& k1,const sessionChannelKey& k2)const
{
	bool bRet = true;
	do {
		if (k1.first < k2.first) {
			break;
		}
		if (k2.first < k1.first) {
			bRet = false;
			break;
		}
		cmpChannelKey funCom;
		bRet = funCom (k1.second, k2.second);
	} while(0);
	return bRet;
}
gateRouteWorker:: gateRouteWorker ()
{
}

gateRouteWorker:: ~gateRouteWorker ()
{
}

int  gateRouteWorker:: onLoopBeginBase()
{
    int  nRet = 0;
    do {
		int nR = 0;	
		endPoint endP[2];
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto appIndex = rConfig.gateIndex();
		auto gateNodeNum = rConfig.gateNodeNum ();
		myAssert (gateNodeNum);
		if (appIndex >= gateNodeNum) {
			appIndex = 0;
		}
		myAssert (appIndex < gateNodeNum);
		auto gateNodes = rConfig.gateNodes ();
		auto& gateNode = gateNodes[appIndex];
		myAssert (gateNode.endPointNum <= 2);
		auto serverGroup = this->serverGroup ();
		auto serverGroupNum = rConfig.serverGroupNum ();
		myAssert (serverGroup < serverGroupNum);
		auto serverGroups = rConfig.serverGroups ();
		auto& rRoutGroup = serverGroups[serverGroup];
		auto serId = this->serverId();
		myAssert (serId >= rRoutGroup.beginId);
		auto serIndex = serId - rRoutGroup.beginId;
		myAssert (serIndex < rRoutGroup.runNum);
		for (decltype (gateNode.endPointNum) i = 0; i < gateNode.endPointNum; i++) {
			strNCpy (endP[i].ip, sizeof(endP[i].ip), gateNode.endPoints[i].first.get());
			endP[i].port = gateNode.endPoints[i].second + serIndex;
			endP[i].ppIConnector = nullptr;
			endP[i].userDataLen = 0;
			endP[i].userData = nullptr;
		}
	
		nR = initNet (endP, gateNode.endPointNum, nullptr, 0, nullptr, 0);
		if (nR) {
			gError("initNet error nR = "<<nR<<" endpoint[0] = "<<endP[0].ip<<" : "<<endP[0].port);
			nRet = 1;
			break;
		}
		gInfo("initNet ok endpointNum is : "<<gateNode.endPointNum<<" endpoint[0] = "<<endP[0].ip<<" : "<<endP[0].port<<" my serverid is : "<<(int)(serverId()));
		channelKey  ch;
		stringToChannel (GM_CHANNEL, ch);
		addChannel (ch);
		nR = routeWorker:: onLoopBeginBase();
		if (nR) {
			nRet = 2;
			break;
		}
    } while (0);
    return nRet;
}

static int regAppRouteAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	regAppRouteRetMsg ret;
    
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	pRet = ret.pop();
	auto pU = (regAppRouteRet*)(P2User(pRet));
	auto pAU = (regAppRouteAsk*)(P2User(pAsk));
	pU->m_result = (ubyte)(pGate->regAppRoute (pAU->m_appGrupId, pAsk->sessionID));
	gInfo(" rec AppRouteAsk app group id is : "<<(int)(pAU->m_appGrupId)<<" my server id is : "<<(int)(pArg->handle));
	nRet |= procPacketFunRetType_stopBroadcast;
	return nRet;
}

static int recBroadcastPacketNtfProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	// gDebug(" rec 	BroadcastPacketNtf");
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	nRet = pGate->processNtfBroadcastPackFun(pAsk);
	return nRet;
}

static int recCreateChannelAskProcFun (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	// gDebug(" rec 	CreateChannelAsk ");
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	auto ppRet = pArg->broadcast ? nullptr : &pRet;
	nRet = pGate->onCreateChannelAsk(pAsk, ppRet);
	return nRet;
}


static int recDeleteChannelAskProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gDebug(" rec 	DeleteChannelAsk");
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	auto ppRet = pArg->broadcast ? nullptr : &pRet;
	nRet = pGate->onDeleteChannelAsk(pAsk, ppRet);
	return nRet;
}
static int recListenChannelAskProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gDebug(" rec 	ListenChannelAsk");
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	auto ppRet = pArg->broadcast ? nullptr : &pRet;
	nRet = pGate->onSubscribeChannelAsk(pAsk, ppRet);
	return nRet;
}
static int recSayToChannelAskProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	gTrace(" rec SayToChannelAsk");
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	auto ppRet = pArg->broadcast ? nullptr : &pRet;
	nRet = pGate->onSayToChannelAsk(pAsk, ppRet);
	return nRet;
}
static int recLeaveChannelAskProcFun  (pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto&  workerMgr = logicWorkerMgr::getMgr();
	auto pServer = workerMgr.findServer (pArg->handle);
	myAssert (pServer);
	auto pGate = dynamic_cast<gateRouteWorker*>(pServer);
	myAssert (pGate);
	auto ppRet = pArg->broadcast ? nullptr : &pRet;
	nRet = pGate->onLeaveChannelAsk(pAsk, ppRet);
	gDebug(" rec leaveChannelAsk");
	return nRet;
}

int  gateRouteWorker:: onSayToChannelAsk(packetHead* pack, pPacketHead* ppRet)
{
	int nRet = procPacketFunRetType_del;
	udword result = 0;	
	auto& rConfig = tSingleton<logicFrameConfig>::single();
	auto& rMgr = logicWorkerMgr::getMgr();
	auto forLogicFun  = rMgr.forLogicFun ();
    do {
		auto& rMap = m_channelMap;
		auto pN = P2NHead(pack);
		auto pU = (sayToChannelAsk*)(N2User(pN));
		
		auto it = rMap.find(*((channelKey*)(pU->channel)));
		if (rMap.end() == it) {
			result = 1;
			break;
		}
		auto pUN = (netPacketHead*)(pU->pack );
		for (auto ite = it->second.begin (); ite != it->second.end (); ite++) {
			auto desAddr = (serverIdType)(*ite);
			auto uqwV = *ite;
			auto ubyDesServId = (serverIdType)(uqwV);
			uqwV>>=32;
			auto sessionId = (SessionIDType)(uqwV);

			if (sessionId == EmptySessionID) {
				packetHead* pNew = nullptr;
				if (pUN->udwLength) {
					rMgr.fromNetPack (pUN, pNew);
				}
				if (!pNew) {
					pNew = nClonePack (pUN);
				}
				auto pNN = P2NHead(pNew);
				pNN->ubyDesServId = ubyDesServId;
				forLogicFun->fnPushPackToServer (ubyDesServId, pNew);
			} else {
				auto pS = getSession(sessionId);
				if (!pS) {
					gTrace(" can not find session sessionId is : "<<sessionId<<" obj serverId = "<<desAddr);
					continue;
				}
				auto pNew = nClonePack (pUN);
				auto pNN = P2NHead(pNew);
				pNN->ubyDesServId = ubyDesServId;
				pS->send(pNew);
			}
		}
	} while (0);
	if (ppRet) {
		sayToChannelRetMsg ret;
		auto pRet = ret.pop();
		auto pRN = P2NHead(pRet);
		auto pRU = (sayToChannelRet*)(N2User(pRN));
		pRU->m_result = result;
		pRU->m_gateIndex = rConfig.gateIndex();
		*ppRet = pRet;
	}
    return nRet;
}

int   gateRouteWorker:: addChannel (const channelKey& rCh)
{
    int   nRet = 0;
    do {
		auto& rMap = m_channelMap;
		auto inRet = rMap.insert(std::make_pair(rCh , channelValue ()));
		if (!inRet.second) {
			nRet = 1;
			break;
		}
    } while (0);
    return nRet;
}

int gateRouteWorker:: onCreateChannelAsk(packetHead* pack, pPacketHead* ppRet)
{
	int nRet = procPacketFunRetType_del;
	udword result = 0;	
	auto& rConfig = tSingleton<logicFrameConfig>::single();
    do {
		auto& rMap = m_channelMap;
		auto pN = P2NHead(pack);
		auto pU = (createChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		/*
		uqword  uqwValue = pack->sessionID;
		uqwValue <<=32;
		uqwValue += pN->ubySrcServId;
		*/
		int nR = addChannel (rCh);
		// auto inRet = rMap.insert(std::make_pair(rCh , channelValue ()));
		if (nR) {
			result = 1;
			break;
		}
		/*
		if (ppRet) {
			inRet.first->second.insert(uqwValue);
		}
		*/
		gInfo(" create channel : "<<std::hex<<rCh.first<<" "<<rCh.second<<" my id is "<<(int)(serverId())<<" sessionId = "<<pack->sessionID<<" serverId = "<<(int)(pN->ubySrcServId));
	} while (0);
	if (ppRet) {
		createChannelRetMsg ret;
		auto pRet = ret.pop();
		auto pRN = P2NHead(pRet);
		auto pRU = (createChannelRet*)(N2User(pRN));
		pRU->m_result = result;
		pRU->m_gateIndex = rConfig.gateIndex();
		*ppRet = pRet;
	}
	return nRet;
}

int  gateRouteWorker:: onDeleteChannelAsk(packetHead* pack, pPacketHead* ppRet)
{
	int nRet = procPacketFunRetType_del;
	udword result = 0;	
	auto& rConfig = tSingleton<logicFrameConfig>::single();
    do {
		auto& rMap = m_channelMap;
		auto& rSessChMap = m_sessionChannelMap;
		auto pN = P2NHead(pack);
		auto pU = (deleteChannelAsk*)(N2User(pN));
		auto& rCh = *((channelKey*)(pU->channel));
		auto iter = rMap.find (rCh);
		if (iter == rMap.end()) {
			result = 1;
			break;
		}
		auto& rSet = iter->second;
		for (auto it = rSet.begin (); it != rSet.end (); it++) {
			auto pa = std::make_pair(*it, rCh);
			rSessChMap.erase(pa);
		}
		rMap.erase(iter);
    } while (0);
	if (ppRet) {
		deleteChannelRetMsg ret;
		auto pRet = ret.pop();
		auto pRN = P2NHead(pRet);
		auto pRU = (deleteChannelRet*)(N2User(pRN));
		pRU->m_result = result;
		pRU->m_gateIndex = rConfig.gateIndex();
		*ppRet = pRet;
	}
    return nRet;
}

int  gateRouteWorker:: leaveChannel (const channelKey& rKey, SessionIDType sessionId, serverIdType	serverId)
{
    int  nRet = 0;
    do {
		auto& rMap = m_channelMap;
		auto it = rMap.find(rKey);
		if (it == rMap.end()) {
			nRet = 1;
			break;
		}
		uqword  uqwValue = sessionId;
		uqwValue <<=32;
		uqwValue += serverId;
		auto& rSet = it->second;
		rSet.erase (uqwValue);
		auto& rSessChMap = m_sessionChannelMap;

		auto pa = std::make_pair(uqwValue, rKey);
		rSessChMap.erase(pa);
		gDebug(" level channel sessionId = "<<sessionId<<std::hex<<" serverId = "<<serverId<<" channel = "<<rKey.first<<" "<<rKey.second);
    } while (0);
    return nRet;
}

int  gateRouteWorker:: subscribeChannel (const channelKey& rKey, SessionIDType sessionId, serverIdType	serverI)
{
    int  nRet = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rMap = m_channelMap;
		
		auto it = rMap.find(rKey);
		if (it == rMap.end()) {
			nRet = 1;
			break;
		}
		uqword  uqwValue = sessionId;
		uqwValue <<=32;
		uqwValue += serverI;
		auto inRet = it->second.insert(uqwValue);
		if (!inRet.second) {
			nRet = 2;
			break;
		}
		auto& rSessChMap = m_sessionChannelMap;
		rSessChMap.insert (std::make_pair(uqwValue, rKey));
		gDebug(" subscribeChannel channel : "<<std::hex<<rKey.first<<" "<<rKey.second<<" my id is "<<(int)(serverId())<<" sessionId = "<<sessionId<<" serverId = "<<serverI);
    } while (0);
    return nRet;
}

int  gateRouteWorker:: onSubscribeChannelAsk(packetHead* pack, pPacketHead* ppRet)
{
    int  nRet = procPacketFunRetType_del;
	udword result = 0;
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rMap = m_channelMap;
		auto pN = P2NHead(pack);
		auto pU = (subscribeChannelAsk*)(N2User(pN));
		/*	
		auto it = rMap.find(*((channelKey*)(pU->channel)));
		if (it == rMap.end()) {
			result = 1;
			break;
		}
		uqword  uqwValue = pack->sessionID;
		uqwValue <<=32;
		uqwValue += pN->ubySrcServId;
		auto inRet = it->second.insert(uqwValue);
		if (!inRet.second) {
			result = 2;
			break;
		}
		*/
		result = subscribeChannel (*((channelKey*)(pU->channel)), pack->sessionID, pN->ubySrcServId);
    } while (0);
	if (ppRet) {
		subscribeChannelRetMsg ret;
		auto pRet = ret.pop();
		auto pRN = P2NHead(pRet);
		auto pRU = (subscribeChannelRet*)(N2User(pRN));
		pRU->m_result = result;
		*ppRet = pRet;
	}
	nRet |= procPacketFunRetType_stopBroadcast;
    return nRet;
}

int  gateRouteWorker:: onLeaveChannelAsk(packetHead* pack, pPacketHead* ppRet)
{
    int  nRet = procPacketFunRetType_del;
	udword result = 0;	
    do {
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rMap = m_channelMap;
		auto pN = P2NHead(pack);
		auto pU = (leaveChannelAsk*)(N2User(pN));
		result = leaveChannel (*((channelKey*)(pU->channel)), pack->sessionID, pN->ubySrcServId);
		/*
		auto it = rMap.find(*((channelKey*)(pU->channel)));
		if (it == rMap.end()) {
			result = 1;
			break;
		}
		uqword  uqwValue = pack->sessionID;
		uqwValue <<=32;
		uqwValue += pN->ubySrcServId;
		auto inRet = it->second.erase(uqwValue);
		if (!inRet) {
			result = 2;
			break;
		}
		*/
    } while (0);
	if (ppRet) {
		leaveChannelRetMsg ret;
		auto pRet = ret.pop();
		auto pRN = P2NHead(pRet);
		auto pRU = (leaveChannelRet*)(N2User(pRN));
		pRU->m_result = result;
		*ppRet = pRet;
	}
    return nRet;
}

int   gateRouteWorker:: regAppRoute (ubyte group, SessionIDType sessionId)
{
    int   nRet = 0;
    do {
		auto& rAppMap = m_appMap;
		uqword fk = group;
		auto endK = fk + 1;
		fk<<=32;
		endK<<=32;
		auto lower = rAppMap.lower_bound(fk); 
		auto upper = rAppMap.upper_bound(endK);
		udword last = 0;
		for (auto it = lower; it != upper; it++) {
			last = (ubyte)(it->first);
		}
		last++;
		auto key = fk;
		key += last;
		auto pSession = getSession (sessionId);
		myAssert (pSession);
		if (!pSession) {
			nRet = 1;
			break;
		}
		rAppMap[key] = pSession;
		pSession->setUserData (&key, sizeof(key));
    } while (0);
    return nRet;
}

int  gateRouteWorker:: sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession)
{
    int  nRet = 0;
    do {
		// auto& rAppMap = m_appMap;
		auto pN = P2NHead(pPack);
		ISession* pSession = nullptr;
		if (NIsRet(pN)) {
			pSession = getSession (pPack->sessionID);
			if (!pSession) {
				logicWorkerMgr::getMgr().forLogicFun ()->fnFreePack(pPack);
				nRet = 1;
				break;
			}
		} else {
			pSession = getOnceAppSession(pN->ubyDesServId>>8);
			if (!pSession) {
				logicWorkerMgr::getMgr().forLogicFun ()->fnFreePack(pPack);
				gWarn("can not find app session for pack :"<<*pPack);
				rRet.m_result = 1;
				break;
			}
		}
		pSession->send (pPack);
		rRet.m_result = 0;
    } while (0);
    return nRet;
}

ISession*   gateRouteWorker:: getOnceAppSession(ubyte appGroup)
{
	ISession*   nRet = nullptr;
	do {
		uqword fk = appGroup;
		auto endK = fk + 1;
		fk<<=32;
		endK<<=32;
		auto& rAppMap = m_appMap;
		auto lower = rAppMap.lower_bound(fk); 
		auto upper = rAppMap.upper_bound(endK);
		std::vector<ISession*> vec;
		for (auto it = lower; it != upper; it++) {
			vec.push_back(it->second);
		}
		// myAssert (!vec.empty());
		if (vec.empty()) {
			break;
		}
		auto n = rand()%vec.size();
		nRet = vec[n];
	} while (0);
	return nRet;
}

void  gateRouteWorker:: onClose(ISession* session)
{
    do {
		auto pKey = (uqword*)(session->userData());
		if (pKey ) {
			auto key = *(pKey);
			auto& rAppMap = m_appMap;
			gInfo(" server session close sessionId = "<<session->id()<<" key is : "<<std::hex<<key);
			rAppMap.erase(key);
		}
		auto sessionId = session->id();
		sessionChannelKey beginKey;
		sessionChannelKey endKey;
		beginKey.first = sessionId;
		endKey.first = beginKey.first + 1;
		beginKey.first <<= 32;
		endKey.first <<= 32;

		std::vector<sessionChannelKey> delV;
		auto& rSM = m_sessionChannelMap;
		auto beginIter = rSM.lower_bound (beginKey);
		auto endIter = rSM.upper_bound (endKey);
		for (auto it = beginIter; it != endIter; ++it) {
			delV.push_back(*it);
		}
		for (auto it = delV.begin (); it != delV.end (); it++) {
			leaveChannel (it->second, sessionId, (serverIdType)(it->first));
		}
    } while (0);
}
/*
int  gateRouteWorker:: localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc)
{
    int  nRet = 0;
    do {
		auto baseProc = false;
		auto nR = routeWorker::localProcessNetPackFun(session, pack, baseProc);
		if (baseProc) {
			nRet = nR;
			break;
		}
		auto pN = P2NHead(pack);
		myAssert (c_emptyLoopHandle == pN->ubyDesServId);

    } while (0);
    return nRet;
}
*/
int   gateRouteWorker:: recPacketProcFun (ForLogicFun* pForLogic)
{
    int   nRet = 0;
    do {
		auto nR = routeWorker::recPacketProcFun (pForLogic);
		if (nR) {
			nRet = 1;
			break;
		}
		auto& sMgr = logicWorkerMgr::getMgr();
		auto fnRegMsg = sMgr.forLogicFun()->fnRegMsg;
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_regAppRouteAsk), regAppRouteAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_createChannelAsk), recCreateChannelAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_deleteChannelAsk), recDeleteChannelAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_subscribeChannelAsk), recListenChannelAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_sayToChannelAsk), recSayToChannelAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_leaveChannelAsk), recLeaveChannelAskProcFun);
		fnRegMsg (serverId (), internal2FullMsg(internalMsgId_broadcastPacketNtf), recBroadcastPacketNtfProcFun);
    } while (0);
    return nRet;
}

static bool sDelToken(void* pU)
{
	auto ppT = (uqword*)(pU);
	auto pT = (gateRouteWorker*)(ppT[0]);
	auto token = (NetTokenType)(ppT[1]);

	auto& rMap = pT->getTokenMap ();
	auto erRet = rMap.erase(token);
	// myAssert (!erRet);
	if (erRet) {
		gWarn(" token delete by timer token is : "<<token);
	}
	return false;
}

gateRouteWorker::tokenMap&   gateRouteWorker:: getTokenMap ()
{
    return m_tokenMap;
}

int  gateRouteWorker:: processBroadcastPackFun(ISession* session, packetHead* pack)
{
    int  nRet = procPacketFunRetType_del;
	auto& sMgr = logicWorkerMgr::getMgr();
	auto forLogicFun = sMgr.forLogicFun ();
	auto& rConfig = tSingleton<logicFrameConfig>::single();
    do {
		gTrace( "BroadcastPack pack is : "<<*pack);
		pack->sessionID = session ? session->id() : EmptySessionID;
		auto pN = P2NHead(pack);
		myAssert (!NIsRet(pN));
		auto fnFindMsg = forLogicFun->fnFindMsg;
		auto procFun = fnFindMsg(serverId(), pN->uwMsgID);
		procPacketArg arg;
		arg.broadcast = false;
		arg.handle = serverId ();
		if (!procFun) {
			gWarn("can not find proc fun msgID is : "<<pN->uwMsgID);
			break;
		}

		auto netNum = rConfig.netNum ();
		
		packetHead* pRet = nullptr;
		auto nR = procFun(pack, pRet, &arg);
		bool stopBroadcast = procPacketFunRetType_stopBroadcast & nR || 1 == netNum;

		if (pRet) {
			pRet->loopId = pack->loopId;
			pRet->sessionID = pack->sessionID;
			auto pRN = P2NHead(pRet);
			pRN->dwToKen = pN->dwToKen;
			pRN->ubyDesServId = pN->ubySrcServId;

			pRN->ubySrcServId = rConfig.appGroupId ();
			pRN->ubySrcServId <<= 8;
			pRN->ubySrcServId |= serverId ();
		}

		if (stopBroadcast) {
			if (pRet) {
				if (session) {
					session->send(pRet);
				} else {
					pRet->packArg = (decltype(pRet->packArg))(pack);
					nRet |= procPacketFunRetType_doNotDel;
					forLogicFun->fnPushPackToServer(pack->loopId, pRet);
				}
			}
			break;
		}

		broadcastPacketNtfMsg  ntf;
		auto pNtf = ntf.pop();
		auto pNtfN = P2NHead(pNtf);
		auto pNtfU = (broadcastPacketNtf*)(N2User(pNtfN));

		pNtfU->retPack = (decltype(pNtfU->retPack))(pRet);
		pNtfU->sessionId = session ? session->id() : EmptySessionID;
		pNtfU->srcServer = pack->loopId;
		// pNtfU->passNum = 0;
		pNtf->packArg= (decltype(pNtf->packArg))(pack);
		pNtf->loopId = serverId ();

		pNtfN->ubySrcServId = rConfig.appGroupId ();
		pNtfN->ubySrcServId <<=8;
		pNtfN->ubySrcServId += serverId ();

		auto& info = rConfig.serverGroups()[serverGroup ()];
		myAssert (info.runNum == netNum);
		auto nextId = serverId();
		myAssert (nextId >= info.beginId);
		nextId -= info.beginId;
		nextId++;
		nextId %= info.runNum;
		nextId += info.beginId;

		pNtfN->ubyDesServId = rConfig.appGroupId ();
		pNtfN->ubyDesServId <<=8;
		pNtfN->ubyDesServId += nextId;
		// pNtfN->ubyDesServId += serverId ();
		forLogicFun->fnPushPackToServer(nextId, pNtf);
		nRet = procPacketFunRetType_doNotDel;
    } while (0);
    return nRet;
}

int  gateRouteWorker:: processNtfBroadcastPackFun(packetHead* pack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		auto& sMgr = logicWorkerMgr::getMgr();
		auto forLogicFun = sMgr.forLogicFun();
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto myServerId = serverId();
		auto pAsk = (packetHead*)(pack->packArg);
		auto pN = P2NHead(pack);
		auto pU = (broadcastPacketNtf*)(N2User(pN));
		if ((ubyte)(pN->ubySrcServId)  == myServerId ) {
			auto pSend = (packetHead*)(pU->retPack);
			if (pSend) {
				if (pU->sessionId == EmptySessionID) {
					pSend->packArg = (decltype(pSend->packArg))(pack);
					nRet |= procPacketFunRetType_doNotDel;
					forLogicFun->fnPushPackToServer(pU->srcServer, pSend);
				} else {
					auto pS = getSession(pU->sessionId);
					myAssert (pS);
					if (pS) {
						pS->send(pSend);
					} else {
						gWarn("Broadcast ret can not find session sessionId = "<<pU->sessionId);
						forLogicFun->fnFreePack(pSend);
					}
				}
			}
			forLogicFun->fnFreePack(pAsk);
			break;
		}
		auto pAN = P2NHead(pAsk);
		auto fnFindMsg = forLogicFun->fnFindMsg;
		auto procFun = fnFindMsg(serverId(), pAN->uwMsgID);
		auto netNum = rConfig.netNum ();
		myAssert (procFun);
		if (!procFun) {
			gWarn(" can not find proFun pack = "<<*pack);
			break;
		}

		procPacketArg arg;
		arg.broadcast = true;
		arg.handle = serverId ();
		packetHead* pRet = nullptr;
		auto nR = procFun(pAsk, pRet, &arg);
		myAssert (!pRet);
		
		auto& info = rConfig.serverGroups()[serverGroup ()];
		myAssert (info.runNum == netNum);
		auto nextId = serverId();
		myAssert (nextId >= info.beginId);
		nextId -= info.beginId;
		nextId++;
		nextId %= info.runNum;
		nextId += info.beginId;

		pN->ubyDesServId = rConfig.appGroupId ();
		pN->ubyDesServId <<=8;
		pN->ubyDesServId += nextId;
		
		forLogicFun->fnPushPackToServer(nextId, pack);
		nRet = procPacketFunRetType_doNotDel;
    } while (0);
    return nRet;
}

void  gateRouteWorker:: sendHeartbeat ()
{
    do {
		auto& rMap = m_appMap;
		for (auto it = rMap.begin (); it != rMap.end (); it++) {
			heartbeatAskMsg msg;
			auto pack = msg.pop();
			auto pS = it->second;
			pS->send (pack);
		}
    } while (0);
}

int  gateRouteWorker:: processNetPackFun(ISession* session, packetHead* pack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		nRet = routeWorker::processNetPackFun (session, pack);
		if (nRet & procPacketFunRetType_stopBroadcast) {
			break;
		}
		auto& rConfig = tSingleton<logicFrameConfig>::single();
		auto& rTokenMap = m_tokenMap;
		auto& sMgr = logicWorkerMgr::getMgr();
		auto pN = P2NHead(pack);
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			nRet = processBroadcastPackFun (session, pack);
			break;
		}
		auto objG = pN->ubyDesServId;
		objG>>=8;
		/*
		auto myG = rConfig.appGroupId ();
		if (objG == myG) {
			nRet = routeWorker::processNetPackFun (session, pack);
			break;
		}
		*/
		ISession* pSendSession = nullptr;
		do {
			if (NIsRet(pN)) {
				auto it = rTokenMap.find (pN->dwToKen);
				if (rTokenMap.end() == it) {
					gWarn(" can not find old token netPacketHead is : "<<*pN);
					break;
				}
				pN->dwToKen = it->second.oldToken;
				pSendSession = getSession (it->second.sessionId);
				rTokenMap.erase(it);
			} else {
				tokenSaveInfo info;
				info.oldToken = pN->dwToKen;
				info.sessionId = session->id();
				pN->dwToKen = newToken();
				auto inRet = rTokenMap.insert(std::make_pair(pN->dwToKen, info));
				myAssert (inRet.second);
				if (!inRet.second) {
					break;
				}
				pSendSession = getOnceAppSession (objG);
				if (!pSendSession) {
					break;
				}
				auto sept = rConfig.delSaveTokenTime ();
				uqword uqwA[2];
				uqwA[0] = (uqword)(this);
				uqwA[1] = pN->dwToKen;
				addTimer(sept, sDelToken, uqwA, sizeof(uqwA));
			}
			if (pSendSession) {
				pSendSession->send(pack);
				nRet = procPacketFunRetType_doNotDel;
			} else {
				gWarn(" can not find session to forward pack is : "<<*pack);
			}
		} while (0);
    } while (0);
    return nRet;
}
/*
int   gateRouteWorker:: sendBroadcastPack (packetHead* pack)
{
    int   nRet = 0;
    do {
		myAssert (0);
    } while (0);
    return nRet;
}
*/
int  gateRouteWorker:: processOncePack(packetHead* pPack)
{
    int  nRet = 0;
    do {
		auto pN = P2NHead(pPack);
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			nRet = processBroadcastPackFun (nullptr, pPack) | procPacketFunRetType_stopBroadcast;
		}
    } while (0);
    return nRet;
}

int   gateRouteWorker:: sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId)
{
    int   nRet = 0;
    do {
		serverIdType	ubyDesServId = appGroupId;
		ubyDesServId <<= 8;
		ubyDesServId |= threadGroupId;
		if (c_emptyLoopHandle == ubyDesServId) {
			auto& rMgr = logicWorkerMgr::getMgr();
			auto forLogicFun  = rMgr.forLogicFun ();
			auto& rConfig = tSingleton<logicFrameConfig>::single();

			auto pN = P2NHead(pPack);
			pN->ubySrcServId = rConfig.appGroupId ();
			pN->ubySrcServId <<= 8;
			pN->ubySrcServId |= serverId ();

			pN->ubyDesServId = c_emptyLoopHandle;
			pPack->loopId = serverId ();
			forLogicFun->fnPushPackToServer (serverId (), pPack);
		} else {
			nRet = logicWorker::sendPacket(pPack, appGroupId, threadGroupId);
		}
    } while (0);
    return nRet;
}
