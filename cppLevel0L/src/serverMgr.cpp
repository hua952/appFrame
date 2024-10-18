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
	PUSH_FUN_CALL
	lv0LogCallStack (27);
	if (pack->pAsk) {
		freePackDebug((packetHead*)(pack->pAsk));
		pack->pAsk = 0;
	}
	// auto pN = P2NHead (pack);
	// rTrace (" will delete pack "<<*pack);
	{
		std::lock_guard<std::mutex> lock(g_mem_mtx);
		auto inRet = s_memMap.erase(pack);
	}

	delete [] ((char*)(pack));
	POP_FUN_CALL
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
/*
server*  serverMgr::getOutServer()
{
	auto n = rand () % m_outNum;
	return m_loopS[n].get(); // g_serverS [n];
}
*/
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

/*
serverIdType 	serverMgr::getServerNum()
{
	return g_ServerNum;
}
pserver* serverMgr::getServerS()
{
	return g_serverS.get();
}
PhyCallback&  serverMgr::getPhyCallback()
{
	return m_PhyCallback;
}

int serverMgr::procArgS(int nArgC, char** argS)
{
	for(int i = 1; i < nArgC; i++)
	{
		auto pC = argS[i];
		auto nL = strlen(pC);
		std::unique_ptr<char[]>	 name = std::make_unique<char[]>(nL + 1);
		strcpy(name.get(), pC);
		const int c_BuffNum = 3;
		char* buff[c_BuffNum];
		auto nR = strR(name.get(), '=', buff, c_BuffNum);
		if(2 == nR)
		{
			if(0 == strcmp(buff[0], "gropId")) {
				int nId = atoi (buff[1]);
				setGropId (nId);
			} else if(0 == strcmp(buff[0], "procId")) {
				int nId = atoi (buff[1]);
				setProcId(nId);
			} else if (0 == strcmp(buff[0], "netLib")) {
				auto nL = strlen (buff[1]);
				m_netLibName = std::make_unique <char[]>(nL + 1);
				auto p = m_netLibName.get();
				strNCpy (p, nL + 1, buff[1]);
			}
		}
	}
	return 0;
}
*/

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
		nR = rConfig.procCmdArgS (cArg, argS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
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

		nR = rConfig.procCmdArgS (cArg, argS);   /* 再次解析一遍命令行参数, 覆盖配置文件里的项,确保命令行参数的优先级最高  */
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

int midSendPackToLoopForChannelFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; //tSingleton<serverMgr>::single ();
	pack->pAsk = 0;
	do {
		myAssert (pN->ubyDesServId == c_emptyLoopHandle);
		/*
		bool bRand = true;
		if (pN->ubyDesServId != c_emptyLoopHandle) {
			bRand = !packInOnceProc(pack);
		}
		*/
		
		NSetUnRet(pN);
		/*
		uword netNum = 0;
		server* pA = rMgr.getNetServerS (netNum);
		*/
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

int midSendPackToLoopFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
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
		myAssert (!bIsRet);    /*   ret包沿ask包路径被动返回,理论上不应调用本函数, 本函数处理主动发送    */
		if (bIsRet) {
			if (EmptySessionID == pack->sessionID) {
				/*
				auto objSe = rMgr.getOnceUpServer ();
				myAssert (c_emptyLoopHandle != objSe);
				*/
				auto pSendServer = rMgr.getOnceNetServer ();
				if (!pSendServer) {
				// if (c_emptyLoopHandle == objSe) {       /*     由于本函数是处理首站发出, 不可能会出现这种情况        */
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
		
		/*  以下处理ask类型消息的发送    */
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
		/*	
		auto objSer = rMgr.getOnceUpServer ();
		myAssert (c_emptyLoopHandle != objSer);
		*/
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
				pNew =  sclonePack (pack);  /*  由于要保存原包,克隆一份 */
			}

			pISave->netTokenPackInsert (pack);  /* 保存pack 因为该函数是通过网络发送的第一站,故在此保存   */
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

int midSendPackUpFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
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

		/*  以下处理ask类型消息的发送    */
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
				pNew =  sclonePack (pack);  /*  由于要保存原包,克隆一份 */
			}
			pISave->netTokenPackInsert (pack);  /* 保存pack 因为该函数是通过网络发送的第一站,故在此保存   */
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
/*
serializePackFunType  serverMgr:: fromNetPack ()
{
    return m_fromNetPack;
}
*/

int serverMgr::init(int nArgC, char** argS)
{
	return 1;
	/*
	auto& forLogic = getForLogicFun();
	forLogic.fnCreateLoop = nullptr; // sCreateServer;
	forLogic.fnAllocPack = allocPack; // sAllocPack; // info.fnAllocPack;
	forLogic.fnFreePack = freePack; // sFreePack; //  info.fnFreePack;
	forLogic.fnRegMsg = sRegMsg;

	forLogic.fnSendPackUp =  midSendPackUpFun;
	forLogic.fnSendPackToLoop =  midSendPackToLoopFun;
	forLogic.fnSendPackToLoopForChannel = midSendPackToLoopForChannelFun;
	forLogic.fnSendPackToSomeSession = sSendPackToSomeSession;
	forLogic.fnLogMsg = logMsg; // info.fnLogMsg;
	forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
	forLogic.fnNextToken = sNextToken; //info.fnNextToken;
	forLogic.fnRegRpc = nullptr; // sRegRpc;
	forLogic.fnGetDefProcServerId = nullptr; // sGetDefProcServerId;
	forLogic.fnRegRoute = sRegRouteFun;
	forLogic.fromNetPack = nullptr;
	forLogic.toNetPack = nullptr;

	int nR = 0;
	int nRet = 0;
	auto& rConfig = tSingleton<argConfig>::single ();
	do
	{
		auto workDir = rConfig.homeDir (); // rConfig.workDir();
		std::string strPath;
		if (workDir) {
			strPath = workDir;
		} else {
			std::unique_ptr<char[]> t;
			getCurModelPath (t);
			strPath = t.get();
		}
		auto serializePackLib = rConfig.serializePackLib ();
		strPath += "/bin/";
		strPath += serializePackLib;
		strPath += dllExtName();
		auto hdll = loadDll (strPath.c_str());
		if (hdll) {
			typedef int  (*getSerializeFunSFT) (serializePackFunType* pFunS, int nNum, ForLogicFun* pForLogic);
			auto funGetSerializeFunS = (getSerializeFunSFT)(getFun (hdll, "getSerializeFunS"));
			if (funGetSerializeFunS) {

				const auto cNum = 512;
				const auto c_bufNUm = cNum * 3;
				auto pBuf = std::make_unique<serializePackFunType[]>(c_bufNUm);
				auto funNum = funGetSerializeFunS(pBuf.get(), c_bufNUm, &forLogic);
				myAssert (funNum < cNum);
				auto serNum = funNum / 3;
				if (serNum) {
					s_SerFunSet.init ((pSerializePackFunType3*)(pBuf.get()), serNum);
				}
				forLogic.fromNetPack = fromNetPack;
				forLogic.toNetPack = toNetPack;
			}

			typedef int  (*getMsgPairSFT) (uword* pFunS, int nNum);
			auto fnGetMsgPairS = (getMsgPairSFT)(getFun (hdll, "getMsgPairS"));
			if (fnGetMsgPairS) {
				const auto cNum = 512;
				const auto c_bufNum = cNum * 2;
				auto pBuf = std::make_unique<uword[]>(c_bufNum);
				auto pB = pBuf.get();
				auto rpcNum = fnGetMsgPairS (pB, c_bufNum);
				myAssert (rpcNum < cNum);
				auto reNum = rpcNum / 2;
				for (decltype (reNum) i = 0; i < reNum; i++) {
					auto j = i * 2;
					sRegRpc (pB[j], pB[j + 1], c_emptyLoopHandle, c_emptyLoopHandle);
				}
			}
		}
		auto midNetLibName = rConfig.netLib (); // rArgS.midNetLibName ();
		if (midNetLibName ) {
			nR = initNetServer ();
			if (nR) {
				mError("initNetServer  return error nR = "<<nR);
				nRet = 2;
				break;
			}
		}

		auto modelNum = rConfig.modelNum ();
		auto pAllMod = rConfig.allModelS ();
		auto nAppNetType = (appNetType)(rConfig.appNetType());
		auto* pSerA = muServerPairSPtr ();
		for (decltype (modelNum) i = 0; i < modelNum; i++) {
			auto& rMod = pAllMod[i];
			for (decltype (rMod.serverTemNum) j = 0; j < rMod.serverTemNum; j++) {
				auto& rS = rMod.serverS[j];
				loopHandleType ubyLv,  onceLv, onceIndex ;
				getLvevlFromSerId (rS.serverTmpId, ubyLv, onceLv, onceIndex);
				auto&  rMu = pSerA[ubyLv];
				rMu.second++;
			}
		}
		for (decltype (c_serverLevelNum) i = 0; i < c_serverLevelNum; i++) {
			auto& rMu = pSerA[i];
			if (rMu.second) {
				rMu.first = std::make_unique<serverPair[]>(rMu.second);
			}
		}
		for (decltype (modelNum) i = 0; i < modelNum; i++) {
			auto& rMod = pAllMod[i];
			for (decltype (rMod.serverTemNum) j = 0; j < rMod.serverTemNum; j++) {
				auto& rS = rMod.serverS[j];
				loopHandleType ubyLv,  onceLv, onceIndex ;
				getLvevlFromSerId (rS.serverTmpId, ubyLv, onceLv, onceIndex);
				auto& rMu = pSerA[ubyLv];
				auto& rPa = rMu.first[onceLv];
				rPa.second = rS.openNum;
				rPa.first = std::make_unique<server[]>(rPa.second);
				serverNode node;
				node.sleepSetp = 10;
				node.fpsSetp = 40000;
				node.autoRun = rS.autoRun;
				node.route = rS.route;
				node.connectorNum = 0;
				node.listenerNum = 0;
				serverEndPointInfo	* pEndpoint = &node.connectEndpoint[0];
				if (node.route) {
					if (nAppNetType == appNetType_gate) {
						node.listenerNum = 1;
						pEndpoint = &node.listenEndpoint[0];
					} else {
						node.connectorNum = 1;
						if (nAppNetType == appNetType_client) {
							myAssert(1 == rPa.second);
						} 
					}
					strNCpy (pEndpoint->ip, sizeof(pEndpoint->ip), rConfig.ip());
					auto curNet = netServerTmp ();
					myAssert (curNet == c_emptyLoopHandle);
					setNetServerTmp (rS.serverTmpId);
				}
				
				for (decltype (rPa.second) k = 0; k < rPa.second; k++) {
					auto& rServer = rPa.first[k];
					auto serId = rS.serverTmpId + k;
					node.handle = serId;
					rServer.setAutoRun(node.autoRun);
					if (node.route) {
						if (nAppNetType == appNetType_client) {
							auto netNum = rConfig.netNum();
							myAssert (netNum);
							pEndpoint->port = rConfig.startPort() + rand()%netNum;
						} else {
							pEndpoint->port = rConfig.startPort() + k;
							auto netNum = rConfig.netNum();
							myAssert (netNum == rPa.second);
						}
						pEndpoint->userData = k;
					}
					rServer.initMid ("", serId, &node);
				}
			}
		}
		auto szModelMgrName = rConfig.modelMgrName ();
		if (!szModelMgrName ) {
			break;
		}
		auto& rMM = ModuleMgr ();
		rMM.init (szModelMgrName);
		nR = rMM.load (nArgC, argS, &forLogic);
		if (nR) {
			mError("rMM.load error nR = "<<nR);
			nRet = 3;
			break;
		}
		for (int i = 0; i < c_serverLevelNum; i++) {
			auto& rMu = pSerA[i];
			for (decltype (rMu.second) j = 0; j < rMu.second; j++) {
				auto& rPa = rMu.first[j];
				for (decltype (rPa.second) k = 0; k < rPa.second; k++) {
					auto& rServer = rPa.first[k];
					auto autoRun = rServer.autoRun ();
					if (autoRun) {
						rServer.start();
						rServer.detach();
					}
				}
			}
		}
	} while(0);
	return nRet;
	*/
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
	/*
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnConnect ();
			fu (fId, sessionId, userData);
		}
    } while (0);
	*/
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnLogicOnConnect ();
	fu (fId, sessionId, userData);
}

void  serverMgr:: logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	/*
	do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnAccept ();
			fu (fId, sessionId, userData);
		}
    } while (0);
	*/
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnLogicOnAccept ();
	fu (fId, sessionId, userData);
}

void  serverMgr:: onLoopBegin(ServerIDType fId)
{
	/*
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopBegin ();
			fu (fId);
		}
    } while (0);
	*/
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnOnLoopBegin ();
	fu (fId);
}

void  serverMgr:: onLoopEnd(ServerIDType fId)
{
	/*
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopEnd ();
			fu (fId);
		}
    } while (0);
	*/
	auto& rM = m_ModuleMgr;
	auto fu = rM.fnOnLoopEnd ();
	fu (fId);

}
/*
uword  serverMgr:: serverNum ()
{
    return m_serverNum;
}

void  serverMgr:: setServerNum (uword v)
{
    m_serverNum = v;
}
*/
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

