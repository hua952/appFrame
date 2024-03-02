#include "appCMakeListsGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/moduleFile.h"
#include <string>
#include "rLog.h"
#include <fstream>

appCMakeListsGen:: appCMakeListsGen ()
{
}

appCMakeListsGen:: ~appCMakeListsGen ()
{
}

int   appCMakeListsGen:: startGen (appFile& rApp)
{
	int   nRet = 0;
	do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		// auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		// for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			std::string strFile = rGlobalFile.projectHome ();
			strFile += rApp.appName ();
			strFile += "/CMakeLists.txt";

			std::ofstream os(strFile.c_str ());
			if (!os) {
				nRet = 1;
				rError ("create file error file name is : "<<strFile.c_str ());
				break;
			}

			auto szPrjName = rGlobalFile.projectName ();
			os<<R"(cmake_minimum_required(VERSION 3.16) 
set(BUILD_USE_64BITS on)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True))"<<std::endl;

			auto& moduleS =  rApp.moduleFileNameS ();
			for (auto it = moduleS.begin (); moduleS.end () != it; ++it) {
				os<<R"(add_subdirectory ()"<<*it<<")"<<std::endl;
			}
		// }
	} while (0);
	return nRet;
}

