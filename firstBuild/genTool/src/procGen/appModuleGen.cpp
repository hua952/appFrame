#include "appModuleGen.h"
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

appModuleGen:: appModuleGen (appFile&  rAppData):m_appData(rAppData)
{
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto  projectHome = rGlobalFile.projectHome ();
	std::string strDir = projectHome;
	strDir += "/";
	auto appName = m_appData.appName(); // rMod.appName ();
	strDir += appName;
	strDir += "/";
	setHomeDir(strDir.c_str());
	strDir += "/src/";
	setSrcDir (strDir.c_str());
}

appModuleGen:: ~appModuleGen ()
{
}

const char*  appModuleGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  appModuleGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

const char*  appModuleGen:: homeDir ()
{
    return m_homeDir.get ();
}

void  appModuleGen:: setHomeDir (const char* v)
{
    strCpy (v, m_homeDir);
}

int  appModuleGen:: genExportFunH ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		create_directories (strFile);
		strFile += "/exportFun.h";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file for write error fileName = "<<strFile.c_str ());
			nRet = 1;
			break;
		}
		const char* szCon = R"(#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"

extern "C"
{
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic, int nDefArgC, char** defArgS, char* taskBuf, int taskBufSize);
	int onLoopBegin	(serverIdType	fId);
	int onFrameLogic	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
	void  beforeUnload();
	int   onRecPacket(serverIdType	fId, packetHead* pack);
}
#endif)";
		os<<szCon;
    } while (0);
    return nRet;
}

int appModuleGen:: genExportFunCpp ()
{
    int  nRet = 0;
    do {
		std::string strMgrClassName = m_appData.appName (); 
		strMgrClassName += "WorkerMgr";

		std::string strFile = srcDir ();
		strFile += "/exportFun.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file for write error fileName = "<<strFile.c_str ());
			nRet = 1;
			break;
		}

		os<<R"(#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "myAssert.h"
#include "logicWorker.h"
#include "logicWorkerMgr.h"
#include "tSingleton.h"
#include "gLog.h"
#include ")"<<strMgrClassName<<R"(.h"

dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic, int nDefArgC, char** defArgS, char* taskBuf, int taskBufSize)
{
	setForMsgModuleFunS (pForLogic);
	tSingleton<)"<<strMgrClassName<<R"(>::createSingleton();
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	logicWorkerMgr::s_mgr = &rMgr;
	return rMgr.initLogicWorkerMgr (nArgC, argS, pForLogic, nDefArgC, defArgS, taskBuf, taskBufSize);
}

int onLoopBegin	(serverIdType	fId)
{
	int nRet = 1;
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		nRet = pS->onLoopBeginBase	();
	}
	return nRet;
}

int onFrameLogic	(serverIdType	fId)
{
	int nRet = procPacketFunRetType_del;
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		if (pS->willExit()) {
			nRet = procPacketFunRetType_exitNow;
		} else {
			nRet = pS->onLoopFrameBase();
		}
	}
	return nRet;
}

void onLoopEnd	(serverIdType	fId)
{
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->onLoopEndBase();
	}
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}
int   onRecPacket(serverIdType	fId, packetHead* pack)
{
	return tSingleton<)"<<strMgrClassName<<R"(>::single().processOncePack (fId, pack);
}
)";
    } while (0);
    return nRet;
}

int   appModuleGen:: versFileGen ()
{
    int  nRet = 0;
    do {
		std::string win = srcDir ();
		win += "/unix";
		myMkdir (win.c_str());
		auto defFun = win;
		defFun += "/lib.vers";
		std::ofstream os(defFun.c_str ());
		if (!os) {
			nRet = 1;
			rError (" open file for write error fileName = "<<defFun.c_str ());
			break;
		}
		const char* szCon = R"(MYLIBRARY_1.0 {
    global:
		afterLoad;
		onLoopBegin;
		onLoopEnd;
		beforeUnload;
		onFrameLogic;
		onRecPacket;
    local:
        *;
};
)";
		os<<szCon;
    } while (0);
    return nRet;

}

int  appModuleGen:: genBashFile ()
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto projectInstallDir = rGlobalFile.projectInstallDir ();
		auto appName = m_appData.appName();
		std::string win =projectInstallDir;
		win += "/bin";
		myMkdir (win.c_str());
		auto defFun = win;
		defFun += "/run";
		defFun += appName;
		defFun += ".sh";
		std::ofstream os(defFun.c_str ());
		if (os) {
		os<<R"(#!/bin/bash
LD_LIBRARY_PATH=)"<<rGlobalFile.frameInstallPath()<<R"(/lib )"<<projectInstallDir<<"/bin/"<<appName<<R"( $@)";
		} else {
			rError (" open file for write error fileName = "<<defFun.c_str ());
		}
		auto defArgsFile = win;
		defArgsFile += "/";
		defArgsFile += appName;
		defArgsFile += "DefArgs.txt";
		if (!isPathExit (defArgsFile.c_str())) {
			std::ofstream osDefArgs(defArgsFile.c_str ());
			if (osDefArgs) {
				auto& rMainArgS = m_appData.mainArgS();
				for (auto it = rMainArgS.begin (); it != rMainArgS.end (); it++) {
					std::unique_ptr<char[]>	kvPt;
					strCpy (it->c_str (), kvPt);
					repChar (kvPt.get(), '=', '#');
					osDefArgs<<kvPt.get()<<std::endl;
				}
			} else {
				rError (" open defArgs file for write error fileName = "<<defArgsFile.c_str ());
			}
		}
    } while (0);
    return nRet;
}

int  appModuleGen:: genWinDef ()
{
    int  nRet = 0;
    do {
		std::string win = srcDir ();
		win += "/win";
		myMkdir (win.c_str());
		auto defFun = win;
		defFun += "/defFun.def";
		std::ofstream os(defFun.c_str ());
		if (!os) {
			nRet = 1;
			rError (" open file for write error fileName = "<<defFun.c_str ());
			break;
		}
		const char* szCon = R"(LIBRARY
EXPORTS
	afterLoad	@1
	onLoopBegin		@2
	onLoopEnd		@3
	beforeUnload	@4
	onFrameLogic	@5
	onRecPacket     @6
	)";
		os<<szCon;
    } while (0);
    return nRet;
}

int   appModuleGen:: startGen ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		std::string strUserLogic = srcDir ();
		strUserLogic += "/userLogic";
		myMkdir (strUserLogic.c_str());
		nR = genExportFunH ();
		if (nR) {
			rError(" genExportFunH return error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = genExportFunCpp ();
		if (nR) {
			rError(" genExportFunCpp return error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = genWinDef ();
		if (nR) {
			rError(" genWinDef return error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = versFileGen ();
		if (nR) {
			rError("versFileGen  return error nR = "<<nR);
			nRet = 6;
			break;
		}

		nR = cmakeListsGen ();
		if (nR) {
			rError(" genWinDef return error nR = "<<nR);
			nRet = 4;
			break;
		}
		genBashFile ();
		auto& orderS  = m_appData.orderS ();
		for (auto it = orderS.begin (); it != orderS.end (); it++) {
			auto& rServer = *(it->get());
			auto serverName = rServer.serverName();
			std::string strDir = srcDir ();
			strDir += serverName;
			strDir += "/userLogic";
			myMkdir (strDir.c_str());
			nR = genServer (rServer);
			if (nR) {
				rError(" genServer  ret error nR = "<<nR);
				nRet = 5;
				break;
			}
		}
    } while (0);
    return nRet;
}

int   appModuleGen:: cmakeListsGen ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto appName = m_appData.appName (); // rMod.appName ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		std::string strFile = homeDir ();
		strFile += "/CMakeLists.txt";
		auto bE = isPathExit (strFile.c_str());
		if (bE) {
			nRet = 0;
			break;
		}
		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strFile.c_str ());
			break;
		}
		os<<"SET(prjName "<<appName<<"Module)"<<std::endl
			<<R"(set(serSrcS))"<<std::endl;

		auto& rSS = m_appData.orderS ();
		std::stringstream incS;
		incS<<"${CMAKE_SOURCE_DIR}/gen/"<<appName<<"/src"<<std::endl;
		std::stringstream cppS;
		cppS<<"src/*.cpp ";
		using groupSet = std::set<std::string>;
		for (auto it = rSS.begin (); rSS.end () != it; ++it) {
			auto pName = it->get()->serverName ();
			incS
				<<"${CMAKE_SOURCE_DIR}/gen/"<<appName<<"/src/"<<pName<<std::endl
				<<"src/"<<pName<<std::endl
				<<"src/"<<pName<<"/userLogic"<<std::endl;
			cppS<<"src/"<<pName<<"/*.cpp"<<" "<<"src/"<<pName<<"/userLogic/*.cpp ";
			groupSet  groupS;
			auto serverName = pName;
			auto pServerF = it->get();
			myAssert (pServerF);
			auto& rMap = pServerF->procMsgS ();
			if (pPmp) {
				myAssert (pPmp);
				for (auto ite = rMap.begin(); rMap.end() != ite; ++ite) {
					auto& rMgr = pPmp->rpcFileS ();
					auto pRpc = rMgr.findRpc (ite->rpcName.c_str ());
					myAssert (pRpc);
					auto pGName = pRpc->groupName ();
					groupS.insert (pGName);
				}
			}

			for (auto ite = groupS.begin(); groupS.end() != ite; ++ite) {
				cppS<<"src/"<<pName<<"/"<<*ite<<"/*.cpp "<<std::endl;
			}
		}
		os<<R"(set(genSrcS)
file(GLOB genSrcS src/userLogic/*.cpp )"<<cppS.str()<<R"()
set(defS)
set(libPath)
set(libDep)
set(osDepLib)
if (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB defS src/win/*.def)

	include_directories( )";
	// auto depInc = rGlobalFile.depIncludeHome ();
	os<<")"<<std::endl;
	// auto depLib = rGlobalFile.depLibHome ();

	// os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()
	include_directories(
	)";
	// auto frameInPath = rGlobalFile.frameInstallPath ();
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
	add_library(${prjName} SHARED ${genSrcS} ${defS})
if (UNIX)
	MESSAGE(STATUS "unix add -fPIC")
	find_package(unofficial-libuuid CONFIG REQUIRED)
	target_compile_options(${prjName} PRIVATE -fPIC)	
    set(VERSION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/lib.vers")
	target_link_options(${prjName} PRIVATE -Wl,-version-script,${VERSION_SCRIPT})
	list(APPEND osDepLib unofficial::UUID::uuid)
endif ()

target_link_libraries(${prjName} PRIVATE
	common
	)"<<configClassName<<R"(
	)"<<appName<<R"(Lib 
	logicCommon
	frameConfig
	cLog
	comTcpProNet
	${osDepLib}
)";
	// auto bH = rGlobalFile.haveMsg();
	auto bH = pPmp;
	if (bH) {
		os<<R"(defMsg
	)";
	}
os<<R"(
	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	)";

    } while (0);
    return nRet;
}

int  appModuleGen:: genServer (serverFile& rServer)
{
    int  nRet = 0;
    do {
		int nR = 0;
		nR = onCreateChannelRetGen (rServer);
		if (nR) {
			nRet = 1;
			break;
		}
		nR = onWorkerInitGen (rServer);
		if (nR) {
			nRet = 2;
			break;
		}
		nR = genOnLoopBegin (rServer);
		if (nR) {
			nRet = 3;
			break;
		}
		nR = genOnLoopEnd (rServer);
		if (nR) {
			nRet = 4;
			break;
		}
		nR = genOnLoopFrame (rServer);
		if (nR) {
			nRet = 5;
			break;
		}
		nR = genPackFun (rServer);
		if (nR) {
			nRet = 6;
			break;
		}
    } while (0);
    return nRet;
}

int   appModuleGen:: onCreateChannelRetGen (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		create_directories (proc);
		proc += "/OnCreateChannelRet.cpp";
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

int   appModuleGen:: onWorkerInitGen (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		proc += "/onWorkerInit.cpp";
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

int   appModuleGen:: genOnLoopBegin (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		proc += "/onLoopBegin.cpp";
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

int   appModuleGen:: genOnLoopEnd(serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		proc += "/onLoopEnd.cpp";
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

int   appModuleGen:: genOnLoopFrame(serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		proc += "/onLoopFrame.cpp";
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

int appModuleGen::procMsgReg (serverFile* pServer, const procRpcNode& rProcRpc, const char* szMsgDir)
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

		std::string strMgrClassName = m_appData.appName ();
		strMgrClassName += "WorkerMgr";
		
		std::string strFile = szMsgDir;
		auto msgFunName = pMsg->msgFunName ();
		strFile += msgFunName;
		strFile += ".cpp";
		bool is = isPathExit (strFile.c_str ());
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
		// auto bH = rGlobalFile.haveServer ();
		// if (bH) {
			ps<<R"(#include "loopHandleS.h"
)";
		//}

ps<<R"(#include "logicWorker.h"
#include ")"<<strMgrClassName<<R"(.h")"<<std::endl<<R"(
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<strDec<<R"(
{
	gInfo("Rec )"<<msgFunName<<R"(");
}
)";
	} while (0);
	return nRet;
}


int   appModuleGen:: genPackFun (serverFile& rServer)
{
    int   nRet = 0;
    do {
		auto serverName = rServer.serverName ();
		std::string proc = srcDir ();
		proc += serverName;
		proc += "/";
		auto pServerF = &rServer;
		
		auto& rMap = pServerF->procMsgS ();
		using groupSet = std::set<std::string>;
		groupSet  groupS;
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		if (!pPmp) {
			break;
		}
        for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rMgr = pPmp->rpcFileS ();
			auto pRpc = rMgr.findRpc (it->rpcName.c_str ());
			myAssert (pRpc);
			auto pGName = pRpc->groupName ();
			groupS.insert (pGName);
        }
		
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
			
			std::string strCppDir = proc;
			
			strCppDir += pGName;
			strCppDir += "/";
			myMkdir (strCppDir.c_str ());
			procMsgReg (pServerF, rProcRpc, strCppDir.c_str ());
		} // for
    } while (0);
    return nRet;
}


