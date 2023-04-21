#include "main.h"
#include "msgTool.h"
#include "appMgr.h"
#include "rpcMgr.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/moduleFileiMgr.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/msgFileMgr.h"
#include "xmlGlobalLoad.h"
#include "xmlCommon.h"
#include "rLog.h"
#include "configMgr.h"
#include "myAssert.h"
#include "procGen/globalGen.h"

int main (int argNum, char* argS[])
{
	int nRet = 0;
	auto nInitLog = initLog ("genTool", "genTool.log", 0);
	do {
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			nRet = 3;
			break;
		}
		/*
		tSingleton<msgTool>::createSingleton ();
		tSingleton<appMgr>::createSingleton ();
		tSingleton<rpcMgr>::createSingleton();
		*/
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
		tSingleton<moduleFileiMgr>::createSingleton ();
		tSingleton<msgGroupFileMgr>::createSingleton ();
		tSingleton<msgFileMgr>::createSingleton ();
		tSingleton<xmlCommon>::createSingleton ();
		tSingleton<globalGen>::createSingleton ();
		
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
		// tSingleton<msgTool>::single ().init (argNum, argS);
	} while (0);
	return nRet;
}
