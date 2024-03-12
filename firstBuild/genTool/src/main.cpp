#include "main.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/structFileMgr.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/toolServerEndPointInfoMgr.h"
#include "xmlGlobalLoad.h"
#include "xmlCommon.h"
#include "rLog.h"
#include "configMgr.h"
#include "myAssert.h"
#include "procGen/globalGen.h"

int main (int argNum, char* argS[])
{
	int nRet = 0;
	initLogGlobal ();
	auto nInitLog = initLog ("genTool", "logs/genTool.log", 0);
	do {
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			nRet = 3;
			break;
		}
		
		tSingleton <configMgr>::createSingleton ();
		auto& rConfig = tSingleton <configMgr>::single ();
		auto nR = rConfig.procArgS (argNum, argS);
		if (nR) {
			rError ("rConfig.procArgS ret error nR = "<<nR);
			nRet = 4;
			break;
		}
		tSingleton<globalFile>::createSingleton();
		tSingleton<xmlGlobalLoad>::createSingleton();
		tSingleton<appFileMgr>::createSingleton ();
		tSingleton<moduleFileMgr>::createSingleton ();
		
		tSingleton<xmlCommon>::createSingleton ();
		tSingleton<globalGen>::createSingleton ();
		tSingleton<toolServerEndPointInfoMgr>::createSingleton ();
		
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto& rXmlGlobal = tSingleton <xmlGlobalLoad>::single ();
		auto defFile = rConfig.defFile ();
		nR = rXmlGlobal.xmlLoad (defFile);
		if (nR) {
			rError ("xmlLoad return error nR = "<<nR);
			nRet = 5;
			break;
		}
		auto& rGlobalGen = tSingleton <globalGen>::single ();
		nR = rGlobalGen.startGen ();
		if (nR) {
			nRet = 6;
			rError ("rGlobalGen.startGen return error nR = "<<nR);
			break;
		}
		nR = rGlobalGen.secondGen ();
		if (nR) {
			nRet = 7;
			rError ("rGlobalGen.secondGen return error nR = "<<nR);
			break;
		}
	} while (0);
	std::cout<<" tool will end nRet = "<<nRet<<std::endl;
	return nRet;
}
