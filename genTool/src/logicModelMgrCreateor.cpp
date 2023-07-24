#include "cLog.h"
#include "logicModelMgrCreateor.h"
#include "logicModelCreator.h"
#include <sstream>
#include <memory>
#include "appMgr.h"
#include "msgTool.h"
#include "rLog.h"

logicModelMgrCreateor::logicModelMgrCreateor (realServer& rApp):m_rApp (rApp)
{
}

realServer&  logicModelMgrCreateor::getApp ()
{
	return m_rApp;
}

int logicModelMgrCreateor:: start ()
{
	rInfo (" start Process ModelMgr");
	int nRet = 0;
	writeCMakeFile ();
	this->writeExportFunH ();
	this->writeExportFunCpp ();
	this->writeLogicServerH ();
	this->writeLogicServerCpp ();
	this->writeDef ();
	auto& rRealServer = getApp ();
	auto& rMap = rRealServer.getModelMap ();
	for (auto it = rMap.begin (); rMap.end () != it; ++it) {
		auto pModel = std::make_unique<logicModelCreator> ();
		pModel->start (it->second.get());
	}
	return nRet;
}

int  logicModelMgrCreateor::writeExportFunCpp()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/";
	myMkdir (strFilename.c_str ());
	strFilename += "exportFun.cpp";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	auto& rTool = tSingleton<msgTool>::single ();
	os<<R"(#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "CChessMsgID.h"
#include "myAssert.h"
#include "CChessRpc.h"
#include "logicServer.h"
#include "tSingleton.h"
#include "cLog.h"

void  afterLoad (int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(nArgC, argS, pForLogic);
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
})";
	return nRet;
}

int logicModelMgrCreateor:: writeExportFunH ()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	auto& rApp = getApp ();
	strFilename += "/src/gen/";
	myMkdir (strFilename.c_str ());
	strFilename += "exportFun.h";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	os<<R"(#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	void  beforeUnload();
}
#endif)";
	return nRet;
}

int  logicModelMgrCreateor::writeCMakeFile ()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	myMkdir (strFilename.c_str ());
	strFilename += "/CMakeLists.txt";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	auto& rApp = getApp ();
	std::string strMgrFullName = rApp.name ();
	strMgrFullName += "LogicModelMgr";
	auto& rTool = tSingleton<msgTool>::single ();
	auto outPutPath = rTool.outPutPath ();
	auto depIncludeHome = rTool.depIncludeHome ();
	auto depLibHome = rTool.depLibHome ();
	auto frameHome = rTool.frameHome ();
	auto frameLibPath = rTool.frameLibPath ();
		os<<"SET(prjName "<<strMgrFullName.c_str ()<<")"<<std::endl
		<<"SET(depIncludeHome "<<depIncludeHome<<")"<<std::endl
		<<"SET(depLibHome "<<depLibHome<<")"<<std::endl
		<<"SET(frameHome "<<frameHome<<")"<<std::endl
		<<"SET(frameLibPath "<<frameLibPath<<")"<<std::endl
		<<"SET(outPutPath "<<outPutPath<<")"<<std::endl
		<<R"(
set(srcS)
set(genSrcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(srcOS)
if (UNIX)
    MESSAGE(STATUS "unix")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/gen/*.def)
	file(GLOB genSrcS src/gen/*.cpp)
	file(GLOB srcOS src/win/*.cpp)
	include_directories(
		${depIncludeHome}
		)
	link_directories(
		${depLibHome}
		)
endif ()
add_library(${prjName} SHARED ${genSrcS} ${srcS} ${srcOS} ${defS})
target_include_directories(${prjName} PUBLIC 
							src
							${frameHome}/common/src
							${frameHome}/testLogicMsg/src
							${frameHome}/logicCommon/src
							${frameHome}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common logicCommon defMsg)
SET(LIBRARY_OUTPUT_PATH ${outPutPath}))";
	return nRet;
}

int  logicModelMgrCreateor:: writeDef ()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/exportFun.def";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	os<<R"(LIBRARY
EXPORTS
	afterLoad	@1
	beforeUnload	@2)";
	return nRet;
}

int logicModelMgrCreateor:: writeLogicServerH()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/logicServer.h";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
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
	return nRet;
}

int logicModelMgrCreateor:: writeLogicServerCpp()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/logicServer.cpp";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
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
	m_ForLogicFun = *pForLogic;
	regRpcS (&rFunS);
	std::unique_ptr<char[]> myPath;
	auto nGet = getCurModelPath (myPath);
	std::string strBase = myPath.get();

	std::vector<std::string> vModelS;
	getModelS (nArgC, argS, vModelS);
	gInfo ("logicModelNum = "<<vModelS.size());
	do {
		for (auto it = vModelS.begin (); vModelS.end () != it; ++it) {
			gInfo ("will load "<<it->c_str ());
			std::string strDll = strBase;
			strDll += *it;
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
			onLoad (nArgC, argS, pForMsg);
		}
	} while (0);
})";
	return nRet;
}

int logicModelMgrCreateor::getLogicMgrPath (std::string& strPath)
{
	int nRet = 0;
	auto& rApp = getApp ();
	auto& rTool = tSingleton<msgTool>::single();
	strPath = rTool.projectHome ();
	strPath += rApp.name();
	strPath += "LogicModelMgr";
	return nRet;
}

