#include "moduleMgrGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include "fromFileData/globalFile.h"
#include "tSingleton.h"
#include "rLog.h"
#include <string>
#include <fstream>

moduleMgrGen:: moduleMgrGen ()
{
}

moduleMgrGen:: ~moduleMgrGen ()
{
}

int   moduleMgrGen:: startGen (appFile& rApp)
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string projectHome = rGlobalFile.projectHome ();
		auto appName = rApp.appName ();
		auto strDir = projectHome;
		std::string mgrName = appName;
		mgrName += "moduleMgr";
		strDir += mgrName;
		int nR = 0;
		nR = myMkdir (strDir.c_str());
		nR = writeCMakeLists (strDir.c_str (), rApp);
		if (nR) {
			rError (" writeCMakeLists return error nR = "<<nR);
			nRet = 2;
			break;
		}
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeCMakeLists (const char* szDir, appFile& rApp)
{
	int   nRet = 0;
	do {
		std::string fileName = szDir;
		fileName += "/CMakeLists.txt";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto appName = rApp.appName ();
		std::string mgrName = appName;
		mgrName += "moduleMgr";
os<<"SET(prjName "<<mgrName<<")"<<std::endl;
		const char* szCon = R"(set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp)
set(defS)
set(libPath)
set(libDep)
if (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	file(GLOB defS src/gen/win/*.def)
	include_directories()";
	auto depInc = rGlobalFile.depIncludeHome ();
	os<<szCon<<depInc<<")"<<std::endl;
	auto depLib = rGlobalFile.depLibHome ();

	os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()
	add_library(${prjName} SHARED ${genSrcS} ${defS})
	include_directories()";
	auto framePath = rGlobalFile.frameHome ();
	os<<szC2<<std::endl
	<<"    "<<framePath<<"common/src"<<std::endl
	<<"    "<<framePath<<"logicCommon/src"<<std::endl
	<<"    "<<framePath<<"cLog/src"<<std::endl
	<<")"<<std::endl;
	auto libPath = rGlobalFile.frameLibPath ();

	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl;
	const char* szC3 = R"(link_directories(${libPath} ${libDep})
	if (UNIX)
	target_link_libraries(${prjName} PUBLIC
	common
	logicCommon
	cLog
	)
	elseif (WIN32)
	target_link_libraries(${prjName} PUBLIC)";
	auto outPath = rGlobalFile.outPutPath ();
	os<<szC3<<std::endl
	<<libPath<<R"(common.lib)"<<std::endl
	<<libPath<<R"(logicCommon.lib)"<<std::endl
	<<libPath<<R"(cLog.lib)"<<std::endl
	<<R"()
	endif ()
	SET(LIBRARY_OUTPUT_PATH )"<<outPath<<R"())";
	} while (0);
    return nRet;
}

