#include "moduleExportFunGen.h"
#include "strFun.h"
#include "fromFileData/moduleFile.h"
#include "rLog.h"
#include <fstream>
#include "moduleGen.h"

moduleExportFunGen:: moduleExportFunGen ()
{
}

moduleExportFunGen:: ~moduleExportFunGen ()
{
}

int  moduleExportFunGen:: startGen (moduleGen& rMod)
{
	int nRet = 0;
	do {
		int nR = 0;
		auto& rData = rMod.moduleData ();
		auto modName = rData.moduleName ();
		nR = genH (rMod);
		if (nR) {
			rError (" moduleExportFunGen write h file error modName = "<<modName<<" nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = genCpp (rMod);
		if (nR) {
			rError (" moduleExportFunGen write cpp file error modName = "<<modName<<" nR = "<<nR);
			nRet = 2;
			break;
		}
	} while (0);
	return nRet;
}

int   moduleExportFunGen:: genH (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		std::string strFile = rMod.genPath ();
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
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
	void onLoopBegin	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	void  beforeUnload();
}
#endif)";
		os<<szCon;
		} while (0);
return nRet;
}

int   moduleExportFunGen:: genCpp (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto& rData = rMod.moduleData ();
		auto pModName = rData.moduleName ();
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";

		std::string strFile = rMod.genPath ();
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
#include "logicServer.h"
#include "tSingleton.h"
#include "gLog.h"
#include ")"<<strMgrClassName<<R"(.h"

dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	setForMsgModuleFunS (pForLogic);
	tSingleton<)"<<strMgrClassName<<R"(>::createSingleton();
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	return rMgr.afterLoad(nArgC, argS, pForLogic);
}

void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->logicOnAcceptSession(sessionId, userData);
	}
}

void onLoopBegin	(serverIdType	fId)
{
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->onLoopBegin	();
	}
}

void onLoopEnd	(serverIdType	fId)
{
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->onLoopEnd();
	}
}

void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<)"<<strMgrClassName<<R"(>::single();
	auto pS = rMgr.findServer(fId);
	if (pS) {
		pS->logicOnConnect(sessionId, userData);
	}
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}
)";
	} while (0);
return nRet;
}

