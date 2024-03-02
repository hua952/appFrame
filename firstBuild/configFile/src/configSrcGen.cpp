#include "configSrcGen.h"
#include "comFun.h"
#include "strFun.h"
#include "item.h"
#include "itemMgr.h"
#include <fstream>
#include <sstream>

configSrcGen:: configSrcGen ()
{
}

configSrcGen:: ~configSrcGen ()
{
}

int  configSrcGen:: startGen ()
{
    int  nRet = 0;
    do {
		auto& rMgr = itemMgr::mgr ();
		std::string strHome = rMgr.projectDir ();
		strHome += "/";
		strHome += rMgr.projectName ();
		setProjectHome (strHome.c_str());
		strHome += "/src";
		setSrcDir (strHome.c_str());
		myMkdir (strHome.c_str());
		auto pClassName = rMgr.className ();
		setConfigFileName (pClassName);
		int nR = 0;
		nR = writeClassH ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = writeClassCpp ();
		if (nR) {
			nRet = 2;
			break;
		}
		nR = writeCMakeList ();
		if (nR) {
			nRet = 3;
			break;
		}
    } while (0);
    return nRet;
}

void toWordStr (std::string& str)
{
	std::unique_ptr<char[]> strT;
	strCpy(str.c_str(), strT);
	toWord (strT.get());
	str = strT.get();
}

int  configSrcGen:: writeClassH ()
{
    int  nRet = 0;
    do {
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();
		auto pClassName = rMgr.className ();
		auto pConfigFileName = configFileName ();
		std::string strFilename = srcDir();
		strFilename += "/";
		strFilename += pConfigFileName;
		strFilename += ".h";

		std::ofstream os (strFilename.c_str ());
		if (!os) {
			nRet = 1;
			break;
		}
		os<< R"---(#ifndef _)---"<<pClassName<<R"(__h_
#define _)"<<pClassName<<R"(__h_)"<<std::endl
		<<R"(
#include "comFun.h"
#include <memory>)"<<std::endl
		<<"class "<<pClassName<<std::endl
		<<R"({
		public:
		)";
		std::stringstream ssF;
		std::stringstream ssM;
		for (auto it = rMap.begin (); rMap.end () != it; ++it) {
			auto pItem = it->second;
			auto itemName = it->first;
			auto dataType = pItem->dataType ();
			auto writeDataType = pItem->itemType ();
			if (1 == dataType) {
				writeDataType = "const char*";
			}
			auto pWordName = pItem->wordItemName();
			ssF<<"  "<<writeDataType<<"  "<<itemName<<" ();"<<std::endl
				<<"  "<<"void  set"<<pWordName<<" ("<<writeDataType<<" v);"<<std::endl;

			writeDataType = pItem->itemType ();
			if (1 == dataType) {
				writeDataType = "std::unique_ptr <char[]>";
			}
			ssM<<"  "<<writeDataType<<"  m_"<<itemName<<";"<<std::endl;
		}
		os<<ssF.str()
			<<"private:"
			<<std::endl<<ssM.str()
			<<"};"<<std::endl
			<<"#endif"<<std::endl;
    } while (0);
    return nRet;
}


int  configSrcGen:: writeCMakeList ()
{
    int  nRet = 0;
    do {
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();

		std::string strFilename = projectHome ();
		strFilename += "/CMakeLists.txt";
		std::ofstream os (strFilename.c_str ());
		if (!os) {
			nRet = 1;
			break;
		}
		auto projectName = rMgr.projectName ();
		auto includeDir = rMgr.includeDir ();
		auto libDir = rMgr.libDir ();
		auto pClassName = rMgr.className ();
		auto pConfigFileName = configFileName ();

		os<<R"(SET(prjName )"<<projectName<<R"()
set(srcS)
file(GLOB srcS src/*.cpp)
if (UNIX)
	add_compile_options(-fPIC)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
endif ()

add_library(${prjName} ${srcS})
target_include_directories(${prjName} PUBLIC 
							)"<<includeDir<<R"(
                           )

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(incS src/)"<<pConfigFileName<<R"(.h)

install(FILES ${incS} DESTINATION include/${myProjectName}))";
    } while (0);
    return nRet;
}

int  configSrcGen:: writeClassCpp ()
{
    int  nRet = 0;
    do {
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();
		auto pClassName = rMgr.className ();
		auto pConfigFileName = configFileName ();

		std::string strFilename = srcDir();
		strFilename += "/";
		strFilename += pConfigFileName;
		strFilename += ".cpp";

		std::ofstream os (strFilename.c_str ());
		if (!os) {
			nRet = 1;
			break;
		}
		std::stringstream funOs;
		std::stringstream initOs;
		std::stringstream readFileOs;
		std::stringstream ssM;

		os<<R"(#include ")"<<pConfigFileName<<R"(.h"
#include <memory>
)";

		for (auto it = rMap.begin (); rMap.end () != it; ++it) {
			auto pItem = it->second;
			auto itemName = it->first;
			auto dataType = pItem->dataType ();
			auto writeDataType = pItem->itemType ();
			auto memberItemName = pItem->memberItemName ();
			if (1 == dataType) {
				writeDataType = "const char*";
			}
			funOs<<writeDataType<<"  "<<pClassName<<"::"<<itemName<<" ()"<<std::endl
				<<"{"<<std::endl
				<<"    return "<<memberItemName;
			if (1 == dataType) {
				funOs<<".get()";
			}
			funOs<<";"<<std::endl
				<<"}"<<std::endl<<std::endl;

			funOs<<"void  "<<pClassName<<"::set"<<itemName<<" ("<<writeDataType<<" v)"<<std::endl
				<<"{"<<std::endl;
			if (1 == dataType) {
				funOs<<"strCpy(v, "<<memberItemName<<");"<<std::endl;
			} else {
				funOs<<"	"<<memberItemName<<" = v;"<<std::endl;
			}
		}
		os<<funOs.str()<<std::endl;
    } while (0);
    return nRet;
}

configSrcGen& configSrcGen:: mgr()
{
	static configSrcGen s_mgr;
	return s_mgr;
}

const char*  configSrcGen:: projectHome ()
{
    return m_projectHome.get ();
}

void  configSrcGen:: setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

const char*  configSrcGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  configSrcGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

const char*  configSrcGen:: configFileName ()
{
    return m_configFileName.get ();
}

void  configSrcGen:: setConfigFileName (const char* v)
{
    strCpy (v, m_configFileName);
}

