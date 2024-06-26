#include "globalGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/appFileMgr.h"
#include "protobufSerGen.h"
#include "tSingleton.h"
#include "appGen.h"
#include "msgGen.h"
#include "defMsgGen.h"
#include "projectCMakeListGen.h"
#include "rLog.h"
#include <vector>

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
			nRet = 1;
			break;
		}
		nR = configLibGen ();
		if (nR) {
			rError ("projectGen.configLibGen error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = 0;
		auto &rGlobal = tSingleton<globalFile>::single();
		auto bH = rGlobal.haveServer ();
		std::vector <std::shared_ptr<msgGen>> gv;
		if (bH) {
			protobufSerGen  protoGen;
			nR = protoGen.startGen ();
			if (nR) {
				rError ("projectGen.startGen error nR = "<<nR);
				nRet = 4;
				break;
			}
			auto &rPmpS = rGlobal.msgFileS ();
			for (auto it = rPmpS.begin(); rPmpS.end() != it; ++it) {
				auto& rPmp = *(it->second.get());
				auto pP = std::make_shared <msgGen> (rPmp);
				msgGen  &gen = *pP;
				nR = gen.startGen ();
				gv.push_back (pP);
				if (nR) {
					break;
				}
			}
			if (nR) {
				nRet = 5;
				rError ("msg gen error nRet = "<<nR);
				break;
			}
			defMsgGen msgGenD;
			nR = msgGenD.startGen ();
			if (nR) {
				rError ("defMsg gen error nR = "<<nR);
				nRet = 6;
				break;
			}
		}
		
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());

			appGen gen;
			// rInfo ("will proc app: "<<it->second->appName ());
			nR = gen.startGen (rApp);
			if (nR) {
				nRet = 7;
				rError ("appGen error nR = "<<nR<<" appName = "<<it->first.c_str());
				break;
			}
		}
		if (nRet) {
			break;
		}
		nR = 0;
		for (auto it = gv.begin(); gv.end() != it; ++it) {
			msgGen&  gen = *(it->get());
			nR = gen.rpcInfoCppGen ();
			if (nR) {
				break;
			}
		}
		if (nR) {
			rError ("rpcInfoCppGen gen error nR = "<<nR);
			break;
		}
    } while (0);
    return nRet;
}

int   globalGen:: configLibGen ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto configDef = rGlobalFile.configDef ();
		std::string strFile = rGlobalFile.xmlDir ();
		strFile += "/";
		std::stringstream cmdOs;
		cmdOs<<"configFile ";
		if (configDef) {
			strFile += configDef;
			auto bE = isPathExit (strFile.c_str());
			myAssert (bE);
			cmdOs<<"file="<<strFile<<" ";
		}
		auto projectDir = rGlobalFile.projectHome ();
		// auto projectName = rGlobalFile.projectName ();
		auto  strPrjConfig = rGlobalFile.configClassName ();
		cmdOs<<"projectDir="<<projectDir<<" projectName="<<strPrjConfig<<" className="<<strPrjConfig;
		system (cmdOs.str().c_str());
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

