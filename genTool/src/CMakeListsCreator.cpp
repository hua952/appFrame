#include "CMakeListsCreator.h"
#include "msgTool.h"
#include "tSingleton.h"
#include "myAssert.h"
#include "appMgr.h"

CMakeListsCreator::CMakeListsCreator (const char* szSrcDir, app* pApp)
{
	myAssert(pApp);
	init(szSrcDir, pApp);
}

CMakeListsCreator::~CMakeListsCreator ()
{
}

dword CMakeListsCreator::init(const char* szFilename, app* pApp)
{
	std::ofstream os(szFilename);
	const char* szAppName = pApp->name();
	os<<"SET(prjName "<<szAppName<<")"<<std::endl;
	const char* szCon = R"(set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp)
set(defS)
set(libPath)
set(libDep)
if (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/gen/win/*.def)
	include_directories()";
	auto& rTool = tSingleton<msgTool>::single ();
	auto depInc = rTool.depIncludeHome ();
	os<<szCon<<depInc<<")"<<std::endl;
	auto depLib = rTool.depLibHome ();

	os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
const char* szC2 = R"(endif ()
add_library(${prjName} SHARED ${genSrcS} ${defS})
	include_directories()";
	auto framePath = rTool.frameHome ();
	os<<szC2<<std::endl
	<<"    "<<framePath<<"common/src"<<std::endl
	<<"    "<<framePath<<"logicCommon/src"<<std::endl
	<<"    "<<framePath<<"cLog/src"<<std::endl
                           <<")"<<std::endl;
	auto libPath = rTool.frameLibPath ();

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
	auto outPath = rTool.outPutPath ();
	os<<szC3<<std::endl
		<<libPath<<R"(common.lib)"<<std::endl
		<<libPath<<R"(logicCommon.lib)"<<std::endl
		<<libPath<<R"(cLog.lib)"<<std::endl
		<<R"()
endif ()
SET(LIBRARY_OUTPUT_PATH )"<<outPath<<R"())";
return 0;
}
