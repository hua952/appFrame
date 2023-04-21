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
	void  afterLoad(ForLogicFun* pForLogic);
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
		std::string strFile = rMod.genPath ();
		strFile += "/exportFun.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file for write error fileName = "<<strFile.c_str ());
			nRet = 1;
			break;
		}

		const char* szCon = R"(#include <iostream>
#include <cstring>
#include <string>
#include "exportFun.h"
#include "msg.h"
#include "myAssert.h"
#include "logicServer.h"
#include "tSingleton.h"
#include "gLog.h"

void  afterLoad(ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}
)";
		os<<szCon;
		} while (0);
return nRet;
}

