#include "projectCMakeListGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
#include "rLog.h"

projectCMakeListGen:: projectCMakeListGen ()
{
}

projectCMakeListGen:: ~projectCMakeListGen ()
{
}

int   projectCMakeListGen:: startGen ()
{
	int   nRet = 0;
	do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		// for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			std::string strFile = rGlobalFile.projectHome ();
			strFile += "CMakeLists.txt";

			std::ofstream os(strFile.c_str ());
			if (!os) {
				nRet = 1;
				rError ("create file error file name is : "<<strFile.c_str ());
				break;
			}

			auto szPrjName = rGlobalFile.projectName ();
			// auto installPath = rGlobalFile.installPath ();
			std::string strInstall;
			rGlobalFile.getRealInstallPath (strInstall);
			os<<R"(cmake_minimum_required(VERSION 3.16) 
set(BUILD_USE_64BITS on)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set (myProjectName )"<<szPrjName <<R"()
project(${myProjectName})
SET(CMAKE_INSTALL_PREFIX )"<<strInstall<<R"()
add_subdirectory (defMsg)
)";
			for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
				os<<R"(add_subdirectory ()"<<it->first<<")"<<std::endl;
			}
		//}
	} while (0);

	return nRet;
}

