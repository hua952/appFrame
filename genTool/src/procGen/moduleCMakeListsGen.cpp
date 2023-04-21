#include "moduleCMakeListsGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFile.h"
#include <string>
#include "rLog.h"
#include <fstream>

moduleCMakeListsGen:: moduleCMakeListsGen ()
{
}

moduleCMakeListsGen:: ~moduleCMakeListsGen ()
{
}

int   moduleCMakeListsGen:: startGen (moduleFile& rMod)
{
	int   nRet = 0;
	do {
		auto moduleName = rMod.moduleName ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto appName = rMod.appName ();

		std::string strFile = rGlobalFile.projectHome ();
		strFile += appName;
		strFile += "/";
		strFile += moduleName;
		strFile += "/CMakeLists.txt";

		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strFile.c_str ());
			break;
		}
		os<<"SET(prjName "<<moduleName<<")"<<std::endl;
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

