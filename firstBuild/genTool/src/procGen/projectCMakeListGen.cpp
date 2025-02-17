#include "projectCMakeListGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
#include "configMgr.h"
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
		auto& rConfig = tSingleton<configMgr>::single ();

		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto& rApps = rAppMgr.appS ();

		auto szPrjName = rGlobalFile.projectName ();
		auto structBadyType = rConfig.structBadyType ();
		std::string strAddProSer;
		std::stringstream ssAdd;
		if (rApps.size() > 1) {
			ssAdd<< R"(add_subdirectory (protobufSer))";
		}
		ssAdd<<R"(add_subdirectory ()"<<szPrjName<<R"(Config)
 )";
		strAddProSer = ssAdd.str();
		//}
		// auto bH = rGlobalFile.haveServer ();
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
			std::string strFile = rGlobalFile.projectHome ();
			strFile += "/CMakeLists.txt";
		if (isPathExit (strFile.c_str ())) {
			break;
		}
			std::ofstream os(strFile.c_str ());
			if (!os) {
				nRet = 1;
				rError ("create file error file name is : "<<strFile.c_str ());
				break;
			}

// set (firstBuildInc ${CMAKE_INSTALL_PREFIX}/include/appFrame  CACHE INTERNAL "Shared variable")
// set (firstBuildLib ${CMAKE_INSTALL_PREFIX}/lib  CACHE INTERNAL "Shared variable")
			std::string strInstall = rGlobalFile.projectInstallDir ();
			auto frameInstallDir = rGlobalFile.frameInstallPath ();
			// rGlobalFile.getRealInstallPath (strInstall);
			os<<R"(cmake_minimum_required(VERSION 3.16) 
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(BUILD_USE_64BITS on)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
if (UNIX)
	MESSAGE(STATUS "unix add -Wl,-Bsymbolic")
	set(CMAKE_CXX_LINK_OPTIONS "${CMAKE_CXX_LINK_OPTIONS} -Wl,-Bsymbolic")
endif ()

set (myProjectName )"<<szPrjName <<R"()
project(${myProjectName})
set (firstBuildInc )"<<frameInstallDir<<R"(/include/appFrame  CACHE INTERNAL "Shared variable")
set (firstBuildLib )"<<frameInstallDir<<R"(/lib  CACHE INTERNAL "Shared variable")
SET(CMAKE_INSTALL_PREFIX )"<<strInstall<<R"()
)";
// if (bH) {
	os<<R"(add_subdirectory (gen)
	)";
// }
os<<R"(
)";
			for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
				os<<R"(add_subdirectory ()"<<it->first<<")"<<std::endl;
			}
		//}
	} while (0);

	return nRet;
}

