#include "appGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include "fromFileData/appFileMgr.h"
#include <string>
#include <fstream>
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFileMgr.h"
// #include "moduleMgrGen.h"
#include "rLog.h"
#include "moduleGen.h"
#include "mainGen.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/serverFile.h"

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
		setThisRoot (strAppPath.c_str ());
		// rInfo ("will mkdir :"<<strAppPath.c_str ());
		int nR = myMkdir (strAppPath.c_str ());
		// appCMakeListsGen cmakeGen;
		// nR = cmakeGen.startGen (rApp);
		nR = CMakeListGen (rApp);
		rInfo (""<<szAppName<<" cmake file gen OK nR = "<<nR);
		if (nR) {
			nRet = 2;
			rError ("cmakeGen.startGen ret error nR = "<<nR);
			break;
		}
		/*
		moduleMgrGen   mgrGen;
		std::string mgrName = thisRoot ();
		
		mgrName += "/";
		mgrName += szAppName;
		mgrName += "ModuleMgr";

		mgrGen.setThisRoot (mgrName.c_str ());
		nR = mgrGen.startGen (rApp);
		if (nR) {
			rError ("moduleMgrGen startGen error nR = "<<nR);
			nRet = 3;
			break;
		}
		*/
		nR = batFileGen (rApp);
		if (nR) {
			rError ("moduleMgrGen batFileGen error nR = "<<nR);
			nRet = 4;
			break;
		}
		auto& rModules = rApp.moduleFileNameS ();
		auto& rModMgr = tSingleton <moduleFileMgr>::single ();
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
		auto dS = rApp.detachServerS ();
		if (dS) {
			mainGen mg;
			nR = mg.startGen (rApp);
		}
    } while (0);
    return nRet;
}

int  appGen:: batFileGen (appFile& rApp)
{
    int  nRet = 0;
    do {
		auto szAppName = rApp.appName ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strInsHome = rGlobalFile.projectInstallDir ();
		std::string strInstall = strInsHome;
		myMkdir (strInstall.c_str ());
		strInstall += "/run_";
		strInstall += szAppName;
		strInstall += ".bat";
		std::ofstream os (strInstall.c_str ());
		if (!os) {
			rError ("open file error fileName = "<<strInstall.c_str ());
			break;
		}
		std::string strBinDir = strInsHome;
		strBinDir += "/bin/";
		auto procId = rApp.procId ();
		auto& rMainArgS = rApp.mainArgS();

		auto& rMap = tSingleton<appFileMgr>::single ().appS ();
		
		auto haveServer = rGlobalFile.haveServer ();
		if (haveServer) {
			std::stringstream ts;
			ts<<"procId="<<procId;
			rMainArgS.push_back(ts.str());
		
			std::stringstream appSS;
			appSS<<"runWorkNum="<<rApp.runWorkNum ();
			rMainArgS.push_back(appSS.str());
		}

		auto netType = rApp.netType ();
		if (appNetType_gate == netType) {
			rMainArgS.push_back("ip=0.0.0.0");
		}
		
		{
			std::stringstream ts;
			ts<<"netNum="<<rGlobalFile.netNum();
			rMainArgS.push_back(ts.str());
		}
		{
			std::stringstream ts;
			uword uwT = rApp.netType ();
			ts<<"appNetType="<<uwT;
			rMainArgS.push_back(ts.str());
		}
		std::string strLogFile = szAppName;
		strLogFile += ".log";
		std::string strLogFull = R"(logFile=)";
		strLogFull += strLogFile;
		rMainArgS.push_back(strLogFull);
		std::string level0Full = "level0=cppLevel0L ";
		rMainArgS.push_back(level0Full);
		std::string frameHomeFull = "frameHome=";
		auto aFreeHome = rGlobalFile.frameInstallPath();
		frameHomeFull += aFreeHome;
		os<<"cppLevel0.exe ";
		os<<strLogFull<<" "<<frameHomeFull;
		rMainArgS.push_back(frameHomeFull);
	
		os<<"procId="<<procId
			<<" logFile="<<strLogFile;

		auto& rModules = rApp.moduleFileNameS ();
		if (!rModules.empty()) {
			std::stringstream ssModelS;
			ssModelS<<"modelS="<<*(rModules.begin ());
			auto& rModMgr = tSingleton <moduleFileMgr>::single ();
			for (auto it = rModules.begin (); rModules.end () != it; ++it) {
				std::stringstream ts;
				ts<<" logicModel="<<*it;
				os<<ts.str();
				ssModelS<<"*";
				auto pM = rModMgr.findModule (it->c_str());
				myAssert (pM);
				auto& rSS = pM->orderS ();
				for (auto ite = rSS.begin (); ite != rSS.end (); ite++) {
					auto& pS = *ite;
					auto autoRun = (int)(pS->autoRun());
					auto route = (int)(pS->route());
					if (rSS.begin() != ite) {
						ssModelS<<"+";
					}
					ssModelS<<pS->tmpNum ()<<"-"<<pS->openNum ()<<"-"<<autoRun<<"-"<<route;
				}
			}
			rMainArgS.push_back(ssModelS.str());
		}
		auto& rV = rApp.argS ();
		for (auto it = rV.begin(); rV.end() != it; ++it) {
			os<<" "<<*it;
			rMainArgS.push_back(*it);
		}
		{
			auto& rV = rGlobalFile.argS ();
			for (auto it = rV.begin(); rV.end() != it; ++it) {
				os<<" "<<*it;
				rMainArgS.push_back(*it);
			}
		}
    } while (0);
    return nRet;
}

int  appGen:: CMakeListGen (appFile& rApp)
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strFile = thisRoot ();
		strFile += "/CMakeLists.txt";

		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strFile.c_str ());
			break;
		}
		std::string strMgr = rApp.appName ();
		strMgr += "ModuleMgr";
		auto szPrjName = rGlobalFile.projectName ();
		os<<R"(cmake_minimum_required(VERSION 3.16) 
set(BUILD_USE_64BITS on)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add_subdirectory ()"<<strMgr<<R"()
)";
		auto ds = rApp.detachServerS ();
		if (ds) {
			os<<R"(add_subdirectory (main))"<<std::endl;
		}
		auto& moduleS =  rApp.moduleFileNameS ();
		for (auto it = moduleS.begin (); moduleS.end () != it; ++it) {
			os<<R"(add_subdirectory ()"<<*it<<")"<<std::endl;
		}

    } while (0);
    return nRet;
}

const char*  appGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  appGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}

