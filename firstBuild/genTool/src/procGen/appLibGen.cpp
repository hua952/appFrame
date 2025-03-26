#include "appLibGen.h"
#include "strFun.h"
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
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "xmlGlobalLoad.h"
#include "tSingleton.h"
#include "rLog.h"

#include<filesystem>
using namespace std::filesystem;

appLibGen:: appLibGen (appFile&  rAppData):m_appData(rAppData)
{
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto  projectHome = rGlobalFile.projectHome ();
	std::string strDir = projectHome;
	strDir += "/gen/";
	auto appName = m_appData.appName(); // rMod.appName ();
	strDir += appName;
	strDir += "/";
	setHomeDir(strDir.c_str());
	strDir += "/src/";
	setSrcDir (strDir.c_str());
}

appLibGen:: ~appLibGen ()
{
}

const char*  appLibGen:: homeDir ()
{
    return m_homeDir.get ();
}

void  appLibGen:: setHomeDir (const char* v)
{
    strCpy (v, m_homeDir);
}

const char*  appLibGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  appLibGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

int  appLibGen:: batFileGen ()
{
	appFile& rApp = m_appData;
    int  nRet = 0;
    do {
		auto szAppName = rApp.appName ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		
		auto procId = rApp.procId ();
		auto& rMainArgS = rApp.mainArgS();

		auto& rMap = tSingleton<appFileMgr>::single ().appS ();
		
		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto& rApps = rAppMgr.appS ();

		// auto haveServer = rGlobalFile.haveServer ();
		// if (haveServer) {
			std::stringstream ts;
			ts<<"procId="<<procId;
			rMainArgS.push_back(ts.str());
		
			std::stringstream appSS;
			appSS<<"runWorkNum="<<rApp.runWorkNum ();
			rMainArgS.push_back(appSS.str());
		// }

		auto netType = rApp.netType ();
		/*
		if (appNetType_gate == netType) {
			rMainArgS.push_back("ip=0.0.0.0");
		}
		*/	
		{
			std::stringstream ts;
			ts<<"netNum="<<rGlobalFile.netNum();
			rMainArgS.push_back(ts.str());
		}
		{
			std::stringstream ts;
			ts<<"modelName="<<szAppName<<"Module";
			rMainArgS.push_back(ts.str());
		}
		/*
		{
			std::stringstream ts;
			ts<<"gateAppGroupId="<<rGlobalFile.gateAppGroupId();
			rMainArgS.push_back(ts.str());
		}
		
		{
			std::stringstream ts;
			ts<<"gateRouteServerGroupId="<<rGlobalFile.gateRouteServerGroupId();
			rMainArgS.push_back(ts.str());
		}
		*/
		{
			std::stringstream ts;
			uword uwT = rApp.netType ();
			ts<<"appNetType="<<uwT;
			rMainArgS.push_back(ts.str());
		}
		{
			std::stringstream ts;
			auto appGroupId  = rApp.appGroupIdInt ();
			ts<<"appGroupId ="<<appGroupId;
			rMainArgS.push_back(ts.str());
		}
		{
			std::stringstream ts;
			auto projectInstallDir = rGlobalFile.projectInstallDir ();
			ts<<"projectInstallDir="<<projectInstallDir;
			rMainArgS.push_back(ts.str());
		}
		// auto haveMsg = rGlobalFile.haveMsg();
		// if (haveMsg)
		if (rApps.size() > 1)
		{
			std::stringstream ts;
			ts<<"netLib=libeventSession";
			rMainArgS.push_back(ts.str());
		}
		std::string strLogFile = szAppName;
		strLogFile += ".log";
		std::string strLogFull = R"(logFile=)";
		strLogFull += strLogFile;
		rMainArgS.push_back(strLogFull);
		std::string level0Full = "level0=cppLevel0L ";
		rMainArgS.push_back(level0Full);
		std::string frameHomeFull = "frameHome=";
		auto aFreeHome = rGlobalFile.frameInstallPath();
		frameHomeFull += aFreeHome;
		rMainArgS.push_back(frameHomeFull);
	
		auto& rV = rApp.argS ();
		int findFrameConfig = 0;
		for (auto it = rV.begin(); rV.end() != it; ++it) {
			rMainArgS.push_back(*it);
			std::string k,v;
			stringToTwoValue (it->c_str(), k, v);
			if (k == "frameConfigFile") {
				findFrameConfig++;
			}
		}
		myAssert (findFrameConfig <= 1);
		if (!findFrameConfig) {
			std::string strFC = "frameConfigFile=";
			strFC += szAppName;
			strFC += "frameConfig.txt";
			rMainArgS.push_back(strFC);
		}
		{
			auto& rV = rGlobalFile.argS ();
			for (auto it = rV.begin(); rV.end() != it; ++it) {
				rMainArgS.push_back(*it);
			}
		}
    } while (0);
    return nRet;
}

int  appLibGen:: genWorkerMgrH ()
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string genMgrH = srcDir ();
		std::string strMgrClassName = m_appData.appName();
		strMgrClassName += "WorkerMgr";
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
		auto& rSS = m_appData.orderS ();
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
)", fmt::arg("include", serInc.str().c_str()), fmt::arg("modName", m_appData.appName ()), fmt::arg("serverS", serVar.str().c_str()));
    } while (0);
    return nRet;
}

int  appLibGen:: genWorkerMgrCpp ()
{
    int  nRet = 0;
    do {
		auto modSrcPath = m_appData.srcPath ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string genMgrH = srcDir ();
		std::string strMgrClassName = m_appData.appName();
		strMgrClassName += "WorkerMgr";
		genMgrH += strMgrClassName;
		genMgrH += ".cpp";
		std::ofstream osMgrC(genMgrH.c_str());
		if (!osMgrC) {
			rError ("open file for write error fileName = "<<genMgrH.c_str ());
			nRet = 2;
			break;
		}
		auto appName = m_appData.appName ();
		
		std::stringstream serVar;
		auto& rSS = m_appData.orderS ();
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

		std::string incRpcH;
		std::string strInitMsg;
		// auto haveMsg = rGlobalFile.haveMsg();
		// if (haveMsg){

		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		if (pPmp ) {
			incRpcH= R"(#include "rpcInfo.h")";
			strInitMsg = R"(
		const uword maxPairNum = 0x2000;
		const uword maxMsgNum = maxPairNum * 2;
		auto tempBuf = std::make_unique<uword[]>(maxMsgNum);
		auto nR = getDefProc (tempBuf.get(), maxMsgNum);
		myAssert (nR < maxMsgNum);
		nR = m_defProcMap.init((defProcMap::NodeType*)(tempBuf.get()), nR/2);
		myAssert (0 == nR);
			)";
		}
		//}
		fmt::print(osMgrC, R"(#include "{modName}WorkerMgr.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"
{incRpcH}

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
		{initMsg}
		
	}} while (0);
	return nRet;
}}

)", fmt::arg("modName", m_appData.appName()), fmt::arg("incRpcH", incRpcH), fmt::arg("newServers", serVar.str().c_str()), fmt::arg("initMsg", strInitMsg));


    } while (0);
    return nRet;
}

int   appLibGen:: genWorkerH (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string strFile = srcDir (); // rMod.genPath ();
		strFile += "/";
		strFile += serverName;
		create_directories (strFile);
		strFile += "/";
		strFile += serverName;
		strFile += ".h";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		
		std::stringstream ssInc;
		std::stringstream ssProc;

		std::string strWorker = "logicWorker";
		ubyte  netType = m_appData.netType ();
		if (appNetType_gate == netType) {
			strWorker = "gateCommonWorker";
		} else if (appNetType_server == netType) {
			strWorker = "serverCommonWorker";
		}
		bool route = rServer.route ();
		if (route) {
			if (appNetType_client == netType) {
				strWorker = "SingleConnectorWorker";
			} else if (appNetType_gate == netType) {
				strWorker = "gateRouteWorker";
			} else {
				strWorker = "serverRouteWorker";
			}
		}
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		if (pPmp ) {
			std::set<std::string> procMsgSet;
			auto& rRpcFileMgr = pPmp->rpcFileS();
			auto& rMsgMgr = pPmp->msgFileS();
			auto& rPMap = rServer.procMsgS ();
			for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
				auto& rProcRpc = *(ite);
				auto rpcFileName = rProcRpc.rpcName;
				auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
				myAssert (pRpc);
				auto pG = pRpc->groupName ();
				procMsgSet.insert (pG);
			}
			

			for (auto iter = procMsgSet.begin(); procMsgSet.end() != iter; ++iter) {
				ssInc<<R"(#include ")"<<iter->c_str()<<R"(Rpc.h")"<<std::endl;
			}

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

int   appLibGen:: genWorkerCpp (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string strFile = srcDir();
		strFile += "/";
		strFile += serverName;
		strFile += "/";
		strFile += serverName;
		strFile += "Gen.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		auto& attrs = rServer.attrs ();
		std::stringstream ssAttrs;
		for (auto it = attrs.begin (); it != attrs.end (); it++) {
			ssAttrs<<R"(setAttrFun(serverId (),")"<<*it<<R"(");)"<<std::endl;
		}
		std::string strRegPackFunDec;
		std::string strRegPackFunName;

		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		if (pPmp) {
			strRegPackFunDec = rServer.regPackFunDec ();
			strRegPackFunName = rServer.regPackFunName ();
			strRegPackFunName += "(pForLogic->fnRegMsg, serverId ());";
		}
		fmt::print(os, R"(#include "{serverName}.h"

int {serverName}::onWorkerInitGen(ForLogicFun* pForLogic)
{{

	{regPackFunDec};
	{regPackFunName}
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	{attrs}
	return nRet;
}}

)", fmt::arg("serverName", serverName), fmt::arg("regPackFunDec", strRegPackFunDec), fmt::arg("regPackFunName", strRegPackFunName), fmt::arg("attrs", ssAttrs.str()));

    } while (0);
    return nRet;
}

static int sProcMsgReg (appFile& rApp, serverFile* pServer,
		const procRpcNode& rProcRpc, std::ostream& os, std::ostream& ss)
{
	int nRet = 0;
	auto serverName = pServer->serverName ();
	bool bAsk = rProcRpc.bAsk;
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
	do {	
		if (!pPmp) {
			break;
		}
		myAssert (pPmp);
		auto& rRpcFileMgr = pPmp->rpcFileS();
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
		myAssert (pMsg);
		if (!pMsg) {
			nRet = 1;
			break;
		}
		auto pMsgId = pMsg->strMsgId ();
		auto pPackFun = pMsg->packFunName ();

		auto askMsgStructName = pRpc->askMsgName ();
		auto retMsgStructName = pRpc->retMsgName ();
		auto msgProcFun = pMsg->msgFunName ();
		auto pAskMsg = rMsgMgr.findMsg (askMsgStructName);
		myAssert (pAskMsg);
		auto pRetMsg = rMsgMgr.findMsg (retMsgStructName);

		std::string strMgrClassName = rApp.appName ();
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
	} while (0);
	return nRet;
}
int   appLibGen:: genPackFun (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		if (!pPmp) {
			break;
		}
		std::string strFile = srcDir(); // rMod.genPath ();
		auto szServerName = rServer.serverName ();
		strFile += "/";
		strFile += szServerName;
		strFile += "/";
		strFile += "ProPacket.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}

		// auto haveMsg = rGlobalFile.haveMsg ();
		// if (haveMsg) {
			os<<R"(#include "msgGroupId.h")"<<std::endl;
			os<<R"(#include "msgStruct.h")"<<std::endl;
			// os<<R"(#include "loopHandleS.h")"<<std::endl;
		// }
		os<<R"(#include "logicFrameConfig.h")"<<std::endl;
		os<<R"(#include "tSingleton.h")"<<std::endl;
		os<<R"(#include "mainLoop.h")"<<std::endl;
		auto pServerF = &rServer;// rApp.findServer (szServerName);
		myAssert (pServerF);

		std::string strMgrClassName = m_appData.appName ();
		strMgrClassName += "WorkerMgr";
		os<<R"(#include ")"<<strMgrClassName<<R"(.h")"<<std::endl;
		
		auto& rMap = pServerF->procMsgS ();
		using groupSet = std::set<std::string>;
		groupSet  groupS;
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
			
			sProcMsgReg (m_appData, &rServer, rProcRpc, os, ss);
		} // for
		ss<<R"(    return nRet;
}
)";
	os<<ss.str()<<std::endl;
    } while (0);
    return nRet;
}

int   appLibGen:: cmakeListsGen ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto appName = m_appData. appName (); // rMod.appName ();
		std::string strFile = homeDir ();
		strFile += "CMakeLists.txt";
		
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");

		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strFile.c_str ());
			break;
		}
		os<<"SET(prjName "<<appName<<"Lib)"<<std::endl
			<<R"(set(serSrcS))"<<std::endl;

		auto& rSS = m_appData.orderS ();
		std::stringstream incS;
		std::stringstream cppS;

		using groupSet = std::set<std::string>;
		for (auto it = rSS.begin (); rSS.end () != it; ++it) {
			auto pServerF = it->get();
			auto pName = pServerF->serverName ();
			incS<<"src/"<<pName<<std::endl;
			cppS<<"src/"<<pName<<"/*.cpp ";
			/*
			groupSet  groupS;
			auto& rMap = pServerF->procMsgS ();
			for (auto ite = rMap.begin(); rMap.end() != ite; ++ite) {
				auto& rMgr = pPmp->rpcFileS ();
				auto pRpc = rMgr.findRpc (ite->rpcName.c_str ());
				myAssert (pRpc);
				auto pGName = pRpc->groupName ();
				groupS.insert (pGName);
			}
			*/
		}

		os<<R"(set(genSrcS)
file(GLOB genSrcS src/*.cpp )"<<cppS.str()<<R"()
file(GLOB mainSrcS src/main.cpp )
set(defS)
set(libPath)
set(libDep)
set(mainLibS)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc src/unix/*.cpp)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB osSrc src/win/*.cpp)
	list(APPEND mainLibS ws2_32)
	include_directories( )";
	// auto depInc = rGlobalFile.depIncludeHome ();
	os<<")"<<std::endl;
	// auto depLib = rGlobalFile.depLibHome ();

	// os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()
	include_directories(
	src)";
	auto frameInPath = rGlobalFile.frameInstallPath ();
	auto configClassName = rGlobalFile.configClassName ();
	std::string incPath = rGlobalFile.frameIncPath ();
	incPath += "/appFrame";
	os<<szC2<<std::endl;
	if (pPmp) {
		os<<"    ${CMAKE_SOURCE_DIR}/gen/defMsg/src"<<std::endl;
	}
	os<<"	${CMAKE_SOURCE_DIR}/gen/"<<configClassName<<"/src"<<std::endl
	<<"src/userLogic"<<std::endl
	<<incPath<<std::endl
	<<incS.str();
	auto prjName = rGlobalFile.projectName ();
	os<<")"<<std::endl;

	auto libPath = rGlobalFile.frameLibPath ();

	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl
	<<R"(link_directories(${libPath} ${libDep})
	add_library(${prjName} ${genSrcS})
	add_executable ()"<<appName<<R"( ${mainSrcS} ${osSrc})
target_link_libraries()"<<appName<<R"( PRIVATE
	common
	${mainLibS}
	)

	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(TARGETS )"<<appName<<R"( RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	)";

    } while (0);
    return nRet;
}

int   appLibGen:: genMain ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		nR = writeMain ();
		if (nR) {
			nRet = 1;
			rError("writeMain return error nR = "<<nR);
			break;
		}

		nR = writeWinDep ();
		if (nR) {
			nRet = 2;
			rError("writeWinDep return error nR = "<<nR);
			break;
		}
		nR = writeUnixDep ();
		if (nR) {
			nRet = 3;
			rError("writeUnixDep return error nR = "<<nR);
			break;
		}
    } while (0);
    return nRet;
}

int   appLibGen:: startGen ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		auto& orderS  = m_appData.orderS ();
		for (auto it = orderS.begin (); it != orderS.end (); it++) {
			auto& rServer = *(it->get());
			auto serverName = rServer.serverName();
			std::string strDir = srcDir ();
			strDir += serverName;
			myMkdir (strDir.c_str());
			nR = genWorkerH (rServer);
			if (nR) {
				rError(" genWorkerH return error nR = "<<nR<<" serverName= " <<serverName);
				nRet = 1;
				break;
			}
			nR = genWorkerCpp (rServer);
			if (nR) {
				rError(" genWorkerCpp return error nR = "<<nR<<" serverName= " <<serverName);
				nRet = 8;
				break;
			}
			nR = genPackFun (rServer);
			if (nR) {
				rError(" genPackFun return error nR = "<<nR);
				nRet = 7;
				break;
			}
		}
		if (nR) {
			break;
		}
		nR = cmakeListsGen ();
		if (nR) {
			rError(" cmakeListsGen return error nR = "<<nR);
			nRet = 2;
			break;
		}
		/*
		nR = genExportFunH ();
		if (nR) {
			rError(" genExportFunH return error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = genExportFunCpp ();
		if (nR) {
			rError(" genExportFuncpp return error nR = "<<nR);
			nRet = 4;
			break;
		}
		*/
		nR = genWorkerMgrH ();
		if (nR) {
			rError(" genWorkerMgrH return error nR = "<<nR);
			nRet = 5;
			break;
		}
		nR = genWorkerMgrCpp ();
		if (nR) {
			rError(" genWorkerMgrCpp return error nR = "<<nR);
			nRet = 6;
			break;
		}
		nR = batFileGen ();
		if (nR) {
			rError(" batFileGen return error nR = "<<nR);
			nRet = 3;
			break;
		}

		nR = genMain ();
		if (nR) {
			rError(" genMain return error nR = "<<nR);
			nRet = 9;
			break;
		}
    } while (0);
    return nRet;
}

int  appLibGen:: writeUnixDep ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/unix";
		myMkdir(strFile.c_str());
		strFile += "/depSys_unix.cpp";
		std::ofstream os (strFile.c_str());
		if (!os) {
			break;
		}
		os<<R"(int   beginMain(int argC, char** argV)
{
    int   nRet = 0;
    do {
    } while (0);
    return nRet;
}

void endMain()
{
    do {
    } while (0);
})";
    } while (0);
    return nRet;
}

int  appLibGen:: writeMain ()
{
	appFile& rApp = m_appData;
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/main.cpp";
		
		std::ofstream os(strFile.c_str());
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#include "comFun.h"
#include "strFun.h"
#include <string.h>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include "myAssert.h"
#include "loop.h"
#include "modelLoder.h"
)";

	auto& rGlobalFile = tSingleton<globalFile>::single ();
	// auto  bHave = rGlobalFile.haveServer ();
	// auto  haveMsg = rGlobalFile.haveMsg ();
	// if (/*bHave && */haveMsg) {
		/*
		os<<R"(#include "loopHandleS.h"
)";
*/
	//}
os<<R"(
int  beginMain(int argC, char** argV);
void endMain();
using defArgMap = std::map<std::string, std::string>;
void addDefKV(defArgMap& rMap, const char* szKV, const char delim = '=')
{
	auto ret = stringSplit (szKV, delim);
	if (ret.size() == 2) {
		rMap[ret[0]] = ret[1];
	}
}

int main(int cArg, char** argS)
{
	auto& ws = std::cout;
	ws<<"start main"<<std::endl;
	int nRet = 0;
	nRet = beginMain(cArg, argS);
	do {
		if (nRet) {
			ws<<" beginMain error nRet = "<<nRet<<std::endl;
			break;
		}
		defArgMap aDefArgMap;
)";
		auto& rMainArgS = rApp.mainArgS();
		for (auto it = rMainArgS.begin(); rMainArgS.end() != it; ++it) {
			os<<R"(		addDefKV(aDefArgMap, ")"<<it->c_str()<<R"(");
)";
		}

		os<<R"(
		std::string strDefDelim;
		char defDelim ='#';
		getOnceValueFromArgS (cArg, argS, "defDelim", strDefDelim);
		if (!strDefDelim.empty()) {
			defDelim = strDefDelim.c_str()[0];
		}
		for (decltype (cArg) i = 1; i < cArg; i++) {
			addDefKV (aDefArgMap, argS[i], defDelim);
		}
		int defArgLen = 0;
		for (auto it = aDefArgMap.begin (); it != aDefArgMap.end (); it++) {
			defArgLen += it->first.length();
			defArgLen += it->second.length();
			defArgLen += 2;
		}
		auto nDefArg = (int)(aDefArgMap.size());
		auto defArgS = std::make_unique<char*[]>(nDefArg + 1);
		auto defArgTxt = std::make_unique<char[]>(defArgLen);
		int curDef = 0;
		char* pCur = defArgTxt.get();
		for (auto it = aDefArgMap.begin (); it != aDefArgMap.end (); it++) {
			defArgS[curDef++] = pCur;
			pCur += (sprintf (pCur, "%s=%s", it->first.c_str(), it->second.c_str()) + 1);
		}

		std::string pLevel0Name;
		std::string strFrameHome;

		auto level0Ret = getTwoValueFromArgS (cArg, argS,  "level0", "frameHome", pLevel0Name, strFrameHome);
		if (!level0Ret) {
			level0Ret = getTwoValueFromArgS (nDefArg, defArgS.get(),  "level0", "frameHome", pLevel0Name, strFrameHome);
		}
		
		if (pLevel0Name.empty()) {
			std::cout<<"LevelName empty"<<std::endl;
			nRet = 1;
		} else {
			std::string strDll;
			if (!strFrameHome.empty()) {
				strDll += strFrameHome;
				strDll += "/";
			}
			strDll += getDllPath (pLevel0Name.c_str());
			auto handle = loadDll (strDll.c_str());
			myAssert(handle);
			do {
				if(!handle) {
					nRet = 11;
					ws<<"load module "<<strDll<<" error"<<std::endl;
					break;
				}
				typedef int (*initFunType) (int cArg, char** argS, int cDArg, char** argDS);
				auto funOnLoad = (initFunType)(getFun(handle, "initFun"));
				myAssert(funOnLoad);
				if(!funOnLoad) {
					std::cout<<"Level0 initFun empty error is"<<std::endl;
					nRet = 12;
					break;
				}
				auto nnR = funOnLoad (cArg, argS, nDefArg, defArgS.get());
				if (nnR) {
					std::cout<<"funOnLoad error nnR = "<<nnR<<std::endl;
					break;
				}
				defArgTxt.reset ();
				defArgS.reset();
				)";
				auto bDet = rApp.detachServerS ();
				if (bDet) {
					// auto mainLoopServer = rApp.mainLoopServer();
					std::string mainLoopGroupId =  rApp.mainLoopGroupId();
					// if (bHave ) {
						// if (!haveMsg) {
							auto mainServer = rApp.mainServer ();
							myAssert(mainServer);
							std::stringstream ts;
							ts<<mainServer->serverGroupId ();
							mainLoopGroupId = ts.str();
						// }
					// }
					if (!mainLoopGroupId.empty()) {
					os<<R"(typedef int (*loopBeginFT)(loopHandleType pThis);
	auto funLoopBegin = (loopBeginFT)(getFun(handle, "onPhyLoopBegin"));
	typedef int (*loopEndFT)(loopHandleType pThis);
	auto funLoopEnd = (loopEndFT)(getFun(handle, "onPhyLoopEnd"));
	typedef bool (*loopFrameFT)(loopHandleType pThis);
	auto funLoopFrame = (loopFrameFT)(getFun(handle, "onPhyFrame"));

	typedef bool (*loopFrameFT)(loopHandleType pThis);

	typedef int  (*runThNumFT) (char*, int);
	auto funRunThNum = (runThNumFT)(getFun(handle, "onPhyGetRunThreadIdS"));
	
	typedef int (*getServerGroupInfoFT)(uword, ubyte*, ubyte*);
	auto funGetServerGroupInfo = (getServerGroupInfoFT)(getFun(handle, "getServerGroupInfo"));
	ubyte beginId = 0;
	ubyte runNum = 0;
	auto  getGroupInfoRet = funGetServerGroupInfo ()"<<mainLoopGroupId<<R"(, &beginId, &runNum);
	myAssert (0 == getGroupInfoRet);

	auto nBegRet = funLoopBegin (beginId);
	if (!nBegRet) {
		while(1) {
			auto bE = funLoopFrame (beginId);
			if (bE) {
				break;
			}
		}
	}
	
	funLoopEnd (beginId);
	int curRunNum = 0;
	const auto c_tempSize = 256;
	auto tempBuf = std::make_unique<char[]>(c_tempSize);
	auto pTemp = tempBuf.get();
	do {
		curRunNum = funRunThNum (pTemp, c_tempSize);
		if (curRunNum) {
			std::cout<<" leaf run num is: "<<curRunNum<<" run serverS is : "<<pTemp<<std::endl;
			std::this_thread::sleep_for(std::chrono::microseconds (500000));
		}
	} while (curRunNum);
					)";
				}
		}
				os<<R"(
				std::cout<<pTemp<<std::endl;
			}while(0);
		}
		endMain();
	} while (0);
	std::this_thread::sleep_for(std::chrono::microseconds (1000000));
	std::cout<<" app exit now"<<std::endl;
	return nRet;
})";
    } while (0);
    return nRet;
}

int  appLibGen:: writeWinDep ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/win";
		myMkdir (strFile.c_str());
		strFile += "/depSys_win.cpp";
		std::ofstream os (strFile.c_str());
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#include <Winsock2.h>
#include <windows.h>

int   beginMain(int argC, char** argV)
{
    int   nRet = 0;
    do {
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			nRet = 1;
		}
    } while (0);
    return nRet;
}

void endMain()
{
    do {
    } while (0);
})";
    } while (0);
    return nRet;
}

