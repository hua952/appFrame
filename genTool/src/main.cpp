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

		tSingleton<msgTool>::createSingleton ();
		tSingleton<appMgr>::createSingleton ();
		tSingleton<rpcMgr>::createSingleton();
		tSingleton<globalFile>::createSingleton();
		tSingleton<xmlGlobalLoad>::createSingleton();
		tSingleton<appFileMgr>::createSingleton ();
		tSingleton<moduleFileiMgr>::createSingleton ();
		tSingleton<msgGroupFileMgr>::createSingleton ();
		tSingleton<msgFileMgr>::createSingleton ();
		tSingleton<xmlCommon>::createSingleton ();
		tSingleton<msgTool>::single ().init (argNum, argS);
	} while (0);
	return nRet;
}
