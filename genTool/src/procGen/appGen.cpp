#include "appGen.h"
#include "appCMakeListsGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include <string>
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFileiMgr.h"
#include "moduleMgrGen.h"
#include "rLog.h"
#include "moduleGen.h"

appGen:: appGen ()
{
}

appGen:: ~appGen ()
{
}

int  appGen:: startGen (appFile& rApp)
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string projectHome = rGlobalFile.projectHome ();
		std::string strAppPath = projectHome;
		auto szAppName = rApp.appName ();
		strAppPath += szAppName;
		rInfo ("will mkdir :"<<strAppPath.c_str ());
		int nR = myMkdir (strAppPath.c_str ());
		appCMakeListsGen cmakeGen;
		nR = cmakeGen.startGen (rApp);
		if (nR) {
			nRet = 2;
			rError ("cmakeGen.startGen ret error nR = "<<nR);
			break;
		}
		moduleMgrGen   mgrGen;
		nR = mgrGen.startGen (rApp);
		if (nR) {
			rError ("moduleMgrGen startGen error nR = "<<nR);
			nRet = 3;
			break;
		}
		auto& rModules = rApp.moduleFileNameS ();
		auto& rModMgr = tSingleton <moduleFileiMgr>::single ();
		for (auto it = rModules.begin (); rModules.end () != it; ++it) {
			auto pMod = rModMgr.findModule (it->c_str ());
			if (pMod) {
				moduleGen   gen (*pMod);
				nR = gen.startGen ();
				if (nR) {
					nRet = 3;
					break;
				}
			} else {
				rError ("can not find module : "<<it->c_str ());
				nRet = 4;
				break;
			}
		}
		if (nRet) {
			break;
		}
    } while (0);
    return nRet;
}

