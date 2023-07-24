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
	beforeUnload	@2)";
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

extern "C"
{
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	void  beforeUnload();
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
#include "logicServer.h"
#include "tSingleton.h"
#include "cLog.h"

void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(nArgC, argS, pForLogic);
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
		fileName += "/logicServer.h";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		os<<R"(#ifndef _logicServer_h_
#define _logicServer_h_
#include <memory>
#include "mainLoop.h"

class logicServer
{
public:
	logicServer();
	~logicServer();
	int init(const char* name, ServerIDType id);
	ServerIDType id();
	const char*  name();
	int OnServerFrame();
private:
	std::unique_ptr<char[]>	 m_name;
	ServerIDType  m_id;
};


class logicServerMgr
{
public:
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
private:
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
		fileName += "/logicServer.cpp";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		os<<R"(#include <iostream>
#include <string>
#include "logicServer.h"
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

logicServer::logicServer()
{
}

logicServer::~logicServer()
{
}

int logicServer::init(const char* name, ServerIDType id)
{
	auto nameL = strlen(name);
	m_name = std::make_unique<char[]>(nameL + 1);
	strcpy(m_name.get(), name);
	m_id = id;
	return 0;
}

ServerIDType logicServer::id()
{
	return m_id;
}

const char*  logicServer::name()
{
	return m_name.get();
}

int logicServer::OnServerFrame()
{
	return 0;
}

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

void logicServerMgr::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
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
		for (auto it = vModelS.begin (); vModelS.end () != it; ++it) {
			std::string strDll = *it;
			gInfo ("will load "<<strDll.c_str ());
			auto hDll = loadDll (strDll.c_str());
			if (!hDll) {
				gError ("load dll fail fileName = "<<strDll.c_str());
				break;
			}
			auto onLoad = (afterLoadFunT)(getFun(hDll, "afterLoad"));
			if (!onLoad) {
				gError ("get fun afterLoad fail");
				break;
			}
			gInfo ("befor call onLoad");
			onLoad (nArgC, argS, pForMsg);
			gInfo ("after call onLoad");
		}
	} while (0);
})";
    } while (0);
    return nRet;
}

