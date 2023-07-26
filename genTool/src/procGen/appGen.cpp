#include "appGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include <string>
#include <fstream>
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "moduleMgrGen.h"
#include "rLog.h"
#include "moduleGen.h"
#include "moduleMgrGen.h"

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
    } while (0);
    return nRet;
}

int  appGen:: batFileGen (appFile& rApp)
{
    int  nRet = 0;
    do {
		// bin\Debug\cppLevel0.exe level0=cppLevel0L.dll addLogic=logicModelMgr.dll logicModel=testLogic.dll logicModel=testLogicB.dll procId=0 netLib=libeventSession.dll
		auto szAppName = rApp.appName ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strInsHome;
		rGlobalFile.getRealInstallPath (strInsHome);
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
		os<<"cppLevel0.exe level0=cppLevel0L.dll netLib=libeventSession.dll addLogic="
			<<strBinDir<<szAppName<<"ModuleMgr.dll procId="<<procId;
		auto& rModules = rApp.moduleFileNameS ();
		auto& rModMgr = tSingleton <moduleFileMgr>::single ();
		for (auto it = rModules.begin (); rModules.end () != it; ++it) {
			os<<" logicModel="<<strBinDir<<*it<<".dll";
		}
		auto& rV = rApp.argS ();
		for (auto it = rV.begin(); rV.end() != it; ++it) {
			os<<" "<<*it;
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
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory ()"<<strMgr<<R"()
)";

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

