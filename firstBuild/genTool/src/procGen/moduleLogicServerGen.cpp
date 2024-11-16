#include "moduleLogicServerGen.h"
#include "strFun.h"
#include "moduleGen.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/serverFile.h"
#include "fromFileData/msgGroupFile.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/rpcFile.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgPmpFile.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/toolServerEndPointInfoMgr.h"
#include "xmlGlobalLoad.h"
#include "tSingleton.h"
#include "rLog.h"
#include "tSingleton.h"
#include <fstream>
#include <string>
#include <map>
#include <set>
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ostream.h>
#include "configMgr.h"

static bool sIsPathExit  (const char* szFile)
{
	bool bRet = tSingleton<configMgr>::single().reProc ();
	if (bRet) {
		bRet = false;
	} else {
		bRet = isPathExit (szFile);
	}
	return bRet;
}

moduleLogicServerGen:: moduleLogicServerGen ()
{
}

moduleLogicServerGen:: ~moduleLogicServerGen ()
{
}

int   moduleLogicServerGen:: startGen (appFile& rApp)
{
    int   nRet = 0;
	do {
		// auto& rData = rMod.moduleData ();
		std::string modName = rApp.appName (); // rData.moduleName ();
		modName += "M";
		int nR = 0;
		// nR = genH (rMod);
		nR = genWorkerMgrH(rApp);
		if (nR) {
			rError ("moduleLogicServerGen write h file error modName = "<<modName<<" nR = "<<nR);
			nRet = 1;
			break;
		}
		// nR = genCpp (rMod);
		nR = genWorkerMgrCpp (rApp);
		if (nR) {
			rError ("genWorkerMgrCpp write cpp file error modName = "<<modName<<" nR = "<<nR);
			nRet = 2;
			break;
		}

		auto& rSS = rApp.orderS ();
		for (auto it = rSS.begin (); rSS.end () != it; ++it) {
			auto pName = it->get()->serverName ();
			std::string strRoot = rApp.srcPath ();
			strRoot += "/";
			strRoot += pName;
			myMkdir (strRoot.c_str ());
			auto gen = strRoot;
			gen += "/gen";
			myMkdir (gen.c_str ());
			auto proc = strRoot;
			proc += "/proc";
			myMkdir (proc.c_str ());
			
			nR = genWorkerH (rApp, pName);
			if (nR) {
				nRet = 5;
				break;
			}
			nR = genWorkerCpp (rApp, pName);
			if (nR) {
				nRet = 6;
				break;
			}
			nR = genOnLoopBegin (rApp, pName);
			if (nR) {
				nRet = 7;
				break;
			}
			nR = genOnLoopEnd(rApp, pName);
			if (nR) {
				nRet = 8;
				break;
			}
			nR = genOnLoopFrame(rApp, pName);
			if (nR) {
				nRet = 9;
				break;
			}

			nR = genOnCreateChannelRet (rApp, pName);
			if (nR) {
				nRet = 9;
				break;
			}

			nR = genOnWorkerInitCpp (rApp, pName);
			if (nR) {
				nRet = 9;
				break;
			}
			nR = genPackFun (rApp, pName);
			if (nR) {
				nRet = 4;
				break;
			}
		}
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genOnWorkerInitCpp (appFile& rApp, const char* serverName)
{
    int   nRet = 0;
    do {
		std::string strRoot = rApp.srcPath ();
		strRoot += "/";
		strRoot += serverName;
		auto proc = strRoot;
		proc += "/proc/onWorkerInit.cpp";
		if (isPathExit (proc.c_str())) {
			nRet = 0;
			break;
		}
		std::ofstream os (proc.c_str ());
		if (!os) {
			rError ("open file : "<<proc.c_str ());
			nRet = 1;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onWorkerInit(ForLogicFun* pForLogic)
{{
	int nRet = 0;
	do {{
	}} while (0);
	return nRet;
}}

)", fmt::arg("serverName", serverName));

    } while (0);
    return nRet;
}

int   moduleLogicServerGen:: genOnLoopBegin (appFile& rApp, const char* serverName)
{
    int   nRet = 0;
    do {
		std::string strRoot = rApp.srcPath ();
		strRoot += "/";
		strRoot += serverName;
		auto proc = strRoot;
		proc += "/proc/onLoopBegin.cpp";
		if (isPathExit (proc.c_str())) {
			nRet = 0;
			break;
		}
		std::ofstream os (proc.c_str ());
		if (!os) {
			rError ("open file : "<<proc.c_str ());
			nRet = 1;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onLoopBegin()
{{
	int nRet = 0;
	do {{
	}} while (0);
	return nRet;
}}

)", fmt::arg("serverName", serverName));


    } while (0);
    return nRet;
}

int   moduleLogicServerGen:: genOnCreateChannelRet (appFile& rApp, const char* serverName)
{
    int   nRet = 0;
    do {
		std::string strRoot = rApp.srcPath ();
		strRoot += "/";
		strRoot += serverName;
		auto proc = strRoot;
		proc += "/proc/OnCreateChannelRet.cpp";
		if (isPathExit (proc.c_str())) {
			nRet = 0;
			break;
		}
		std::ofstream os (proc.c_str ());
		if (!os) {
			rError ("open file : "<<proc.c_str ());
			nRet = 1;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onCreateChannelRet(const channelKey& rKey, udword result)
{{
	int nRet = 0;
	do {{
	}} while (0);
	return nRet;
}}

)", fmt::arg("serverName", serverName));


    } while (0);
    return nRet;
}

int   moduleLogicServerGen:: genOnLoopEnd(appFile& rApp, const char* serverName)
{
	int   nRet = 0;
	do {
		std::string strRoot = rApp.srcPath ();
		strRoot += "/";
		strRoot += serverName;
		auto proc = strRoot;
		proc += "/proc/onLoopEnd.cpp";
		if (isPathExit (proc.c_str())) {
			nRet = 0;
			break;
		}
		std::ofstream os (proc.c_str ());
		if (!os) {
			rError ("open file : "<<proc.c_str ());
			nRet = 1;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onLoopEnd()
{{
	int nRet = 0;
	do {{
	}} while (0);
	return nRet;
}}

)", fmt::arg("serverName", serverName));

		} while (0);
return nRet;
}

int   moduleLogicServerGen:: genOnLoopFrame(appFile& rApp, const char* serverName)
{
	int   nRet = 0;
	do {
		std::string strRoot = rApp.srcPath ();
		strRoot += "/";
		strRoot += serverName;
		auto proc = strRoot;
		proc += "/proc/onLoopFrame.cpp";
		if (isPathExit (proc.c_str())) {
			nRet = 0;
			break;
		}
		std::ofstream os (proc.c_str ());
		if (!os) {
			rError ("open file : "<<proc.c_str ());
			nRet = 1;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onLoopFrame()
{{
	int nRet = 0;
	do {{
	}} while (0);
	return nRet;
}}

)", fmt::arg("serverName", serverName));
		} while (0);
return nRet;
}

int   moduleLogicServerGen:: genMgrCpp (appFile& rApp, const char* genPath)
{
	int   nRet = 0;
	do {
		// auto& rData = rMod.moduleData();
		auto& rSS = rApp.orderS ();
		auto& rGloble = tSingleton<xmlGlobalLoad>::single ();
		auto& rGlobleFile = tSingleton<globalFile>::single ();
		auto& rConfig = tSingleton<configMgr>::single ();
		auto structBadyType = rConfig.structBadyType ();

		auto pPmp = rGlobleFile.findMsgPmp ("defMsg");
		auto serializePackFunStName = pPmp->serializePackFunStName ();
		auto configClassName = rGlobleFile.configClassName ();
		// nTmpId 
		std::stringstream ssNew;
		for (auto it = rSS.begin (); it != rSS.end (); it++) {
			auto pS = it->get();
			auto pName = pS->serverName ();
			ssNew<<"	";
			if (it != rSS.begin ()) {
				ssNew<<" else ";
			}
			auto strTmpHandle = pS->strTmpHandle ();
			ssNew<<R"(if (rSerN.first == )"<<strTmpHandle<<R"() {
			m_)"<<pName<<R"(.first = std::make_unique<)"<<pName<<R"([]>(rSerN.second);
			m_)"<<pName<<R"(.second = rSerN.second;
			loopHandleType level,  onceLv, onceIndex;
			getLvevlFromSerId (rSerN.first, level, onceLv, onceIndex);
			auto& rBigLv = m_muServerPairS[level];
			auto& rOnce = rBigLv.first[onceLv];
			// auto& rSA = rBigLv.first[onceLv];
			for (decltype (rSerN.second) k = 0; k < rSerN.second; k++) {
				auto& rSr = m_)"<<pName<<R"(.first [k];
				rSr.setServerId (rSerN.first + k); 
				rOnce.first[k] = &rSr;
				rSr.setServerName (")"<<pName<<R"(");
				rSr.onServerInitGen (pForLogic);
				rSr.setAutoRun(rSerN.autoRun);
				rSr.setRoute(rSerN.route);
			}
		})";
		}
		auto serverNum = rSS.size ();
		std::string strFilename = genPath;
		std::string pModName = rApp.appName(); // rData.moduleName ();
		pModName += "M";
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		std::string genMgrCpp = genPath;
		genMgrCpp += "/";
		genMgrCpp += strMgrClassName;
		genMgrCpp += ".cpp";
		std::ofstream osCpp (genMgrCpp.c_str());
		if (!osCpp) {
			rError ("open file for write error fileName = "<<genMgrCpp.c_str ());
			nRet = 2;
			break;
		}
		auto& os = osCpp;
		osCpp<<R"(#include ")"<<strMgrClassName<<R"(.h"
#include "strFun.h"
)";
		auto bH = rGlobleFile.haveServer ();
		if (bH) {
			os<<R"(#include "loopHandleS.h"
)";
		}
os<<R"(
#include "tSingleton.h"
#include "msg.h"
#include <map>
#include <string>
#include <cstring>
#include "myAssert.h"
#include "rpcInfo.h"
#include ")"<<configClassName<<R"(.h"

struct conEndPointT 
{
	std::string ip;
	std::string strEndPointName;
	ServerIDType	targetHandle;
	uword       port;
	uword       userData;
	bool        bDef;
	bool		rearEnd; 
	bool        regRoute;
};
)"<<strMgrClassName<<R"(::)"<<strMgrClassName<<R"(()
{
	strCpy(")"<<pModName<<R"(", m_modelName);)"<<R"(
}

dword )"<<strMgrClassName<<R"(::afterLoadLogic (int nArgC, char** argS, ForLogicFun* pForLogic)
{
	dword nRet = 0;
	do {
		tSingleton<)"<<configClassName<<R"(>::createSingleton ();
		auto& rConfig = tSingleton<)"<<configClassName<<R"(>::single ();
		int  nR = 0;
		nR = rConfig.procCmdArgS (nArgC, argS);
		if (nR) {
			nRet = 1;
			break;
		}
		{
			auto pWorkDir = rConfig.homeDir();
			myAssert (pWorkDir);
			std::string strFile = pWorkDir;
			auto frameConfig = rConfig.logicConfigFile ();
			strFile += "/config/";
			strFile += frameConfig;
			rConfig.loadConfig (strFile.c_str());
		}
		nR = rConfig.procCmdArgS (nArgC, argS);
		if (nR) {
			nRet = 2;
			break;
		}
		m_pForLogicFun = pForLogic;
		auto& rFunS = getForMsgModuleFunS();
		rFunS = *pForLogic;
		struct  tempServerInfo
		{
			loopHandleType  first;
			loopHandleType  second;
			bool            autoRun;
			bool            route;
		};
		auto szModelS = rConfig.modelS ();
		std::unique_ptr<char[]> modelSBuf;
		strCpy(szModelS, modelSBuf);
		auto pModelSBuf = modelSBuf.get();
		const auto c_retMaxNum = 64;
		auto retS = std::make_unique<char* []>(c_retMaxNum);
		auto pRetS = retS.get();
		nR = strR (pModelSBuf, '*', pRetS, c_retMaxNum);
		myAssert (nR < c_retMaxNum);
		if (nR >= c_retMaxNum) {
			nRet = 3;
			break;
		}
		myAssert (2 == nR);
		if (2 != nR) {
			nRet = 4;
			break;
		}
		for (decltype (nR) i = 1; i < nR; i++) {
			const auto c_retSerMaxNum = 64;
			auto retSerS = std::make_unique<char* []>(c_retSerMaxNum);
			auto pRetSerS = retSerS.get();
			auto nRS = strR (pRetS[i], '+', pRetSerS, c_retSerMaxNum);
			auto serverN = (loopHandleType)(nRS);
			auto pSerNum = std::make_unique<tempServerInfo []>(serverN);
			for (decltype (nRS) j = 0; j < nRS; j++) {
				const auto c_retSerMaxNum = 6;
				char* onceRet[c_retSerMaxNum];
				auto onceR = strR (pRetSerS[j], '-', onceRet, c_retSerMaxNum);
				auto& rSerN = pSerNum [j];
				rSerN.first = (loopHandleType)(atoi(onceRet[0]));
				if (onceR > 1) {
					rSerN.second = atoi (onceRet[1]);
				} else {
					rSerN.second = 1;
				}
				if (onceR > 2) {
					rSerN.autoRun = atoi (onceRet[2]);
				}
				if (onceR > 3) {
					rSerN.route = atoi (onceRet[3]);
				}
				loopHandleType level,  onceLv, onceIndex;
				getLvevlFromSerId (rSerN.first, level, onceLv, onceIndex);
				auto& bigLv = m_muServerPairS[level];
				bigLv.second++;
			}
			for (decltype (serverN) j = 0; j < serverN; j++) {
				auto& rSerN = pSerNum [j];
				loopHandleType level,  onceLv, onceIndex;
				getLvevlFromSerId (rSerN.first, level, onceLv, onceIndex);
				auto& bigLv = m_muServerPairS[level];
				if (!bigLv.first) {
					bigLv.first = std::make_unique<logicServerPair[]>(bigLv.second);
					for (decltype (bigLv.second) k = 0; k < bigLv.second; k++) {
						bigLv.first[k].second = 0;
					}
				}
				auto& rOnceA = bigLv.first[onceLv];
				rOnceA.second = rSerN.second;
				rOnceA.first = std::make_unique<logicServer*[]>(rOnceA.second);
			}
			for (decltype (serverN) j = 0; j < serverN; j++) {
				auto& rSerN = pSerNum [j];
				)"<<ssNew.str()<<R"(
			}
					break;
		}
		const auto c_defProcBufSize = 512 * 2;
		auto defProcBuf = std::make_unique<loopHandleType[]>(c_defProcBufSize);
		auto pDefProc = defProcBuf.get();
		auto defProcNum = getDefProc (pDefProc, c_defProcBufSize);
		myAssert (defProcNum < c_defProcBufSize);
		nR = initMsgDefProc (pDefProc, defProcNum);
		if (nR) {
			nRet = 5;
			break;
		}
	)"
	<<R"(
)";
	
	os<<R"(
	} while (0);
	return nRet;
}
)";
	} while (0);
	return nRet;
}
int   moduleLogicServerGen:: genWorkerH (appFile& rApp, const char* serverName)
{
    int   nRet = 0;
    do {
		std::string strFile = rApp.srcPath (); // rMod.genPath ();
		strFile += "/";
		strFile += serverName;
		std::string strProcDir = strFile;
		strProcDir += "/proc/";
		strFile += "/gen/";
		myMkdir (strFile.c_str ());
		strFile += serverName;
		strFile += ".h";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		/*
		auto& rData = rMod.moduleData ();
		auto appName = rData.appName ();
		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto  pApp = rAppMgr.findApp (appName);
		myAssert (pApp);
		*/
		auto pS = rApp.findServer (serverName);
		myAssert (pS);
		std::set<std::string> procMsgSet;
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rRpcFileMgr = pPmp->rpcFileS();
		auto& rMsgMgr = pPmp->msgFileS();
		auto& rPMap = pS->procMsgS ();
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto pG = pRpc->groupName ();
			procMsgSet.insert (pG);
		}
		std::stringstream ssInc;
		std::string strWorker = "logicWorker";
		bool route = pS->route ();
		if (route) {
			ubyte  netType = rApp.netType ();
			if (appNetType_client == netType) {
				strWorker = "SingleConnectorWorker";
			} else if (appNetType_gate == netType) {
				strWorker = "gateRouteWorker";
			} else {
				strWorker = "serverWorker";
			}
		}

		for (auto iter = procMsgSet.begin(); procMsgSet.end() != iter; ++iter) {
			ssInc<<R"(#include ")"<<iter->c_str()<<R"(Rpc.h")"<<std::endl;
			std::string strDir = strProcDir;
			strDir += iter->c_str();
			myMkdir (strDir.c_str());
		}

		std::stringstream ssProc;
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto& rRpc = *pRpc;
			auto szMsgStructName = rProcRpc.bAsk ? 
				pRpc->askMsgName () : pRpc->retMsgName ();
			auto pMsg = rMsgMgr.findMsg (szMsgStructName);
 			auto pDec = pMsg->msgFunDec ();
			ssProc<<"    "<<pDec<<";"<<std::endl;
		}
		
		fmt::print(os, R"(#ifndef _{serverName}_h__
#define _{serverName}_h__

#include <memory>
#include "mainLoop.h"
#include "{pubClass}.h"
{include}

class {serverName}:public {pubClass}
{{
public:
	int onWorkerInitGen(ForLogicFun* pForLogic) override;
	int onWorkerInit(ForLogicFun* pForLogic) override;
	int onLoopBegin() override;
	int onLoopEnd() override;
	int onLoopFrame() override;
	int onCreateChannelRet(const channelKey& rKey, udword result) override;
	{procMsg}
private:
}};
#endif
)", fmt::arg("serverName", serverName), fmt::arg("include", ssInc.str().c_str()), fmt::arg("pubClass", strWorker.c_str()), fmt::arg("procMsg", ssProc.str().c_str()));
    } while (0);
    return nRet;
}

int   moduleLogicServerGen:: genWorkerCpp (appFile& rApp, const char* serverName)
{
    int   nRet = 0;
    do {
		// auto& rData = rMod.moduleData ();
		std::string strFile = rApp.srcPath (); // rMod.genPath ();
		strFile += "/";
		strFile += serverName;
		strFile += "/gen/";
		myMkdir (strFile.c_str ());
		strFile += serverName;
		strFile += "Gen.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		auto pS = rApp.findServer (serverName);
		myAssert (pS);
		auto& attrs = pS->attrs ();
		std::stringstream ssAttrs;
		for (auto it = attrs.begin (); it != attrs.end (); it++) {
			ssAttrs<<R"(setAttrFun(serverId (),")"<<*it<<R"(");)"<<std::endl;
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onWorkerInitGen(ForLogicFun* pForLogic)
{{

	{regPackFunDec};
	{regPackFunName}(pForLogic->fnRegMsg, serverId ());
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);

	{attrs}
	return nRet;
}}

)", fmt::arg("serverName", serverName), fmt::arg("regPackFunDec", pS->regPackFunDec ()), fmt::arg("regPackFunName", pS->regPackFunName ()), fmt::arg("attrs", ssAttrs.str()));

    } while (0);
    return nRet;
}

int   moduleLogicServerGen:: genWorkerMgrH (appFile& rApp)
{
	int   nRet = 0;
	do {
		auto modSrcPath = rApp.srcPath ();
		// auto& rData = rMod.moduleData ();
		std::string pModName = rApp.appName(); // rData.moduleName ();
		pModName += "M";
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rRpcFileMgr = pPmp->rpcFileS();
		auto& rMsgMgr = pPmp->msgFileS();
		std::string genMgrH = modSrcPath;
		std::string strMgrClassName = pModName;
		strMgrClassName += "WorkerMgr";
		genMgrH += "/gen/";
		genMgrH += strMgrClassName;
		genMgrH += ".h";
		std::ofstream osMgrH(genMgrH.c_str());
		if (!osMgrH) {
			rError ("open file for write error fileName = "<<genMgrH.c_str ());
			nRet = 2;
			break;
		}
		// auto appName = rData.appName ();

		// auto pApp = tSingleton<appFileMgr>::single().findApp (appName);
		// myAssert (pApp);
		auto netType = rApp.netType();
		if (appNetType_gate == netType ) {
		}
		std::stringstream serInc;
		std::stringstream serVar;
		auto& rSS = rApp.orderS ();
		auto serverNum = rSS.size ();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			serInc<<R"(#include ")"<<pName<<R"(.h")"<<std::endl;
			serVar<<"    std::pair<std::unique_ptr<"<<pName<<R"([]>, loopHandleType>      m_)"<<pName<<R"(;
)";
		}
		fmt::print(osMgrH, R"(#ifndef _{modName}WorkerMgr_h__
#define _{modName}WorkerMgr_h__

#include <memory>
#include "mainLoop.h"
#include "logicWorkerMgr.h"
{include}

class {modName}WorkerMgr:public logicWorkerMgr
{{
public:
	int initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS) override;
{serverS}
private:
}};
#endif
)", fmt::arg("include", serInc.str().c_str()), fmt::arg("modName", pModName), fmt::arg("serverS", serVar.str().c_str()));
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genWorkerMgrCpp (appFile& rApp)
{
    int   nRet = 0;
    do {
		auto modSrcPath = rApp.srcPath ();
		// auto& rData = rMod.moduleData ();
		std::string pModName = rApp.appName (); // rData.moduleName ();
		pModName += "M";
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rRpcFileMgr = pPmp->rpcFileS();
		auto& rMsgMgr = pPmp->msgFileS();
		std::string genMgrH = modSrcPath;
		std::string strMgrClassName = pModName;
		strMgrClassName += "WorkerMgr";
		genMgrH += "/gen/";
		genMgrH += strMgrClassName;
		genMgrH += ".cpp";
		std::ofstream osMgrC(genMgrH.c_str());
		if (!osMgrC) {
			rError ("open file for write error fileName = "<<genMgrH.c_str ());
			nRet = 2;
			break;
		}
		auto appName = rApp.appName ();
		/*
		auto pApp = tSingleton<appFileMgr>::single().findApp (appName);
		myAssert (pApp);
		*/
		auto netType = rApp.netType();
		if (appNetType_gate == netType ) {
		}
		std::stringstream serVar;
		auto& rSS = rApp.orderS ();
		auto serverNum = rSS.size ();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			auto serverGroupId = rServer.serverGroupId ();	
			if (i) {
				serVar<<"else ";
			}
			serVar<<R"(if (i == )"<<serverGroupId <<R"() {
				m_)"<<pName<<R"(.first = std::make_unique<)"<<pName<<R"([]>(serverGroups[i].runNum);
				for (decltype (serverGroups[i].runNum) j = 0; j < serverGroups[i].runNum; j++) {
					m_allServers [serverGroups[i].beginId + j] = m_)"<<pName<<R"(.first.get() + j;
				}
			} )";
		}
		fmt::print(osMgrC, R"(#include "{modName}WorkerMgr.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
#include "rpcInfo.h"

int {modName}WorkerMgr::initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS)
{{
	auto& rConfig = tSingleton<logicFrameConfig>::single ();
	int nRet = 0;
	do {{
		auto serverGroupNum = rConfig.serverGroupNum ();
		auto serverGroups = rConfig.serverGroups ();
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {{
			{newServers}
		}}

		const uword maxPairNum = 0x2000;
		const uword maxMsgNum = maxPairNum * 2;
		auto tempBuf = std::make_unique<uword[]>(maxMsgNum);
		auto nR = getDefProc (tempBuf.get(), maxMsgNum);
		myAssert (nR < maxMsgNum);
		nR = m_defProcMap.init((defProcMap::NodeType*)(tempBuf.get()), nR/2);
		myAssert (0 == nR);
	}} while (0);
	return nRet;
}}

)", fmt::arg("modName", pModName), fmt::arg("newServers", serVar.str().c_str()));

    } while (0);
    return nRet;
}
/*
int   moduleLogicServerGen:: genH (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto modSrcPath = rMod.srcPath ();
		auto& rData = rMod.moduleData ();
		auto pModName = rData.moduleName ();

		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rRpcFileMgr = pPmp->rpcFileS();
		auto& rMsgMgr = pPmp->msgFileS();
		std::string genMgrH = modSrcPath;
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		genMgrH += "/gen/";
		genMgrH += strMgrClassName;
		genMgrH += ".h";
		std::ofstream osMgrH(genMgrH.c_str());
		if (!osMgrH) {
			rError ("open file for write error fileName = "<<genMgrH.c_str ());
			nRet = 2;
			break;
		}
		std::stringstream serInc;
		std::stringstream serVar;
		auto& rSS = rData.orderS ();
		auto serverNum = rSS.size ();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			serInc<<R"(
)"<<R"(#include ")"<<pName<<R"(.h"
)";
			serVar<<"    std::pair<std::unique_ptr<"<<pName<<R"([]>, loopHandleType>      m_)"<<pName<<R"(;
)";
		}
		osMgrH<<R"(#ifndef )"<<pModName<<R"(ServerMgr_h__
#define )"<<pModName<<R"(ServerMgr_h__
#include "logicServer.h"
#include "logicServerMgr.h"
#include <map>
)"<<serInc.str()<<R"(
class )"<<strMgrClassName<<R"( : public logicServerMgr
{
public:
	)"<<strMgrClassName<<R"(();
	dword afterLoadLogic (int nArgC, char** argS, ForLogicFun* pForLogic) override;
)"<<serVar.str()<<R"(
};
#endif
		)";
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto& rServer = *rSS[i];
		auto pName = rServer.serverName ();

		std::string serverHFile = modSrcPath; // rMod.genPath ();
		// std::string serverHFile = frameFunDir;
		serverHFile += "/";
		serverHFile += pName;
		auto strProc = serverHFile;
		strProc += "/proc/";
		myMkdir (strProc.c_str());
		serverHFile += "/gen/";
		myMkdir (serverHFile.c_str());
		auto strGenPath = serverHFile;
		serverHFile += pName;
		serverHFile += ".h";
		
		std::ofstream osH(serverHFile.c_str ());
		if (!osH) {
			rError ("open file: "<<serverHFile.c_str ()<<" error");
			nRet = 4;
			break;
		}
		std::set<std::string> procMsgSet;
		auto& rPMap = rServer.procMsgS ();
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto pG = pRpc->groupName ();
			procMsgSet.insert (pG);
		}

		osH<<R"(#ifndef )"<<pName<<R"(_h__
#define )"<<pName<<R"(_h__
#include "logicServer.h"
#include "logicServerMgr.h"
)";
		for (auto iter = procMsgSet.begin(); procMsgSet.end() != iter; ++iter) {
			osH<<R"(#include ")"<<iter->c_str()<<R"(Rpc.h")"<<std::endl;
			auto strG = strProc;
			strG += iter->c_str();
			myMkdir (strG.c_str());
		}
osH<<R"(
class )"<<pName<<R"( : public  logicServer
{
public:
	)"<<pName<<R"( ();
	int onFrameFun () override;
	int onServerInitGen(ForLogicFun* pForLogic) override;
	int onServerInit(ForLogicFun* pForLogic);
	int sendAddChannel (udword& token, channelKey& rCh);
	int onAddChannelResult (udword token, udword result, channelKey& rKey);
	int sendDelChannel (channelKey & chK);
	int sendListenChannel (channelKey & chK);
	int sendQuitChannel (channelKey & chK);
	int sendPackToChannel(packetHead* pack, channelKey& chK, bool ntfMe = false);
	int sendMsgToChannel(CMsgBase& rMsg, channelKey& chK, bool ntfMe = false);
	void logicOnAcceptSession(SessionIDType sessionId, uqword userData)override;
	void logicOnConnect(SessionIDType sessionId, uqword userData)override;
	void onLoopBegin() override;
	void onLoopEnd() override;
public:
)";
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto& rRpc = *pRpc;
			auto szMsgStructName = rProcRpc.bAsk ? 
				pRpc->askMsgName () : pRpc->retMsgName ();
			auto pMsg = rMsgMgr.findMsg (szMsgStructName);
 			auto pDec = pMsg->msgFunDec ();
			osH<<"    "<<pDec<<";"<<std::endl;
		}
osH<<R"(
};
#endif
)";
		std::string serverLoopBeginCppFile = rMod.srcPath (); //frameFunDir;
		serverLoopBeginCppFile += "/";
		serverLoopBeginCppFile += pName;
		serverLoopBeginCppFile += "/proc/";
		auto procPath = serverLoopBeginCppFile;
		serverLoopBeginCppFile += pName;
		serverLoopBeginCppFile += "LoopBegin.cpp";
		auto bECLoopBegin = sIsPathExit (serverLoopBeginCppFile.c_str());
		if (bECLoopBegin) {
			continue;
		}
		std::ofstream osLoopBeginCpp (serverLoopBeginCppFile.c_str ()) ;
		if (!osLoopBeginCpp) {
			nRet = 5;
			rError ("open file: "<<serverLoopBeginCppFile.c_str ()<<" error");
			break;
		}
	osLoopBeginCpp<<R"(#include ")"<<pName<<R"(.h"
#include "gLog.h"

void )"<<pName<<R"(::onLoopBegin()
{
}

void )"<<pName<<R"(::onLoopEnd()
{
}
)";
		auto serverCppFile = procPath;
		serverCppFile += pName;
		serverCppFile += ".cpp";
		auto bEC = sIsPathExit (serverCppFile.c_str());
		if (bEC) {
			continue;
		}
		std::ofstream osCpp (serverCppFile.c_str ()) ;
		if (!osCpp) {
			nRet = 5;
			rError ("open file: "<<serverCppFile.c_str ()<<" error");
			break;
		}
		
		std::stringstream strRegRoute;
		
		osCpp<<R"(#include "comMsgRpc.h"
#include "gLog.h"
#include ")"<<pName<<R"(.h"
int )"<<pName<<R"(::onFrameFun ()
{
	return procPacketFunRetType_del;
}

void )"<<pName<<R"(::logicOnAcceptSession(SessionIDType sessionId, uqword userData)
{
}

void )"<<pName<<R"(::logicOnConnect(SessionIDType sessionId, uqword userData)
{
	gInfo ("connect finish userData = "<<userData);
)";osCpp<<R"(
}

int )"<<pName<<R"(::onAddChannelResult (udword token, udword result, channelKey& rKey)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: onServerInit(ForLogicFun* pForLogic)
{
	int nRet = 0;
	return nRet;
}
)";
		auto pH = rServer.strHandle();
	}
	if (nRet) {
		break;
	}
	} while (0);
	return nRet;
}
int   moduleLogicServerGen:: genCpp (moduleGen& rMod)
{
    int   nRet = 0;
    do {
		std::string genPath = rMod.srcPath();
		genPath += "/gen";
		myMkdir(genPath.c_str());
		int nR = 0;
		nR = genMgrCpp (rMod, genPath.c_str());
		if (nR) {
			rError ("genMgrCpp  error nR = "<<nR);
			break;
		}
		nR = genServerReadOnlyCpp (rMod);
		if (nR) {
			rError ("genServerReadOnlyCpp error nR = "<<nR);
			break;
		}
	} while (0);
    return nRet;
}
*/
static void   sOutListenChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(auto& rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			createChannel (rCh, srcSer, seId);
			break;
		}
		auto& rSidS = it->second;
		uqword uqwSe = srcSer;
		uqwSe <<= 32;
		uqwSe |= seId;
		auto inRet = rSidS.insert(uqwSe);
		if (!inRet.second) {
			gWarn("alder in channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
		}
	} while (0);
	)";
	}
}
static void   sOutSendToChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
			ps<<R"(auto rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			break;
		}
		auto& rSidS = it->second;
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToSomeSession;
		auto  ptrHandleS = std::make_unique<uqword[]> (rSidS.size());
		auto  pHandleS = ptrHandleS.get();
		udword   udwSeNum = 0;
		for (auto it = rSidS.begin(); rSidS.end() != it; ++it) {
			uqword uqwK = *it;
			auto sessionId = (SessionIDType)(uqwK);
			uqwK>>=32;
			auto toSid = (serverIdType)uqwK;
			if (rAsk.m_excSender && toSid == srcSer && sessionId == seId ) {
				continue;
			}
			pHandleS [udwSeNum++] = *it;
		}
		auto bodySize = rAsk.m_packNum - NetMsgLenSize;
		myAssert (bodySize >= 0);
		auto pN = (netPacketHead*)(rAsk.m_pack);
		myAssert (pN->udwLength = bodySize);
		fnSendPackToLoop (serverId(), pN, pHandleS, udwSeNum);
	} while (0);
	)";
		}
}
static void   sOutRegRoute(bool bAsk, std::ostream& ps)
{
	/*
	if (bAsk) {
			ps<<R"(auto fnRegRoute  =  getForMsgModuleFunS ().fnRegRoute;
	fnRegRoute  (serverId(), srcSer, seId, 0);)";
		}
		*/
}

static void   sOutQuitChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
			ps<<R"(auto rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			break;
		}
		auto& rSidS = it->second;
		uqword uqwSe = srcSer;
		uqwSe <<= 32;
		uqwSe |= seId;
		rSidS.erase(uqwSe);
	} while (0);
	)";
		}
}
static void   sOutDelChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(auto rSendS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	rSendS.erase(rCh);
	)";
	}
}
static void   sOutNtfExit (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(	gInfo("rec NtfExit ask");
	)";
	} else {
		ps<<R"(	gInfo("rec NtfExit ret");
	)";
	}
}

static void sOutNtfExitByNet (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(	gInfo("rec NtfExitByNet ask");
		/* ntfExitAskMsg  ask;
		sendMsgToAllOtherLocalServer (ask);
		*/
		setWillExit (true);
	)";
	} else {
		ps<<R"(	gInfo("rec NtfExitByNet ret");
	)";
	}
}

static void   sOutAddChannel (const std::string& strMgrClassName, bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto& rCh = *((channelKey*)rAsk.m_chKey);
	createChannel (rCh, srcSer, seId);
		)";	
	} else {
		ps<<R"(auto rSendS = channelSendS ();
	do {
		auto& rCh = *((channelKey*)rAsk.m_chKey);
	if (rRet.m_result) {
		onAddChannelResult (rAsk.m_token, rRet.m_result, rCh);
		rSendS.erase (rAsk.m_token);
		gError(" and channel error result = "<<rRet.m_result);
		break;
	}
	auto it = rSendS.find (rAsk.m_token);
	myAssert (rSendS.end () != it);
	if (rSendS.end () == it) {
		gError ("can not find sendS token = "<<rAsk.m_token);
		break;
	}
	uqword uqwSe = srcSer;
	uqwSe <<= 32;
	uqwSe |= seId;
	auto& rSidS = it->second;
	auto inRet = rSidS.insert(uqwSe);
	myAssert (inRet.second);
	if (!inRet.second) {
		gError ("addChennel ret have the save sessionId key = "<<uqwSe);
		onAddChannelResult (rAsk.m_token, 2, rCh);
		rSendS.erase (it);
		break;
	}
	uword rootNum = 0;
	auto pNetS = getNetServerS (rootNum);
	if (rSidS.size() == rootNum) {
		onAddChannelResult (rAsk.m_token, 0, rCh);
		rSendS.erase (it);
	}
	}while (0);
	)";
	}
}

static int sProcMsgReg (const char* pModName, serverFile* pServer,
		const procRpcNode& rProcRpc, const char* strHandle, const char* szMsgDir,
		std::ostream& os, std::ostream& ss)
{
	int nRet = 0;
	auto serverName = pServer->serverName ();
	bool bAsk = rProcRpc.bAsk;
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
	myAssert (pPmp);
	auto& rRpcFileMgr = pPmp->rpcFileS(); // tSingleton <rpcFileMgr>::single ();
	auto pRpc = rRpcFileMgr.findRpc (rProcRpc.rpcName.c_str());
	myAssert (pRpc);
	auto& rRpc = *pRpc;
	auto pGName = rRpc.groupName ();
	auto& rGMgr = pPmp->msgGroupFileS ();
	auto pGroup = rGMgr.findGroup (pGName);
	myAssert (pGroup);
	auto pFullMsg = pGroup->fullChangName ();
	auto szMsgStructName = bAsk ? pRpc->askMsgName () : pRpc->retMsgName ();
	auto& rMsgMgr = pPmp->msgFileS ();
	auto pMsg = rMsgMgr.findMsg (szMsgStructName);
	do {	
		myAssert (pMsg);
		if (!pMsg) {
			nRet = 1;
			break;
		}
		auto pMsgId = pMsg->strMsgId ();
		auto pPackFun = pMsg->packFunName ();

		auto askMsgStructName = pRpc->askMsgName ();
		auto retMsgStructName = pRpc->retMsgName ();
		// auto isChannel = pRpc->isChannel ();
		auto msgProcFun = pMsg->msgFunName ();
		auto pAskMsg = rMsgMgr.findMsg (askMsgStructName);
		myAssert (pAskMsg);
		auto pRetMsg = rMsgMgr.findMsg (retMsgStructName);

		std::string strMgrClassName = pModName;
		strMgrClassName += "WorkerMgr";
		os<<R"(
static int )"<<pPackFun<<
				R"((pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = )"<<rProcRpc.retValue<<R"(;
    )";
	os<<pAskMsg->msgName ()<<R"( ask (pAsk);
	pAsk = ask.getPack ();
	)";
	bool bHaveRet = true;
	if (bAsk) {
		if (pRetMsg) {
			std::string strRetMsgName = pRetMsg->msgName ();
			os<<strRetMsgName<<R"( ret;
    )";
		} else {
			bHaveRet = false;
		}
	} else {
		myAssert (pRetMsg);
		os<<pRetMsg->msgName ()<<R"( ret (pRet);
		pRet = ret.getPack ();
	)";
	}
	auto neetSe = pMsg->neetSession();
	if (neetSe) {
		if (bAsk) {
			os<<R"(	auto pPack = pAsk;
	)";
		} else {
			os<<R"(auto pPack = pRet;
	)";
		}
		os<<R"(auto pN = P2NHead(pPack);
	)";
	}
	
	os<<R"(
	auto&  workerMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto allServerNum = workerMgr.allServerNum ();
	myAssert (pArg->handle < allServerNum);
	auto pServer = workerMgr.allServers()[pArg->handle];
	auto pRealServer = dynamic_cast<)"<<serverName<<R"(*>(pServer);
	myAssert (pRealServer);
	pRealServer->)"<<msgProcFun <<R"(()";
	auto askHasData = pAskMsg->hasData ();
	if (askHasData) {
		os<<R"(*ask.pack())";
	}
	if (pRetMsg) {
		auto retHasData = pRetMsg->hasData ();
		if (retHasData) {
			if (askHasData) {
				os<<", ";
			}
			os<<R"(*ret.pack())";
		}
	}
	
	if (neetSe) {
		os<<", pN->ubySrcServId, pPack->sessionID";
	}
	os<<R"();
	)";
	if (bAsk ) {
		if (bHaveRet) {
			os<<R"(pRet = ret.pop();
		)";
		}
	} else {
		os<<R"(ret.pop ();
)";
	}
	os<<R"(
	ask.pop ();
	return nRet;
}
)";
		ss<<R"(    fnRegMsg (serId, )"<<pFullMsg<<
			R"(()"<<pMsgId<<R"(), )"<<pPackFun<<R"();
)";
		std::string strFile = szMsgDir;
		strFile += "/";
		auto msgFunName = pMsg->msgFunName ();
		strFile += msgFunName;
		strFile += ".cpp";
		bool is = sIsPathExit (strFile.c_str ());
		if (is) {
			break;
		}
		std::ofstream ps (strFile.c_str ());
		if (!ps) {
			rError ("open file : "<<strFile.c_str ()<<" error");
			nRet = 2;
			break;
		}
		auto pGSrcName = pGroup->rpcSrcFileName ();
		std::string strDec;
		
		pMsg->getClassMsgFunDec (serverName, strDec);
		ps<<R"(#include "tSingleton.h"
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
#include <memory>
)";
		auto bH = rGlobalFile.haveServer ();
		if (bH) {
			ps<<R"(#include "loopHandleS.h"
)";
		}

ps<<R"(#include "logicWorker.h"
#include ")"<<strMgrClassName<<R"(.h")"<<std::endl<<R"(
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<strDec<<R"(
{
	)";
	auto rpcName = pRpc->rpcName ();
		
	if (strcmp (rpcName, "ntfExit") == 0) {
		sOutNtfExit (bAsk, ps);
	} else if (strcmp (rpcName, "ntfExitByNet") == 0) {
		sOutNtfExitByNet (bAsk, ps);
	} else if (strcmp (rpcName, "addChannel") == 0) {
		sOutAddChannel (strMgrClassName, bAsk, ps);
	} else if (strcmp (rpcName, "delChannel") == 0) {
		sOutDelChannel (bAsk, ps);
	}  else if (strcmp (rpcName, "listenChannel") == 0) {
		sOutListenChannel (bAsk, ps);
	} else if (strcmp (rpcName, "sendToChannel") == 0) {
		sOutSendToChannel (bAsk, ps);
	} else if (strcmp (rpcName, "quitChannel") == 0) {
		sOutQuitChannel (bAsk, ps);
	} else if (strcmp (rpcName, "regRoute") == 0) {
		sOutRegRoute(bAsk, ps);
	}
ps<<R"(
}
)";
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genPackFun (appFile& rApp, const char* szServerName)
{
    int   nRet = 0;
    do {
		std::string strFile = rApp.srcPath (); // rMod.genPath ();
		strFile += "/";
		strFile += szServerName;
		strFile += "/gen/";
		myMkdir (strFile.c_str ());
		// strFile += szServerName;
		strFile += "ProPacket.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		os<<R"(#include "msgGroupId.h")"<<std::endl;
		os<<R"(#include "msgStruct.h")"<<std::endl;
		os<<R"(#include "loopHandleS.h")"<<std::endl;
		os<<R"(#include "logicFrameConfig.h")"<<std::endl;
		os<<R"(#include "tSingleton.h")"<<std::endl;
		os<<R"(#include "mainLoop.h")"<<std::endl;
		auto pServerF = rApp.findServer (szServerName);
		std::string pModuleName = rApp.appName (); //rMod.moduleData ().moduleName ();
		pModuleName += "M";
		myAssert (pServerF);

		std::string strMgrClassName = pModuleName;
		strMgrClassName += "WorkerMgr";
		os<<R"(#include ")"<<strMgrClassName<<R"(.h")"<<std::endl;
		
		auto strHandle = pServerF->strHandle ();
		auto& rMap = pServerF->procMsgS ();
		using groupSet = std::set<std::string>;
		groupSet  groupS;
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
        for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rMgr = pPmp->rpcFileS ();
			auto pRpc = rMgr.findRpc (it->rpcName.c_str ());
			myAssert (pRpc);
			auto pGName = pRpc->groupName ();
			groupS.insert (pGName);
        }
		for (auto it = groupS.begin(); groupS.end() != it; ++it) {
			auto& pGName = *it;
			os<<R"(#include ")"<<pGName<<R"(MsgId.h")"<<std::endl;
			os<<R"(#include ")"<<pGName<<R"(Rpc.h")"<<std::endl;
		}
		os<<std::endl;
		auto pRegFunName = pServerF->regPackFunDec ();
		std::stringstream ss;
		ss<<std::endl<<pRegFunName<<R"(
{
	int nRet = 0;
)";
		auto& rGMgr = pPmp->msgGroupFileS ();
		auto& rRpcFileMgr = pPmp->rpcFileS ();
		for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rProcRpc = *(it);
			auto& rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto& rRpc = *pRpc;
			auto pGName = rRpc.groupName ();
			auto pGroup = rGMgr.findGroup (pGName);
			myAssert (pGroup);
			
			std::string strCppDir = rApp.srcPath ();
			strCppDir += "/";
			strCppDir += szServerName;
			auto userLogic = strCppDir;
			userLogic += "/userLogic";
			myMkdir (userLogic.c_str());
			strCppDir += "/proc/";
			strCppDir += pGName;
			sProcMsgReg (pModuleName.c_str(), pServerF, rProcRpc,
					strHandle, strCppDir.c_str (), os, ss);
		} // for
		ss<<R"(    return nRet;
}
)";
	os<<ss.str()<<std::endl;
    } while (0);
    return nRet;
}


int   moduleLogicServerGen:: genServerReadOnlyCpp (appFile& rApp)
{
    int   nRet = 0;
    do {
		// auto& rData = rMod.moduleData ();
		std::string genPath = rApp.srcPath();
		genPath += "/gen";
		myMkdir(genPath.c_str());
		auto& rSS = rApp.orderS ();
		auto serverNum = rSS.size ();
		auto srcPath = rApp.srcPath();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			std::string strFile = srcPath;// genPath;
			strFile += "/";
			strFile += pName;
			strFile += "/gen/";
			myMkdir (strFile.c_str());
			strFile += pName;
			strFile += "Gen.cpp";
			auto regPackFunName = rServer.regPackFunName ();
			auto regPackFunDec = rServer.regPackFunDec ();
			std::string pModName = rApp.appName(); //rData.moduleName();
			pModName += "M";
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
			std::ofstream os (strFile.c_str ());
			os<<R"(#include ")"<<pName<<R"(.h"
#include ")"<<strMgrClassName<<R"(.h"
#include "comMsgRpc.h"
#include "tSingleton.h"
#include "loopHandleS.h"
#include "msg.h"
#include "gLog.h"
#include "comMsgMsgId.h"
#include "msgGroupId.h"

int )"<<pName<<R"( :: onServerInitGen(ForLogicFun* pForLogic)
{
	int nRet = 0;
	do {
		)"<<regPackFunDec <<R"(;
		)"<<regPackFunName<<R"( (pForLogic->fnRegMsg, serverId ());
		nRet = onServerInit(pForLogic);
		)";
	/*
	auto appName = rData.appName ();
	auto& rMap = tSingleton<appFileMgr>::single ();
	auto pApp = rMap.findApp (appName);
	myAssert (pApp);
	*/
	auto netType = rApp.netType(); 
	auto route = rServer.route ();
	if (appNetType_server == netType && route) {
		
		os<<R"(
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	// auto fun = rMgr.forLogicFunSt().fnSendPackUp;
	regRouteAskMsg askMsg;
	auto p = askMsg.getPack ();
	auto pN = P2NHead(p);
	pN->ubySrcServId = serverId ();
	askMsg.pop();
	// fun (p);
	)";
	}
		os<<R"(
	} while (0);
	return nRet;
}

)"<<pName<<R"(::)"<<pName<<R"(()
{
}

int )"<<pName<<R"( :: sendDelChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		delChannelAskMsg  askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		sendToAllGateServer (askMsg);
		/*
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		*/
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendListenChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		listenChannelAskMsg askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		sendToAllGateServer (askMsg);
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendQuitChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		quitChannelAskMsg askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		sendToAllGateServer (askMsg);
		
	} while (0);
	return nRet;
}
int )"<<pName<<R"( :: sendMsgToChannel(CMsgBase& rMsg, channelKey& chK, bool ntfMe)
{
	int  nRet = 0;
    do {
		auto pack = rMsg.pop();
		nRet = sendPackToChannel (pack, chK, ntfMe);
    } while (0);
    return nRet;
}

int )"<<pName<<R"( :: sendPackToChannel(packetHead* pack, channelKey& chK, bool ntfMe)
{
	int nRet = 0;
	do {
		auto fnFreePack = getForMsgModuleFunS ().fnFreePack;
		auto pSN = P2NHead(pack);
		auto toNetPack = getForMsgModuleFunS ().toNetPack;
		/*
		packetHead* pNew = nullptr;
		toNetPack (pSN, pNew);
		if (pNew) {
			fnFreePack (pack);
			pack = pNew;
			pSN = P2NHead(pack);
		}
		*/
		auto sendSize = NetHeadSize + pSN->udwLength;
		auto pT = (sendToChannelAsk*)(0);
		auto uqwS = (uqword)&(pT->m_pack[0]);
		uqwS += sendSize;
		auto p = allocPacket ((udword)uqwS);
		p->pAsk = 0;
		p->sessionID = EmptySessionID;
		auto pNN = P2NHead (p);
		pNN->ubySrcServId = serverId ();
		pNN->ubyDesServId = c_emptyLoopHandle;
		pNN->uwMsgID = comMsg2FullMsg(comMsgMsgId_sendToChannelAsk);
		NSetUnRet(pNN);
		auto pU = (sendToChannelAsk*)(N2User(pNN));
		memcpy (pU->m_pack, pSN, sendSize);
		pU->m_packNum = (udword)sendSize;
		pU->m_excSender = ntfMe ? 0 : 1;
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		fnFreePack (pack);
		auto fnSendPackToLoopForChannel = getForMsgModuleFunS ().fnSendPackToLoopForChannel;
		fnSendPackToLoopForChannel (p);
		
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendAddChannel (udword& token, channelKey& rCh)
{
	int nRet = 0;
	addChannelAskMsg ask;
	auto pU = ask.pack();
	auto pK = (channelKey*)(&(pU->m_chKey[0]));
	*pK = rCh;
	auto fnNextToken = getForMsgModuleFunS().fnNextToken;
	auto newToken = fnNextToken (serverId());
	
	do {
		
		pU->m_token = newToken;
		sendToAllGateServer (ask);
		
		token = newToken;
	} while (0);
	return nRet;
}
)";
		} // for
    } while (0);
    return nRet;
}


