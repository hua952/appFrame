#include "workServerMgr.h"
#include "strFun.h"
#include "mLog.h"
#include "workServer.h"
#include "tSingleton.h"
#include "cTimer.h"
#include "argConfig.h"
#include "modelLoder.h"

packetHead* allocPackDebug(udword udwSize);
void	freePackDebug(packetHead* pack);
void outMemLeakDebug (std::ostream& os);

packetHead* allocPackRelease (udword udwSize);
void	freePackRelease (packetHead* pack);
void outMemLeakRelease(std::ostream& os);

workServerMgr:: workServerMgr ()
{
}

workServerMgr:: ~workServerMgr ()
{
}

static int sRegMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun)
{
	int nRet = 0;
	do 
	{
		// nRet = pLoop->regMsg(uwMsgId, pFun);
	} while(0);
	return nRet;
}

static int sAddComTimer(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	int nL = pThis;
	int nRet = 0;
	auto& rMgr = tSingleton<workServerMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}

static NetTokenType   sNextToken(loopHandleType pThis)
{
	NetTokenType   nRet = 0;
	auto& rMgr = tSingleton<workServerMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		nRet = pTH->nextToken ();
	}
	return nRet;
}

static int    sRegRpc(msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer)
{
	auto& rMgr = tSingleton<serverMgr>::single().defMsgInfoMgr();
	return rMgr.regRpc (askId, retId, askDefProcSer, retDefProcSer);
}

static int sPushPackToServer(loopHandleType desServerId, packetHead* pack)
{
	int nRet = 0;
	do 
	{
		auto& rMgr = tSingleton<workServerMgr>::single();
		auto pS = rMgr.getServer(desServerId);
		if (!pS) {
			nRet = 1;
			break;
		}
		nRet = pS->pushPack (pack) ? 0 : 1;
	} while(0);
	return nRet;
}

int  workServerMgr:: initWorkServerMgr (int cArg, char** argS, int cDefArg, char** defArgS)
{
	int  nRet = 0;
	tSingleton <argConfig>::createSingleton ();
	auto& rConfig = tSingleton<argConfig>::single ();
	auto nR = getCurModelPath (m_homeDir);
	myAssert (0 == nR);
	auto bR = upDir (m_homeDir.get());
	myAssert (bR);
	do {
		nR = rConfig.procCmdArgS (cDefArg, defArgS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
		if (nR) {
			nRet = 1;
			break;
		}
		nR = rConfig.procCmdArgS (cArg, argS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
		if (nR) {
			nRet = 2;
			break;
		}

		auto pWorkDir = homeDir ();// rConfig.workDir ();
		myAssert (pWorkDir);
		std::string frameConfigFile = pWorkDir;
		auto frameConfig = rConfig.frameConfigFile ();
		frameConfigFile += "/config/";
		frameConfigFile += frameConfig;
		rConfig.loadConfig (frameConfigFile.c_str());

		auto logLevel = rConfig.logLevel ();
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
		auto& forLogic = getForLogicFun();
		forLogic.fnCreateLoop = nullptr; // sCreateServer;
		forLogic.fnAllocPack = allocPack; // sAllocPack; // info.fnAllocPack;
		forLogic.fnFreePack = freePack; // sFreePack; //  info.fnFreePack;
		forLogic.fnRegMsg = sRegMsg;

		forLogic.fnSendPackUp =  nullptr;
		forLogic.fnSendPackToLoop =  nullptr;
		forLogic.fnSendPackToLoopForChannel = nullptr;
		forLogic.fnSendPackToSomeSession = nullptr;
		forLogic.fnLogMsg = logMsg; // info.fnLogMsg;
		forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
		forLogic.fnNextToken = sNextToken; //info.fnNextToken;
		forLogic.fnRegRpc = nullptr; // sRegRpc;
		forLogic.fnGetDefProcServerId = nullptr; // sGetDefProcServerId;
		forLogic.fnRegRoute = nullptr; // sRegRouteFun;
		forLogic.fromNetPack = nullptr;
		forLogic.toNetPack = nullptr;
		forLogic.fnPushPackToServer = sPushPackToServer;

		std::string strPath = workDir();
		auto modelName = rConfig.modelName();
		strPath += "/bin/";
		strPath += modelName;
		strPath += dllExtName();
		auto hdll = loadDll (strPath.c_str());
		do {
			if (!hdll) {
				mWarn ("loadDll error szName = "<<strPath.c_str());
				nRet = 3;
				break;
			}
			auto funOnLoad = (afterLoadFunT)(getFun (hdll, "afterLoad"));
			if(!funOnLoad) {
				mWarn ("funOnLoad empty error is");
				nRet = 4;
				break;
			}
			m_fnAfterLoad = funOnLoad;
			
			m_onUnloadFun = (beforeUnloadFunT)(getFun(hdll, "beforeUnload"));
			if (!m_onUnloadFun) {
				mWarn ("fun beforeUnload empty error is");
				nRet = 5;
				break;
			}
			m_fnOnLoopBegin = (onLoopBeginFT)(getFun(hdll, "onLoopBegin"));
			if (!m_fnOnLoopBegin) {
				mWarn ("fun onLoopBegin empty error is");
				nRet = 6;
				break;
			}

			m_fnRecPacket= (onRecPacketFT)(getFun(hdll, "onRecPacket"));
			if (!m_fnRecPacket) {
				mWarn ("fun onRecPacket empty error is");
				nRet = 7;
				break;
			}
			m_fnOnFrameLogic = (onFrameLagicFT)(getFun(hdll, "onFrameLogic"));
			if (!m_fnOnFrameLogic) {
				mWarn ("fun onLoopBegin empty error is");
				nRet = 8;
				break;
			}
			m_fnOnLoopEnd = (onLoopEndFT)(getFun(hdll, "onLoopEnd"));
			if (!m_fnOnLoopEnd) {
				mWarn ("fun fnOnFrameLogic empty error is");
				nRet = 9;
				break;
			}
			m_handle = hdll;
		} while (0);
		if (nRet) {
			mError("loopMgr init error nRet = "<<nRet);
			break;
		}
		auto serverGroupNum  = rConfig.serverGroupNum ();
		auto serverGroups = rConfig.serverGroups ();
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			m_allServerNum += serverGroups[i].runNum;
		}
		m_allServers = std::make_unique<workServer[]>(m_allServerNum);
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			auto& rInfo = serverGroups[i];
			for (decltype (rInfo.runNum) j = 0; j < rInfo.runNum; j++) {
				auto serverId = rInfo.beginId + j;
				auto& rServer = m_allServers[serverId];
				rServer.setServerId ((ubyte)(serverId));	
				rServer.setSleepSetp (rInfo.sleepSetp);
				rServer.initWorkServer ();
			}
		}
		if (m_fnAfterLoad ) {
			auto nR = m_fnAfterLoad (cArg, argS, &forLogic, 0, nullptr);
			if (nR) {
				mError ("funOnLoad ret error nR = "<<nR);
			}
		}
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			auto& rInfo = serverGroups[i];
			for (decltype (rInfo.runNum) j = 0; j < rInfo.runNum; j++) {
				auto& rServer = m_allServers[rInfo.beginId + i];
				if (rInfo.autoRun) {
					rServer.start();
					rServer.detach();
				}
			}
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

void  workServerMgr:: afterAllLoopEndBeforeExitApp  ()
{
    do {
		if (m_handle ) {
			if (m_onUnloadFun) {
				m_onUnloadFun ();
			}
			unloadDll (m_handle);
			m_handle = nullptr;
		}
    } while (0);
}

const char*   workServerMgr:: workDir ()
{
	auto& rConfig = tSingleton<argConfig>::single ();
	auto workDir = rConfig.workDir ();
	do {
		if (workDir) {
			auto nL = strlen (workDir);
			if (nL) {
				break;
			}
		}
		workDir = homeDir();
	} while (0);
	return workDir;
}

const char*  workServerMgr:: homeDir ()
{
    return m_homeDir.get ();
}

void  workServerMgr:: setHomeDir (const char* v)
{
    strCpy (v, m_homeDir);
}

ForLogicFun&  workServerMgr::getForLogicFun()
{
	return  m_forLogic;
}

workServer*   workServerMgr:: getServer(loopHandleType id)
{
    workServer*   nRet = nullptr;
    do {
		auto serI = (ubyte)(id);
		if (serI < m_allServerNum) {
			nRet = &m_allServers[serI];
		}
    } while (0);
    return nRet;
}

int  workServerMgr::runThNum (char* szBuf, int bufSize)
{
	int nRet = 0;
	const int c_MaxNum = 128;
	auto tempH = std::make_unique<loopHandleType []>(c_MaxNum);
	auto pH = tempH.get();
	{
		std::lock_guard<std::mutex> lock(m_mtxRunThNum);
		auto& rIdS = runThreadIdS ();
		for (auto it = rIdS.begin (); it != rIdS.end () && nRet < c_MaxNum; it++) {
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

void   workServerMgr:: incRunThNum (loopHandleType pThis)
{
	std::lock_guard<std::mutex> lock(m_mtxRunThNum);
	auto& rIdS = runThreadIdS ();
	rIdS.insert(pThis);
}

void   workServerMgr:: subRunThNum (loopHandleType pThis)
{
	std::lock_guard<std::mutex> lock(m_mtxRunThNum);
	auto& rIdS = runThreadIdS ();
	rIdS.erase(pThis);
}

serverMgr::runThreadIdSet&  workServerMgr:: runThreadIdS ()
{
    return m_runThreadIdS;
}

onRecPacketFT      workServerMgr:: onRecPacketFun ()
{
	return m_fnRecPacket;
}

