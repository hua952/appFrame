#include "logicWorkerMgr.h"
#include "logicWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "myAssert.h"

logicWorkerMgr:: logicWorkerMgr ()
{
}

logicWorkerMgr:: ~logicWorkerMgr ()
{
}

int  logicWorkerMgr:: initLogicWorkerMgr (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS)
{
    int  nRet = 0;
    do {
		m_pForLogicFun = pForLogic;
		tSingleton <logicFrameConfig>::createSingleton ();
		auto& rConfig = tSingleton<logicFrameConfig>::single ();
		int nR = rConfig.procCmdArgS (cDefArg, defArgS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
		if (nR) {
			nRet = 1;
			break;
		}
		nR = rConfig.procCmdArgS (cArg, argS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
		if (nR) {
			nRet = 2;
			break;
		}

		auto pWorkDir = rConfig.homeDir (); // rConfig.frameConfigFile();// rConfig.homeDir ();
		if (!pWorkDir) {
			std::unique_ptr<char[]>	homeDirPtr;
			auto nR = getCurModelPath (homeDirPtr);
			myAssert (0 == nR);
			auto bR = upDir (homeDirPtr.get());
			myAssert (bR);
			rConfig.setHomeDir(homeDirPtr.get());
			pWorkDir = rConfig.homeDir ();
		}
		std::string frameConfigFile = pWorkDir;
		auto frameConfig = rConfig.frameConfigFile ();
		frameConfigFile += "/config/";
		frameConfigFile += frameConfig;
		rConfig.loadConfig (frameConfigFile.c_str());
		nR = rConfig.procCmdArgS (cArg, argS);   /* 下一步用到参数里的文件名,所以要先解析一遍命令行参数  */
		if (nR) {
			nRet = 3;
			break;
		}
		nR = rConfig.afterAllArgProc ();
		if (nR) {
			nRet = 4;
			break;
		}
		auto serverGroupNum = rConfig.serverGroupNum ();
		auto serverGroups = rConfig.serverGroups ();
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			m_allServerNum += serverGroups[i].runNum;
		}

		m_allServers = std::make_unique<logicWorker*[]>(m_allServerNum);

		nR = initLogicGen (cArg, argS, pForLogic, cDefArg, defArgS);
		if (nR) {
			nRet = 5;
			break;
		}

		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			for (decltype (serverGroups[i].runNum) j = 0; j < serverGroups[i].runNum; j++) {
				auto curIncex = (ubyte)(serverGroups[i].beginId + j);
				m_allServers [curIncex]->setServerId (curIncex);
				m_allServers [curIncex]->setServerGroup ((ubyte)i);
				m_allServers [curIncex]->onWorkerInitGen (pForLogic);
			}
		}
    } while (0);
    return nRet;
}

int  logicWorkerMgr:: initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS)
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int  logicWorkerMgr:: processOncePack(ubyte serverIndex, packetHead* pPack)
{
    int  nRet = procPacketFunRetType_del;
    do {
		if (serverIndex >= m_allServerNum) {
			break;
		}
		auto pS = m_allServers[serverIndex];
		myAssert (pS);
		nRet = pS->processOncePack(pPack);
    } while (0);
    return nRet;
}

ForLogicFun*    logicWorkerMgr:: forLogicFun ()
{
    return m_pForLogicFun;
}

uword  logicWorkerMgr:: allServerNum ()
{
    return m_allServerNum;
}

void  logicWorkerMgr:: setAllServerNum (uword v)
{
    m_allServerNum = v;
}

logicWorker**  logicWorkerMgr:: allServers ()
{
    return m_allServers.get();
}

logicWorker*  logicWorkerMgr:: findServer (ubyte serverIndex)
{
    logicWorker*  nRet = nullptr;
	if (serverIndex < m_allServerNum) {
		nRet = m_allServers[serverIndex];
	}
    return nRet;
}

bool  logicWorkerMgr:: findDefProc (uword msgId, ubyte& appGroup, ubyte& serverGroup)
{
	bool  nRet = false;
	auto pF = m_defProcMap.find (msgId);
	if (pF) {
		serverGroup = (ubyte)(*pF);
		appGroup = (ubyte)((*pF)>>8);
		nRet = true;
	}
    return nRet;
}

logicWorkerMgr* logicWorkerMgr::s_mgr = nullptr;

logicWorkerMgr& logicWorkerMgr::getMgr()
{
	return *s_mgr;
}
