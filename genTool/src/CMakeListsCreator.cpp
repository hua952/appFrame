#include "CMakeListsCreator.h"
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
	const char* szCon = R"(set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/*.def)
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()
add_library(${prjName} SHARED ${srcS} ${defS})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin))";
os<<szCon;
return 0;
}
