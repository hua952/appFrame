#include <string>
#include "myAssert.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <filesystem>
#include "comFun.h"
#include "strFun.h"

std::string g_strProjectName;
std::string g_strProjectDir;
std::string g_strProjectHome;
std::string g_strSrc;

udword g_mfc = 0;
int genCmakeFile ()
{
	int nRet = 0;
	do {
		std::string strFile = g_strProjectHome;
		strFile += "/CMakeLists.txt";
		if (isPathExit (strFile.c_str())) {
			break;
		}
		std::ofstream of(strFile.c_str());
		if (!of) {
			nRet = 1;
			break;
		}
		of<<R"(cmake_minimum_required(VERSION 3.16) 
SET(prjName )"<<g_strProjectName<<R"()
project(${prjName})
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
# find_package(fmt CONFIG REQUIRED)
# find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")
set(srcS)
set(rcS)
file(GLOB srcS src/*.cpp)
file(GLOB rcS src/*.rc)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	add_definitions(-w)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/utf-8)
	ADD_DEFINITIONS(/W1)
	)";
	if (g_mfc) {
		myAssert (g_mfc < 3);
		of<<R"(
	ADD_DEFINITIONS(-D_AFXDLL)
	SET(CMAKE_MFC_FLAG 2)
		)";
	}
	of<<R"(
endif ()

# include_directories()

add_executable (${prjName} )";
if (g_mfc) {
	of<<"WIN32 ";
}
of<<R"(${srcS} ${rcS})
# target_include_directories(${prjName} PRIVATE src)
# target_link_libraries(${prjName} PRIVATE  common cLog)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
		)";
	} while (0);
	return nRet;
}

int  genMain ()
{
	int nRet = 0;
	do {
		if (g_mfc) {
			break;
		}
		std::string strFile = g_strSrc;
		strFile += "/main.cpp";
		if (isPathExit (strFile.c_str())) {
			break;
		}
		std::ofstream of(strFile.c_str());
		if (!of) {
			nRet = 1;
			break;
		}
		of<<R"(#include <iostream>
#include <memory>
int main (int argC, char** argS)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}
		)";

	} while (0);
	return nRet;
}

int  main (int argC, char** argS)
{
	int nRet = 0;
	if (argC > 1) {
		g_strProjectName = argS[1];
	}
		/*
	if (argC > 2) {
		g_strProjectDir = argS[2];
	}
	*/
	for (decltype (argC) i = 1; i < argC; i++) {
		getValueFromArgKV(argS[i], "mfc", g_mfc);
		getValueFromArgKV(argS[i], "projectDir", g_strProjectDir);
	}
	do {
		if (g_strProjectName.empty()) {
			std::cout<<"必须输入工程名称 : "<<std::endl;
			return 1;
		}
		if (g_strProjectDir.empty()) {
			g_strProjectDir = std::filesystem::current_path().string();
		}
		g_strProjectHome = g_strProjectDir;
		g_strProjectHome += "/";
		g_strProjectHome += g_strProjectName;
		auto strBuild = g_strProjectHome;
		strBuild += "/build";
		std::filesystem::create_directories (strBuild.c_str());
		g_strSrc = g_strProjectHome;
		g_strSrc += "/src";
		std::filesystem::create_directories (g_strSrc.c_str());
		int nR = 0;
		nR = genCmakeFile ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = genMain ();
		if (nR) {
			nRet = 2;
			break;
		}
	} while (0);
	return nRet;
}
