#include "globalGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/appFileMgr.h"
#include "tSingleton.h"
#include "appGen.h"
#include "defMsgGen.h"
#include "projectCMakeListGen.h"
#include "rLog.h"

globalGen:: globalGen ()
{
}

globalGen:: ~globalGen ()
{
}

int   globalGen:: startGen ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		nR = makePath ();
		projectCMakeListGen  projectGen;
		nR = projectGen.startGen ();
		if (nR) {
			rError ("projectGen.startGen error nR = "<<nR);
			break;
		}
		defMsgGen msgGen;
		nR = msgGen.startGen ();
		if (nR) {
			rError ("defMsg gen error nR = "<<nR);
			break;
		}
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());

			appGen gen;
			// rInfo ("will proc app: "<<it->second->appName ());
			nR = gen.startGen (rApp);
			if (nR) {
				nRet = 2;
				rError ("appGen error nR = "<<nR<<" appName = "<<it->first.c_str());
				break;
			}
		}
		if (nRet) {
			break;
		}
		nR = msgGen.rpcInfoCppGen ();
		if (nR) {
			rError ("rpcInfoCppGen gen error nR = "<<nR);
			break;
		}
    } while (0);
    return nRet;
}

int   globalGen:: secondGen ()
{
    int   nRet = 0;
    do {
    } while (0);
    return nRet;
}

int   globalGen:: makePath ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton <globalFile>::single ();
		auto szProject = rGlobalFile.projectHome ();
		// rInfo ("will mkdir : "<<szProject);
		nRet = myMkdir (szProject);
    } while (0);
    return nRet;
}

