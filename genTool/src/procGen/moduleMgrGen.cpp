#include "moduleMgrGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include "fromFileData/globalFile.h"
#include "tSingleton.h"
#include "rLog.h"
#include <string>
#include <fstream>

moduleMgrGen:: moduleMgrGen ()
{
}

moduleMgrGen:: ~moduleMgrGen ()
{
}

int   moduleMgrGen:: startGen (appFile& rApp)
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strDir = thisRoot ();
		strDir += "/src/gen";
		setGenSrcDir (strDir.c_str ());
		int nR = 0;
		std::string strWin = strDir;
		strWin += "/win";
		nR = myMkdir (strWin.c_str());
		// std::string strUnix = strDir;
		// strUnix += "/unix";
		// nR = myMkdir (strUnix.c_str());
		nR = writeCMakeLists (rApp);
		if (nR) {
			rError (" writeCMakeLists return error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = writeDefFile ();
		if (nR) {
			rError ("writeDefFile error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = writeExportFunH ();
		if (nR) {
			rError ("writeExportFunH error nR = "<<nR);
			nRet = 4;
			break;
		}
		nR = writeExportFunCpp ();
		if (nR) {
			rError ("writeExportFunCpp error nR = "<<nR);
			nRet = 5;
			break;
		}
		nR = writeLogicServerH ();
		if (nR) {
			rError ("writeLogicServerH error nR = "<<nR);
			nRet = 6;
			break;
		}
		nR = writeLogicServerCpp ();
		if (nR) {
			rError ("writeLogicServerCpp error nR = "<<nR);
			nRet = 7;
			break;
		}
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeCMakeLists (appFile& rApp)
{
	int   nRet = 0;
	do {
		std::string fileName = thisRoot ();
		fileName += "/CMakeLists.txt";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		// auto installPath = rGlobalFile.installPath ();
		std::string mgrName = rApp.appName ();
		mgrName += "ModuleMgr";
os<<"SET(prjName "<<mgrName<<")"<<std::endl;
	// <<"SET(CMAKE_INSTALL_PREFIX "<<installPath<<")"<<std::endl;
		const char* szCon = R"(set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp)
set(defS)
set(libPath)
set(libDep)
if (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB defS src/gen/win/*.def)
	include_directories()";
	auto depInc = rGlobalFile.depIncludeHome ();
	os<<szCon<<depInc<<")"<<std::endl;
	auto depLib = rGlobalFile.depLibHome ();

	os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()

	include_directories()";
	auto framePath = rGlobalFile.frameHome ();
	os<<szC2<<std::endl
	<<"    ${CMAKE_SOURCE_DIR}/defMsg/src"<<std::endl
	<<"    "<<framePath<<"common/src"<<std::endl
	<<"    "<<framePath<<"logicCommon/src"<<std::endl
	<<"    "<<framePath<<"cLog/src"<<std::endl
	<<")"<<std::endl;
	auto libPath = rGlobalFile.frameLibPath ();
	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl;

	const char* szC3 = R"(link_directories(${libPath} ${libDep})
	add_library(${prjName} SHARED ${genSrcS} ${defS})
	target_link_libraries(${prjName} PUBLIC
	common
	logicCommon
	cLog
	defMsg
	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	)";
	os<<szC3;
	// SET(LIBRARY_OUTPUT_PATH )"<<outPath<<R"())";
	} while (0);
    return nRet;
}

const char*  moduleMgrGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  moduleMgrGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}

int   moduleMgrGen:: writeDefFile ()
{
    int   nRet = 0;
    do {
		std::string strDef = genSrcDir();
		strDef += "/win/mgr.def";
		std::ofstream os(strDef.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strDef.c_str ());
			break;
		}
		os<<R"(LIBRARY
EXPORTS
	afterLoad	@1
	logicOnAccept	@2
	logicOnConnect  @3
	onLoopBegin		@4
	onLoopEnd		@5
	beforeUnload	@6)";
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeExportFunH ()
{
    int   nRet = 0;
    do {
		std::string strExport = genSrcDir ();
		strExport += "/exportFun.h";
		std::ofstream os(strExport.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strExport.c_str ());
			break;
		}
		os<<R"(#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"
class logicServer;
extern "C"
{
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	void  beforeUnload();

	void onLoopBegin	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
}
#endif)";
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeExportFunCpp ()
{
    int   nRet = 0;
    do {
		std::string strExport = genSrcDir ();
		strExport += "/exportFun.cpp";
		std::ofstream os(strExport.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strExport.c_str ());
			break;
		}
		os<<R"(#include <iostream>
#include "exportFun.h"
#include "myAssert.h"
#include "allLogicServerMgr.h"
#include "tSingleton.h"
#include "cLog.h"

void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	tSingleton<allLogicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	rMgr.afterLoad(nArgC, argS, pForLogic);
}
void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.logicOnAccept(fId, sessionId, userData);
}

void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.logicOnConnect(fId, sessionId, userData);
}

void onLoopBegin	(serverIdType	fId)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.onLoopBegin	(fId);
}

void onLoopEnd	(serverIdType	fId)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.onLoopEnd(fId);
}

void  beforeUnload()
{
})";
    } while (0);
    return nRet;
}

const char*  moduleMgrGen:: genSrcDir ()
{
    return m_genSrcDir.get ();
}

void  moduleMgrGen:: setGenSrcDir (const char* v)
{
    strCpy (v, m_genSrcDir);
}

int   moduleMgrGen:: writeLogicServerH ()
{
    int   nRet = 0;
	do {
		std::string fileName = genSrcDir ();
		fileName += "/allLogicServerMgr.h";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		os<<R"(#ifndef _allLogicServerMgr_h_
#define  _allLogicServerMgr_h_
#include <memory>
#include "mainLoop.h"
#include <string>
#include <map>

class logicServer;
class allLogicServerMgr
{
public:
	// typedef logicServer* (*findServerFT)(serverIdType);
	struct moduleInfo
	{
		void* handle;
		afterLoadFunT fnAfterLoadFun;
		logicOnAcceptFT  fnLogicOnAccept;
		logicOnConnectFT  fnLogicOnConnect;
		beforeUnloadFT fnBeforeUnload;
		onLoopBeginFT  fnOnLoopBegin;
		onLoopEndFT  fnOnLoopEnd;
		// findServerFT   fnFindServer;
	};
	using moduleMap = std::map<std::string, moduleInfo>;
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	// logicServer*  findServer (serverIdType	sId);
	moduleMap&  moduleS ();
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);

	void onLoopBegin	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
private:
	moduleMap  m_moduleS;
	ForLogicFun m_ForLogicFun;
};
#endif)";
	} while (0);
    return nRet;
}

int   moduleMgrGen:: writeLogicServerCpp ()
{
    int   nRet = 0;
    do {
		std::string fileName = genSrcDir ();
		fileName += "/allLogicServerMgr.cpp";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		os<<R"(#include <iostream>
#include <string>
#include "allLogicServerMgr.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
#include "loopHandleS.h"
#include "strFun.h"
#include "comFun.h"
#include "rpcInfo.h"
#include "modelLoder.h"
#include <vector>

void getModelS (int nArgC, const char* argS[], std::vector<std::string>& vModelS)
{
	char* pRetBuf[3];
	for (int i = 1; i < nArgC; i++) {
		auto pa = argS [i];
		auto nL = strlen (pa);
		auto pB = std::make_unique<char[]> (nL + 1);
		auto pBuf = pB.get ();
		strNCpy (pBuf, nL + 1, pa);
		auto nNum = strR(pBuf, '=', pRetBuf, 3);
		if (2 != nNum) {
			gWarn("arg num error"<< "2 != nNum = " << nNum);
			continue;
		}
		if (strcmp (pRetBuf[0], "logicModel") == 0) {
			vModelS.push_back (pRetBuf[1]);
		}
	}
}

allLogicServerMgr::moduleMap& allLogicServerMgr:: moduleS ()
{
	return m_moduleS;
}
/*
logicServer*  allLogicServerMgr::findServer (serverIdType	sId)
{
	logicServer* pRet = nullptr;
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		pRet = it->second.fnFindServer (sId);
		if(pRet) {
			break;
		}
	}
	return pRet;
}
*/
void allLogicServerMgr::logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnLogicOnAccept(fId, sessionId, userData);
	}
}

void allLogicServerMgr::logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnLogicOnConnect(fId, sessionId, userData);
	}
}

void allLogicServerMgr::onLoopBegin	(serverIdType	fId)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnOnLoopBegin(fId);
	}
}

void allLogicServerMgr::onLoopEnd(serverIdType	fId)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnOnLoopEnd(fId);
	}
}

void allLogicServerMgr::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	m_ForLogicFun = *pForLogic;
	auto pForMsg = &m_ForLogicFun;
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	regRpcS (&rFunS);

	std::vector<std::string> vModelS;
	getModelS (nArgC, argS, vModelS);
	gInfo ("logicModelNum = "<<vModelS.size());
	do {
		auto& modS = moduleS ();
		for (auto it = vModelS.begin (); vModelS.end () != it; ++it) {
			std::string strDll = *it;
			gInfo ("will load "<<strDll.c_str ());
			moduleInfo info;
			info.handle = loadDll (strDll.c_str());
			if (!info.handle) {
				gError ("load dll fail fileName = "<<strDll.c_str());
				break;
			}
			info.fnAfterLoadFun = (afterLoadFunT)(getFun(info.handle, "afterLoad"));
			if (!info.fnAfterLoadFun) {
				gError ("get fun afterLoad fail");
				break;
			}
			info.fnLogicOnAccept= (logicOnAcceptFT)(getFun(info.handle, "logicOnAccept"));
			if (!info.fnLogicOnAccept) {
				gError ("get fun fnLogicOnAccept fail");
				break;
			}
			info.fnLogicOnConnect = (logicOnConnectFT)(getFun(info.handle, "logicOnConnect"));
			if (!info.fnLogicOnConnect) {
				gError ("get fun fnLogicOnConnect fail");
				break;
			}
			info.fnBeforeUnload = (beforeUnloadFT)(getFun(info.handle, "beforeUnload"));
			if (!info.fnBeforeUnload) {
				gError ("get fun fnFindServer fail");
				break;
			}
			info.fnOnLoopBegin = (onLoopBeginFT)(getFun(info.handle, "onLoopBegin"));
			if (!info.fnOnLoopBegin) {
				gWarn ("fun onLoopBegin empty error is");
				break;
			}
			info.fnOnLoopEnd = (onLoopEndFT)(getFun(info.handle, "onLoopEnd"));
			if (!info.fnOnLoopEnd) {
				gWarn ("fun onLoopEnd empty error is");
				break;
			}
			gInfo ("befor call onLoad");
			info.fnAfterLoadFun(nArgC, argS, pForMsg);
			gInfo ("after call onLoad");
			auto inRet = modS.insert(std::make_pair(strDll, info));
			myAssert(inRet.second);
		}
	} while (0);
})";
    } while (0);
    return nRet;
}

