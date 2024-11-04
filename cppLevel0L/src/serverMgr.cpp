/*
#include <iostream>
#include <sstream>
#include <thread>
#include "serverMgr.h"
#include "strFun.h"
#include "loop.h"
#include "tSingleton.h"
#include "comFun.h"
#include "cppLevel0LCom.h"

#include <set>
#include <map>
#include <string>
#include "CModule.h"
#include "myAssert.h"
#include "mLog.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "modelLoder.h"
#include "comMsgMsgId.h"
#include <vector>
#include "argConfig.h"
#include <time.h>
#include <mutex>

allocPackFT  allocPack = nullptr;
freePackFT  freePack = nullptr;
outMemLeakFT  outMemLeak = nullptr;

static std::mutex g_mem_mtx;
using memMap = std::map<packetHead*, packetHead*>;
static memMap s_memMap;

packetHead* allocPackDebug(udword udwSize)
{
	auto pRet = (packetHead*)(new char [sizeof(packetHead) + sizeof(netPacketHead) + udwSize]);
	pRet->pAsk = 0;
	pRet->loopId = c_emptyLocalServerId;
	pRet->sessionID = EmptySessionID;
	auto pN = P2NHead(pRet);
	pN->udwLength = udwSize;
	pN->uwTag = 0;
	{
		std::lock_guard<std::mutex> lock(g_mem_mtx);
		auto inRet = s_memMap.insert(std::make_pair(pRet, pRet));
		myAssert (inRet.second);
	}
	return pRet;
}

void	freePackDebug(packetHead* pack)
{
	if (pack->pAsk) {
		freePackDebug((packetHead*)(pack->pAsk));
		pack->pAsk = 0;
	}
	{
		std::lock_guard<std::mutex> lock(g_mem_mtx);
		auto inRet = s_memMap.erase(pack);
		myAssert(inRet);
	}

	delete [] ((char*)(pack));
}

void outMemLeakDebug (std::ostream& os)
{
	os<<" leak mem is :"<<std::endl;
	for (auto it = s_memMap.begin (); it != s_memMap.end (); it++) {
		os<<" leaf pack : "<<*(it->first)<<std::endl;
	}
}

packetHead* allocPackRelease (udword udwSize)
{
	auto pRet = (packetHead*)(new char [sizeof(packetHead) + sizeof(netPacketHead) + udwSize]);
	pRet->pAsk = 0;
	pRet->loopId = c_emptyLocalServerId;
	pRet->sessionID = EmptySessionID;
	auto pN = P2NHead(pRet);
	pN->udwLength = udwSize;
	pN->uwTag = 0;
	return pRet;
}

void	freePackRelease (packetHead* pack)
{
	if (pack->pAsk) {
		freePackRelease ((packetHead*)(pack->pAsk));
		pack->pAsk = 0;
	}
	delete [] ((char*)(pack));
}

void outMemLeakRelease(std::ostream& os)
{
}

int sPushPackToLoop (loopHandleType pThis, packetHead* pack)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	server* pS = nullptr;
	pS = rMgr.getServer (pThis); // pServerS[ubyDl];
	if (pS) {
		pS->pushPack (pack);
	} else {
		nRet = 1;
		rError ("can not find server handle = "<<pThis);
	}
	return nRet;
}

static void stopLoopS()
{
}

struct pSerializePackFunType3
{
	serializePackFunType f[3];
};

class pSerializePackFunType3Cmp 
{
public:
	bool operator () (const pSerializePackFunType3& a, const pSerializePackFunType3& b) const
	{
		return a.f[0] < b.f[0];
	}
};

using  msgSerFunSet = arraySet<pSerializePackFunType3, pSerializePackFunType3Cmp>;
static msgSerFunSet s_SerFunSet;

static pSerializePackFunType3* sGetNode (udword msgId)
{
	auto & rArr = s_SerFunSet;
	pSerializePackFunType3 temp;
	temp.f[0] = (serializePackFunType)msgId;
	auto pRet = rArr.GetNode (temp);
	return pRet;
}

int fromNetPack (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		// myAssert (p);
		// auto pN = P2NHead(p);
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[1];
			if (fun) {
				auto nR = fun (pN, pNew);
				if (nR) {
					nRet = 2;
				}
				if (pNew) {
					pNew->pAsk = 0;
				}
			}
		}
	} while (0);
	return nRet;
}

int toNetPack (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[2];
			auto nR = fun (pN, pNew);
			if (nR) {
				nRet = 2;
			}
			if (pNew) {
				pNew->pAsk = 0;
			}
		}
	} while (0);
	return nRet;
}
serverMgr::serverMgr()
{
	for (decltype (c_serverLevelNum) i = 0; i < c_serverLevelNum; i++) {
		  m_muServerPairS[i].second = 0;
	}
	m_netServerTmp = c_emptyLoopHandle;
	m_serverNum = 0;
	m_outNum = 1;
	m_createTcpServerFn = nullptr;
	m_delTcpServerFn = nullptr;
	m_packSendInfoTime = 5000;

	m_gropId = 0;
    m_delSendPackTime = 5000;
	m_canUpRouteServerNum = 0;
	m_canDownRouteServerNum = 0;
}

int  serverMgr:: pushPackToLoop (loopHandleType pThis, packetHead* pack)
{
	int  nRet = 0;
	do {
		server* pS = nullptr;
		pS = getServer (pThis);
		if (pS) {
			pS->pushPack (pack);
		} else {
			nRet = 1;
			rError ("can not find server handle = "<<pThis);
		}
	} while (0);
	return nRet;
}

udword  serverMgr::  delSendPackTime ()
{
    return m_delSendPackTime;
}
void  serverMgr::setDelSendPackTime (udword  va)
{
    m_delSendPackTime = va;
}
udword  serverMgr::  packSendInfoTime ()
{
    return m_packSendInfoTime;
}

void serverMgr:: setPackSendInfoTime (udword  va)
{
    m_packSendInfoTime = va;
}

serverMgr::~serverMgr()
{
}

loopHandleType	serverMgr::procId()
{
	return m_procId;
}

loopHandleType	serverMgr::gropId()
{
	return m_gropId;
}


void serverMgr::setProcId(loopHandleType proc)
{
	m_procId = proc;
}

void			serverMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

static NetTokenType   sNextToken(loopHandleType pThis)
{
	NetTokenType   nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		nRet = pTH->nextToken ();
	}
	return nRet;
}


static void  sPushToCallStack (loopHandleType pThis, const char* szTxt)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->pushToCallStack (szTxt);
	}
}

static void  sPopFromCallStack (loopHandleType pThis)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->popFromCallStack ();
	}
}

static void  sLogCallStack (loopHandleType pThis, int nL)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->logCallStack (nL);
	}
}

server*   serverMgr:: getServer(loopHandleType handle)
{
	server* pRet = nullptr;

	loopHandleType ubyLv,  onceLv, onceIndex ;
	getLvevlFromSerId (handle, ubyLv, onceLv, onceIndex);
	muServerPairS* pAS = muServerPairSPtr ();
	auto& rMu = pAS[ubyLv];
	if (onceLv < rMu.second) {
		auto& rS = rMu.first[onceLv];
		if (onceIndex < rS.second) {
			pRet = &rS.first[onceIndex];
		}
	}
	return pRet;
}

static loopHandleType      getCurServerHandle ()
{
	return server::s_loopHandleLocalTh;
}

int serverMgr::initFun (int cArg, char** argS)
{
	int nRet = 0;
	tSingleton <argConfig>::createSingleton ();
	auto& rConfig = tSingleton<argConfig>::single ();
	auto nR = getCurModelPath (m_homeDir);
	myAssert (0 == nR);
	auto bR = upDir (m_homeDir.get());
	myAssert (bR);
	do {
		nR = rConfig.procCmdArgS (cArg, argS);
		if (nR) {
			nRet = 1;
			break;
		}
		{
			auto pWorkDir = rConfig.homeDir ();
			myAssert (pWorkDir);
			std::string strFile = pWorkDir;
			auto frameConfig = rConfig.frameConfigFile ();
			strFile += "/config/";
			strFile += frameConfig;
			rConfig.loadConfig (strFile.c_str());
		}
		
		auto logLevel = rConfig.logLevel ();
		auto pWorkDir = rConfig.homeDir ();
		myAssert (pWorkDir);
		std::string strFile = pWorkDir;
		strFile += "/logs/";
		myMkdir (strFile.c_str());
		auto pLogFile = rConfig.logFile();
		strFile += pLogFile;
		initLogGlobal ();
		auto nInitLog = initLog ("appFrame", strFile.c_str(), logLevel);
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			break;
		}
		rInfo ("Loger init OK");

		nR = rConfig.procCmdArgS (cArg, argS);
		if (nR) {
			nRet = 1;
			mError("rConfig.procCmdArgS error nR = "<<nR);
			break;
		}
		pWorkDir = rConfig.homeDir ();
		auto allocDebug = rConfig.allocDebug ();
		if (allocDebug) {
			allocPack = allocPackDebug;
			freePack = freePackDebug;
			outMemLeak = outMemLeakDebug;
		} else {
			allocPack = allocPackRelease;
			freePack = freePackRelease;
			outMemLeak = outMemLeakRelease;
		}
		nR = rConfig.afterAllArgProc ();
		if (nR) {
			nRet = 2;
			mError("rConfig.afterAllArgProc error nR = "<<nR);
			break;
		}
		auto sr = rConfig.srand ();
		if (sr) {
			srand(time(NULL));
		}
		nR = init(cArg, argS);
		if (nRet) {
			mError("loopMgr init error nRet = "<<nRet);
			break;
		}
		auto dumpMsg = rConfig.dumpMsg ();
		if (dumpMsg) {
			rInfo ("dupmMsg end plese check");
			break;
		}
		loggerDrop ();
	} while (0);
	return nRet;
}

createTcpServerFT  serverMgr::  createTcpServerFn ()
{
    return m_createTcpServerFn;
}

void  serverMgr::setCreateTcpServerFn (createTcpServerFT  va)
{
    m_createTcpServerFn = va;
}

delTcpServerFT  serverMgr::delTcpServerFn ()
{
    return m_delTcpServerFn;
}

void  serverMgr::setDelTcpServerFn (delTcpServerFT  va)
{
    m_delTcpServerFn = va;
}

ubyte  serverMgr::upNum ()
{
    return m_outNum;
}

void serverMgr::setUpNum (ubyte  va)
{
    m_outNum = va;
}

void         lv0PushToCallStack (const char* szTxt)
{
	sPushToCallStack (server::s_loopHandleLocalTh, szTxt);
}

void         lv0PopFromCallStack ()
{
	sPopFromCallStack(server::s_loopHandleLocalTh);
}

void         lv0LogCallStack (int nL)
{
	sLogCallStack (server::s_loopHandleLocalTh, nL);
}

static int sRegMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun)
{
	int nRet = 0;
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->regMsg(uwMsgId, pFun);
	} while(0);
	return nRet;
}

static int sRemoveMsg(loopHandleType handle, uword uwMsgId)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->removeMsg(uwMsgId);
	} while(0);
	return nRet;
}

static int sAddComTimer(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	int nL = pThis;
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop(pThis);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}

static bool sDelNetPack (void* pUP)
{
	auto pArg = (std::pair<NetTokenType, server*>*) pUP; 
	auto pS = pArg->second;
	auto pISavePack = pS->getIPackSave ();
	auto pack = pISavePack->netTokenPackFind (pArg->first);
	if (pack) {
		mWarn ("pack delete by timer"<<*pack);
		pISavePack->netTokenPackErase (pArg->first);
		freePack (pack);
	}
	return false;
}
static int sSendPackToSomeSession(loopHandleType myServerId, netPacketHead* pN, uqword* pSessS, udword sessionNum)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	do {
		auto pLoop = rMgr.getLoop(myServerId);
		if (!pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->sendPackToSomeSession (pN, pSessS, sessionNum);
	} while (0);
	return nRet;
}

int midSendPackToLoopForChannelFun(packetHead* pack)
{
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; //tSingleton<serverMgr>::single ();
	pack->pAsk = 0;
	do {
		myAssert (pN->ubyDesServId == c_emptyLoopHandle);
		
		NSetUnRet(pN);
		
		auto pSendServer = rMgr.getOnceNetServer ();
		if (pSendServer) {
			pSendServer->pushPack (pack);
		} else {
			mError(" no net server pack is : "<<*pack);
			freePack (pack);
		}
	} while (0);
	return nRet;
}

int midSendPackToLoopFun(packetHead* pack)
{
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; // tSingleton<serverMgr>::single ();

	auto pS = rMgr.getLoop(pN->ubySrcServId);
	myAssert (pS);
	bool bIsRet = NIsRet(pN);
	auto bInOnceProc =  packInOnceProc(pack);

	do {
		myAssert(pN->ubyDesServId != c_emptyLoopHandle);
		if (pN->ubyDesServId == c_emptyLoopHandle) {
			mError ("ubyDesServId == c_emptyLoopHandle pack = "<<*pack);
			freePack (pack);
			break;
		}
		if (bInOnceProc) {
			rSerMgr.pushPackToLoop (pN->ubyDesServId, pack);
			break;
		}
		myAssert (!bIsRet);
		if (bIsRet) {
			if (EmptySessionID == pack->sessionID) {
				
				auto pSendServer = rMgr.getOnceNetServer ();
				if (!pSendServer) {
				// if (c_emptyLoopHandle == objSe) {
					mError(" can not find net server whith pack pack is : "<<*pack);
					freePack (pack);
				} else {
					pSendServer->pushPack (pack);
					// rSerMgr.pushPackToLoop (objSe, pack);
				}
			} else {
				auto pSess = pS->getSession (pack->sessionID);
				myAssert (pSess);
				if (pSess) {
					pSess->send (pack);
				} else {
					mError(" can not find sesssion whith pack pack is : "<<*pack);
					freePack (pack);
				}
			}
			break;
		}
		
		myAssert(EmptySessionID == pack->sessionID);

		bool  bNeetRet = false;

		auto& rMsgInfoMgr = rMgr.defMsgInfoMgr ();
		auto retMsgId = rMsgInfoMgr.getRetMsg (pN->uwMsgID);
		if (c_null_msgID != retMsgId) {
			auto pFun = pS->findMsg(retMsgId);
			if (pFun ) {
				bNeetRet = true;
			}
		}
		if (bNeetRet) {
			NSetNeetRet(pN);
		} else {
			NSetUnRet(pN);
		}
		if (bNeetRet) {
			pN->dwToKen = pS->nextToken ();
		}
		pack->pAsk = 0;
		
		auto pSendServer = rMgr.getOnceNetServer ();
		myAssert (pSendServer);
		if (!pSendServer) {
			mError(" no net server pack is : "<<*pack);
			freePack (pack);
			break;
		}

		// auto toNetPack = rMgr.toNetPack ();
		packetHead* pNew = nullptr;
		toNetPack (pN, pNew);
		
		if (bNeetRet) {
			iPackSave* pISave = pS->getIPackSave ();

			packetHead* sclonePack(packetHead* p);
			if (!pNew) {
				pNew =  sclonePack (pack);
			}

			pISave->netTokenPackInsert (pack);
			std::pair<NetTokenType, server*> pa;
			pa.first = pN->dwToKen;
			pa.second = pS;
			auto delTime = 61800;
			auto& rTimeMgr = pS->getTimerMgr ();
			rTimeMgr.addComTimer (delTime, sDelNetPack, &pa, sizeof (pa));
			pack = pNew;
		} else {
			if (pNew) {
				freePack (pack);
				pack = pNew;
			}
		}
		pack->sessionID = EmptySessionID;
		pack->loopId = c_emptyLoopHandle;
		// rSerMgr.pushPackToLoop (objSer, pack);
		pSendServer->pushPack (pack);
	} while (0);
	return nRet;
}

int midSendPackUpFun(packetHead* pack)
{
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; // tSingleton<serverMgr>::single ();
	auto& rConfig = tSingleton<argConfig>::single();
	auto pS = rMgr.getLoop(pN->ubySrcServId);
	myAssert (pS);
	myAssert (!NIsRet(pN));
	do {
		if (!pS) {
			mError ("ont find server pack = "<<*pack);
			freePack (pack);
			break;
		}
		
		pN->ubyDesServId = c_emptyLoopHandle;

		myAssert(EmptySessionID == pack->sessionID);

		bool  bNeetRet = false;
		auto& rMsgInfoMgr = rMgr.defMsgInfoMgr ();
		auto retMsgId = rMsgInfoMgr.getRetMsg (pN->uwMsgID);
		if (c_null_msgID != retMsgId) {
			auto pFun = pS->findMsg(retMsgId);
			if (pFun ) {
				bNeetRet = true;
			}
		}
		if (bNeetRet) {
			NSetNeetRet(pN);
		} else {
			NSetUnRet(pN);
		}
		if (bNeetRet) {
			pN->dwToKen = pS->nextToken ();
		}
		pack->pAsk = 0;
		
		auto nAppNetType = (appNetType)(rConfig.appNetType());
		if (appNetType_gate == nAppNetType) {
			nRet = pS->clonePackToOtherNetThread (pack, false);
			freePack (pack);
			break;
		}
		packetHead* pNew = nullptr;
		toNetPack (pN, pNew);
		if (bNeetRet) {
			iPackSave* pISave = pS->getIPackSave ();
			packetHead* sclonePack(packetHead* p);
			if (!pNew) {
				pNew =  sclonePack (pack);
			}
			pISave->netTokenPackInsert (pack);
			std::pair<NetTokenType, server*> pa;
			pa.first = pN->dwToKen;
			pa.second = pS;
			auto delTime = 61800;
			auto& rTimeMgr = pS->getTimerMgr ();
			rTimeMgr.addComTimer (delTime, sDelNetPack, &pa, sizeof (pa));
			pack = pNew;
		} else {
			if (pNew) {
				freePack (pack);
				pack = pNew;
			}
		}
		pack->sessionID = EmptySessionID;
		pack->loopId = c_emptyLoopHandle;
		auto iRoute = pS->route ();
		if (iRoute) {
			auto pUpSession = pS->defSession ();
			myAssert (pUpSession);
			if (!pUpSession) {
				mError ("pUpSession empty pack = "<<*pack);
				freePack (pack);
				break;
			}
			pUpSession->send (pack);
		} else {
			auto pNet = rMgr.getOnceNetServer ();
			myAssert (pNet);
			if (!pNet) {
				mError (" pNet empty pack = "<<*pack);
				freePack (pack);
				break;
			}
			pNet->pushPack(pack);
		}
		
	} while (0);
	return nRet;
}
static int    sRegRpc(msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer)
{
	auto& rMgr = tSingleton<serverMgr>::single().defMsgInfoMgr();
	return rMgr.regRpc (askId, retId, askDefProcSer, retDefProcSer);
}

serverIdType sGetDefProcServerId (msgIdType msgId)
{
	serverIdType nRet = c_emptyLoopHandle;
	auto& rMgr = tSingleton<serverMgr>::single().defMsgInfoMgr();
	auto nR = rMgr.getDefProcServerId (msgId);
	do {
		if (c_emptyLoopHandle == nR) {
			break;
		}
		loopHandleType level,  onceLv, onceIndex;
		getLvevlFromSerId (nR, level, onceLv, onceIndex);
		auto pA = tSingleton<serverMgr>::single().muServerPairSPtr ();
		auto& rMu = pA[level];
		if (onceLv >= rMu.second) {
			break;
		}
		auto& rOnce = rMu.first[onceLv];
		if (rOnce.second != 1) {
			break;
		}
		nRet = nR;
	} while (0);
	return nRet;
}

static int sRegRouteFun(loopHandleType myServerId, loopHandleType objServerId, SessionIDType sessionId,  udword onlyId)
{
	int nRet = 0;
	do {
		auto& rMgr = tSingleton<serverMgr>::single();
		auto pS = rMgr.getLoop(myServerId);
		myAssert (pS);
		if (!pS) {
			nRet = 1;
			mError ("can not find server id = "<<myServerId);
			break;
		}
		auto nR = pS->regRoute (objServerId, sessionId, onlyId);
		if (nR) {
			nRet = 2;
			mError ("RegRouteFun ret error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}

typedef void		(*freePackFT)(packetHead* pack);

int serverMgr::init(int nArgC, char** argS)
{
	return 1;
}

msgMgr& serverMgr::defMsgInfoMgr ()
{
	return m_defMsgInfoMgr;
}

server* serverMgr::getLoop(loopHandleType id)
{
	return getServer (id);
}

ForLogicFun&  serverMgr::getForLogicFun()
{
	return  m_forLogic;
}

int getMidDllPath (std::unique_ptr<char[]>& pathBuf);

int   serverMgr :: initNetServer ()
{
    int    nRet = 0;
    do {
		auto& rConfig = tSingleton<argConfig>::single ();
		auto midNetLibName = rConfig.netLib ();
		std::unique_ptr<char[]> binH;
		getMidDllPath (binH);
		std::string strPath = binH.get (); 
		strPath += midNetLibName;
		strPath += dllExtName ();
		nRet = initComTcpNet (strPath.c_str(), allocPack, freePack, logMsg);
		if (nRet) {
			mError ("initComTcpNet error nRet = "<<nRet<<" strPath = "
					<<strPath.c_str());
			break;
		}
    } while (0);
    return nRet;
}

uword serverMgr:: canRouteNum ()
{
    return m_canUpRouteServerNum  + m_canDownRouteServerNum; // m_upNum;
}

bool  serverMgr:: isRootApp ()
{
    bool   nRet = 0;
    do {
		nRet =  m_canDownRouteServerNum && !m_canUpRouteServerNum;
    } while (0);
    return nRet;
}

uword serverMgr:: canUpRouteServerNum ()
{
    return m_canUpRouteServerNum;
}

void serverMgr:: setCanUpRouteServerNum (uword v)
{
    m_canUpRouteServerNum = v;
}

uword serverMgr:: canDownRouteServerNum ()
{
    return m_canDownRouteServerNum;
}

void  serverMgr:: setCanDownRouteServerNum (uword v)
{
    m_canDownRouteServerNum = v;
}

void serverMgr:: logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnLogicOnConnect ();
	fu (fId, sessionId, userData);
}

void  serverMgr:: logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnLogicOnAccept ();
	fu (fId, sessionId, userData);
}

void  serverMgr:: onLoopBegin(ServerIDType fId)
{
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnOnLoopBegin ();
	fu (fId);
}

void  serverMgr:: onLoopEnd(ServerIDType fId)
{
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnOnLoopEnd ();
	fu (fId);

}

CModule&  serverMgr:: ModuleMgr ()
{
    return m_ModuleMgr;
}

serverMgr::muServerPairS* serverMgr:: muServerPairSPtr ()
{
	return m_muServerPairS;
}

serverIdType  serverMgr:: netServerTmp ()
{
    return m_netServerTmp;
}

void           serverMgr:: setNetServerTmp (serverIdType serverId)
{
    m_netServerTmp = serverId;
}

server*   serverMgr:: getNetServerS (uword& num)
{
    server*        nRet = nullptr;
	do {
		auto tmpId = netServerTmp ();
		if (c_emptyLoopHandle == tmpId) {
			break;
		}
		loopHandleType ubyLv,  onceLv, onceIndex ;
		getLvevlFromSerId (tmpId, ubyLv, onceLv, onceIndex);
		muServerPairS* pAS = muServerPairSPtr ();
		auto& rMu = pAS[ubyLv];
		if (onceLv >= rMu.second) {
			break;	
		}
		auto& rS = rMu.first[onceLv];
		if (rS.second) {
			nRet = rS.first.get ();
			num = rS.second;
		}
	} while (0);
    return nRet;
}

server*        serverMgr:: getOnceNetServer ()
{
    server*        nRet = nullptr;
    do {
		auto tmpId = netServerTmp ();
		if (c_emptyLoopHandle == tmpId) {
			break;
		}
		loopHandleType ubyLv,  onceLv, onceIndex ;
		getLvevlFromSerId (tmpId, ubyLv, onceLv, onceIndex);
		muServerPairS* pAS = muServerPairSPtr ();
		auto& rMu = pAS[ubyLv];
		if (onceLv >= rMu.second) {
			break;	
		}
		auto& rS = rMu.first[onceLv];
		if (rS.second) {
			nRet = rS.first.get ();
			auto num = rS.second;
			myAssert (num);
			nRet += rand()%num;
		}
    } while (0);
    return nRet;
}

int  serverMgr::runThNum (char* szBuf, int bufSize)
{
	int nRet = 0;
	const int c_MaxNum = 128;
	auto tempH = std::make_unique<loopHandleType []>(c_MaxNum);
	auto pH = tempH.get();
	{
		std::lock_guard<std::mutex> lock(m_mtxRunThNum);
		auto& rIdS = runThreadIdS ();
		for (auto it = rIdS.begin (); it != rIdS.end (); it++) {
			pH[nRet++] = *it;
		}
	}
	auto ss = std::make_unique<std::stringstream>();
	auto& rS = *ss;
	for (decltype (nRet) i = 0; i < nRet; i++) {
		rS<<pH[i]<<" ";
	}
	if (!nRet) {
		outMemLeak (rS);
	}
	strNCpy (szBuf, bufSize, rS.str().c_str());
    return nRet;
}

void   serverMgr:: incRunThNum (loopHandleType pThis)
{
	std::lock_guard<std::mutex> lock(m_mtxRunThNum);
	auto& rIdS = runThreadIdS ();
	rIdS.insert(pThis);
}

void   serverMgr:: subRunThNum (loopHandleType pThis)
{
	std::lock_guard<std::mutex> lock(m_mtxRunThNum);
	auto& rIdS = runThreadIdS ();
	rIdS.erase(pThis);
}

serverMgr::runThreadIdSet&  serverMgr:: runThreadIdS ()
{
    return m_runThreadIdS;
}

void  serverMgr:: setHomeDir (const char* v)
{
    strCpy (v, m_homeDir);
}
*/
