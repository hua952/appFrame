#include "protobufSerGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "comFun.h"
#include <string>
#include <fstream>

protobufSerGen:: protobufSerGen ()
{
}

protobufSerGen:: ~protobufSerGen ()
{
}

int   protobufSerGen:: startGen ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		nR = mkDir ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = CMakeListGen ();
		if (nR) {
			break;
		}
		nR = defFileGen ();
		if (nR) {
			break;
		}
    } while (0);
    return nRet;
}

int   protobufSerGen:: CMakeListGen ()
{
    int   nRet = 0;
    do {
		auto aRoot = rootDir ();
		std::string strFile = aRoot;
		strFile += "/CMakeLists.txt";
		std::ofstream os (strFile.c_str());
		auto& rGlobal = tSingleton<globalFile>::single ();
		auto thirdPartyDir = rGlobal.thirdPartyDir ();
		auto appFrameInstall = rGlobal.frameInstallPath ();
		os<<R"(SET(prjName protobufSer)
set(defS)
set(osSrc)
set(libPath)
set(libDep)
set(srcS)
set(pbSrcS)
file(GLOB srcS src/*.cpp)
file(GLOB pbSrcS src/*.pb.cc)
if (UNIX)
    MESSAGE(STATUS "unix")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	add_compile_definitions(PROTOBUF_USE_DLLS)
	file(GLOB defS src/*.def)
	include_directories()"<<thirdPartyDir<<R"(/include/)
list(APPEND libDep )"<<thirdPartyDir<<R"(/lib/)
endif ()
	include_directories(
	)"<<appFrameInstall<<R"(/include/appFrame
)
list(APPEND libPath )"<<appFrameInstall<<R"(/lib)
link_directories(${libPath} ${libDep})
	add_library(${prjName} SHARED  ${defS} ${srcS} ${pbSrcS} )
	target_link_libraries(${prjName} PUBLIC
	common
	logicCommon
	cLog
	abseil_dll
	libprotobufd
	defMsg
	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	)";
    } while (0);
    return nRet;
}

int   protobufSerGen:: defFileGen ()
{
    int   nRet = 0;
    do {
		
		auto aSrc = srcDir ();
		std::string strFile = aSrc;
		strFile += "/expFun.def";
		std::ofstream os (strFile.c_str());
		if (!os) {
			nRet = 1;
			break;
		}
		const char* szCon = R"(LIBRARY
EXPORTS
	getSerializeFunS @1)";
		os<<szCon;
    } while (0);
    return nRet;
}

int   protobufSerGen:: mkDir ()
{
    int   nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strDir = projectHome;
		strDir += "/protobufSer";
		setRootDir (strDir.c_str());
		strDir += "/src";
		myMkdir (strDir.c_str ());
    } while (0);
    return nRet;
}

const char*  protobufSerGen:: rootDir ()
{
    return m_rootDir.get ();
}

void  protobufSerGen:: setRootDir (const char* v)
{
    strCpy (v, m_rootDir);
	std::string strS = v;
	strS += "/src";
	setSrcDir (strS.c_str ());
}

const char*  protobufSerGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  protobufSerGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

