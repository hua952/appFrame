#include "exportFunCrector.h"
#include <fstream>

int writeExportFunH (const char* szFilename)
{
	int nRet = 0;

	std::ofstream os(szFilename);
	do {
		if (!os) {
			nRet = 1;
			break;
		}
		const char* szCon = R"(#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(const ForLogicFun* pForLogic);
	void  beforeUnload();
}
#endif)";
		os<<szCon;
	} while (0);
	return nRet;
}

int writeExportFunCpp (const char* szFilename)
{
	int nRet = 0;

	std::ofstream os(szFilename);
	do {
		if (!os) {
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

void  afterLoad(const ForLogicFun* pForLogic)
{
	tSingleton<logicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<logicServerMgr>::single();
	rMgr.afterLoad(pForLogic);
}

void  beforeUnload()
{
	std::cout<<"In   beforeUnload"<<std::endl;
}

ForMsgModuleFunS& getForMsgModuleFunS()
{
	static ForMsgModuleFunS s_ForMsgModuleFunS;
	return  s_ForMsgModuleFunS;
})";
		os<<szCon;
	} while (0);

	return nRet;
}
