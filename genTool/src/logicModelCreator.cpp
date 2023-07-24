#include "logicModelCreator.h"
#include "appMgr.h"
#include "msgTool.h"
#include "rLog.h"

logicModelCreator::logicModelCreator ()
{
}
logicModelCreator::~logicModelCreator ()
{
}
int logicModelCreator:: start (logicModel* pModel)
{
	m_pModel = pModel;
	rInfo (" start Process ModelMgr");
	int nRet = 0;
	writeCMakeFile ();
	this->writeExportFunH ();
	this->writeExportFunCpp ();
	this->writeLogicServerH ();
	this->writeLogicServerCpp ();
	this->writeDef ();
	return nRet;
}

int logicModelCreator::writeExportFunCpp()
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
	os<<R"(#include "exportFun.h"
#include "logicServerMgr.h"
#include "tSingleton.h"
#include "msg.h"
#include <iostream>
#include <gLog.h>

void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	auto pFun = getLogMsgFun ();
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
})";
	return nRet;
}

int logicModelCreator:: writeExportFunH ()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
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

int logicModelCreator::writeCMakeFile ()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	myMkdir (strFilename.c_str ());
	strFilename += "/CMakeLists.txt";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	std::string strMgrFullName = m_pModel->name ();
	//strMgrFullName += "LogicModelMgr";
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

int logicModelCreator:: writeDef ()
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

int  logicModelCreator:: writeLogicServerH()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/logicServer.h";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	os<<R"(#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	void  afterLoad(ForLogicFun* pForLogic);
private:
};
#endif)";
	return nRet;
}

int  logicModelCreator:: writeLogicServerCpp()
{
	int nRet = 0;
	std::string strFilename;	
	getLogicMgrPath (strFilename);
	strFilename += "/src/gen/logicServer.cpp";
	rInfo ("will write : "<<strFilename.c_str ());
	std::ofstream os(strFilename);
	os<<R"(#include "logicServerMgr.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include "loopHandleS.h"

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

void  logicServerMgr::afterLoad(ForLogicFun* pForLogic)
{
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
})";
	return nRet;
}

int  logicModelCreator::getLogicMgrPath (std::string& strPath)
{
	int nRet = 0;
	auto& rTool = tSingleton<msgTool>::single();
	strPath = rTool.projectHome ();
	strPath += m_pModel->name();
	strPath += "LogicModelMgr";
	return nRet;
}

