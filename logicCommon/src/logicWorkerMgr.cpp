#include "logicWorkerMgr.h"
#include "logicWorker.h"
#include "strFun.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "modelLoder.h"
#include "comTcpNet.h"
#include "gLog.h"

bool logicWorkerMgr::pSerializePackFunType3Cmp::operator () (const logicWorkerMgr::pSerializePackFunType3& a, const logicWorkerMgr::pSerializePackFunType3& b) const
{
	return a.f[0] < b.f[0];
}

logicWorkerMgr::pSerializePackFunType3* logicWorkerMgr::sGetNode (udword msgId)
{
	auto & rArr = s_SerFunSet;
	pSerializePackFunType3 temp;
	temp.f[0] = (serializePackFunType)msgId;
	auto pRet = rArr.GetNode (temp);
	return pRet;
}

logicWorkerMgr:: logicWorkerMgr ()
{
}

logicWorkerMgr:: ~logicWorkerMgr ()
{
}

int logicWorkerMgr::fromNetPack (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[1];
			if (fun) {
				auto nR = fun (pN, pNew);
				if (nR) {
					nRet = 2;
				}
				if (pNew) {
					pNew->packArg = 0;
				}
			}
		}
	} while (0);
	return nRet;
}

int logicWorkerMgr::toNetPack (netPacketHead* pN, pPacketHead& pNew)
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
				pNew->packArg = 0;
			}
		}
	} while (0);
	return nRet;
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
		int workDirLen = 0;
		if (pWorkDir) {
			workDirLen = (int)(strlen(pWorkDir));
		}
		if (!workDirLen) {
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

		pWorkDir = rConfig.homeDir ();
		workDirLen = 0;
		if (pWorkDir) {
			workDirLen = (int)(strlen(pWorkDir));
		}
		if (!workDirLen) {
			std::unique_ptr<char[]>	homeDirPtr;
			auto nR = getCurModelPath (homeDirPtr);
			myAssert (0 == nR);
			auto bR = upDir (homeDirPtr.get());
			myAssert (bR);
			rConfig.setHomeDir(homeDirPtr.get());
			pWorkDir = rConfig.homeDir ();
		}
		std::string strPath = pWorkDir;
		
		auto serializePackLib = rConfig.serializePackLib ();
		strPath += "/bin/";
		std::string binHome = strPath;
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
				auto funNum = funGetSerializeFunS(pBuf.get(), c_bufNUm, pForLogic);
				myAssert (funNum < cNum);
				auto serNum = funNum / 3;
				if (serNum) {
					s_SerFunSet.init ((pSerializePackFunType3*)(pBuf.get()), serNum);
				}
			}
		} else {
			gWarn(" can not load "<<strPath);
		}
		auto midNetLibName = rConfig.netLib (); // rArgS.midNetLibName ();
		if (midNetLibName ) {
			std::string strNet;
			strNet += midNetLibName;
			strNet += dllExtName();
			auto frameHome = rConfig.frameHome ();
			if (frameHome) {
				std::string temp = frameHome;
				strNet = temp + "/bin/" + strNet;
			}
			nR = initComTcpNet (strNet.c_str(), pForLogic->fnAllocPack, pForLogic->fnFreePack, pForLogic->fnLogMsg);
			if (nR) {
				gError ("initComTcpNet error nRet = "<<nRet<<" strPath = "
					<<strNet.c_str());
				nRet = 5;
				break;
			}
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
				m_allServers [curIncex]->recPacketProcFun (pForLogic);
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
