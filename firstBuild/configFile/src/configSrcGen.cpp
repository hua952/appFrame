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
	)"<<pClassName<<R"( ();
	int  procCmdArgS (int nArg, char** argS);
	int  dumpConfig (const char* szFile);
	int  loadConfig (const char* szFile);
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
target_include_directories(${prjName} PRIVATE 
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

		auto pSSVr = std::make_unique<std::stringstream>();
		std::stringstream& ssProcCmdArgS = *pSSVr;
		std::stringstream ssInit;

		os<<R"(#include ")"<<pConfigFileName<<R"(.h"
#include <memory>
#include <sstream>
#include <fstream>
#include <vector>
#include "strFun.h"

)";

	auto pDumpOs = std::make_unique<std::stringstream>();
	auto& dumpOs = *pDumpOs;
		for (auto it = rMap.begin (); rMap.end () != it; ++it) {
			auto pItem = it->second;
			auto itemName = it->first;
			auto dataType = (BigDataType)(pItem->dataType ());
			auto writeDataType = pItem->itemType ();
			auto memberItemName = pItem->memberItemName ();
			auto itemType = pItem->itemType ();
			auto itemValue = pItem->itemValue ();
			auto pWordName = pItem->wordItemName();
			
			if (BigDataType_string == dataType) {
				writeDataType = "const char*";
				dumpOs<<"	int n"<<itemName<<"Len = 0;"<<std::endl
				<<"	auto "<<itemName<<" = this->"<<itemName<<"();"<<std::endl
				<<"	if ("<<itemName<<") n"<<itemName<<"Len = strlen("<<itemName<<");"<<std::endl
				<<"	std::string strT"<<itemName<<R"++( =R"("")";)++"<<std::endl
				<<"	if (n"<<itemName<<"Len) "<<std::endl
				<<"		strT"<<itemName<<" = "<<itemName<<";"<<std::endl
				//<<R"(		ofs<<R"()"<<itemName<<"= <<)"<<"strT"<<itemName;
				<<R"++(		ofs<<")++"<<itemName<<R"++(="<<strT)++"<<itemName;
			} else {
				// dumpOs<<"		ofs<<"""<<itemName<<"=""<<"<<itemName<<"()"<<std::endl;
				dumpOs<<R"++(		ofs<<")++"<<itemName<<R"++(="<<)++"<<itemName<<"()";
			}
			auto pCommit = pItem->commit();
			if (pCommit) {
				dumpOs<<R"++(<<R"--(  ## )++"<<pCommit<<R"**(   )--")**";
			}
			dumpOs<<"<<std::endl;"<<std::endl;
			funOs<<writeDataType<<"  "<<pClassName<<"::"<<itemName<<" ()"<<std::endl
				<<"{"<<std::endl
				<<"    return "<<memberItemName;
			if (BigDataType_string == dataType) {
				funOs<<".get()";
			}
			funOs<<";"<<std::endl
				<<"}"<<std::endl<<std::endl;
			
			funOs<<"void  "<<pClassName<<"::set"<<pWordName<<" ("<<writeDataType<<" v)"<<std::endl
				<<"{"<<std::endl;
			if (BigDataType_string == dataType) {
				funOs<<"	strCpy(v, "<<memberItemName<<")";
			} else {
				funOs<<"	"<<memberItemName<<" = v";
			}
			funOs<<";"<<std::endl
				<<"}"<<std::endl<<std::endl;
			
			std::stringstream& ssVR = *pSSVr;

			ssVR<<R"(		if (strKey == ")"<<itemName<<R"(") {
				)";
			if (BigDataType_int == dataType) {
					std::string strV = "0";
					if (itemValue) {
						strV = itemValue;
					}
					ssInit<<memberItemName<<R"( = )"<<strV<<R"(;
	)";

				if (strcmp (itemType, "ubyte") == 0) {
					ssVR<<memberItemName<<R"( = (ubyte)(atoi(retS[1]));)";
				} else {
					ssVR<<R"(ssV>>)"<<memberItemName<<";";
					std::string strV = "0";
					if (itemValue) {
						std::stringstream ssV;
						ssV<<R"((ubyte)(atoi()"<<itemValue<<R"()))";
						strV = ssV.str();
					}
				}
			} else if (BigDataType_bool == dataType) {

				ssVR<<R"(ssV>>strVal;
	)";
				ssVR<<memberItemName<<R"( = (strVal == "true") || (atoi(strVal.c_str()));)";

				std::string strV = "false";
				if (itemValue) {
					strV = itemValue;
				}
				ssInit<<memberItemName<<R"( = )"<<strV<<R"(;
	)";
			} else {

				ssVR<<R"(ssV>>strVal;
				if('"'==strVal.c_str()[0] && '"'==strVal.c_str()[strVal.length()-1]) {
					strVal = strVal.substr(1,strVal.length()-2);
				}
	strCpy(strVal.c_str(), )"<<memberItemName<<");";
			
				std::string strV = "";
				if (itemValue) {
					strV = itemValue;
				}

				ssInit<<R"(strCpy(")"<<strV<<R"(", )"<<memberItemName<<R"();
	)";
			}
			ssVR<<R"(
				continue;
			}
		)";
		}

		os<<pClassName<<"::"<<pClassName<<R"( ()
{
	)"<<ssInit.str()<<R"(
}
)"
<<funOs.str()<<std::endl<<R"(

int  )"<<pClassName<<R"(:: dumpConfig (const char* szFile)
{
	int nRet = 0;
	
	std::unique_ptr<char[]>	dirBuf;
	strCpy (szFile,dirBuf);
	auto pDir = dirBuf.get();
	upDir (pDir);
	do {
		std::ofstream ofs (szFile);
		if (!ofs) {
			nRet = 1;
			break;
		}
)";
	os<<dumpOs.str()<<R"(
	} while (0);
	return nRet;
}

int  )"<<pClassName<<R"(:: loadConfig (const char* szFile)
{
	int nRet = 0;
	do {
		std::ifstream ifs (szFile);
		if (!ifs) {
			dumpConfig (szFile);
			break;
		}

		std::stringstream ss;
		std::string strLine;
		std::vector <std::string> vecT;
		while(getline(ifs, strLine)) {
			auto nf = strLine.find ("##");
			if (nf != strLine.npos) {
				strLine = strLine.substr(0, nf);
			}
			vecT.push_back (strLine);
		}
		auto argS = std::make_unique<std::unique_ptr<char[]>[]> (vecT.size());
		auto ppArgS = std::make_unique<char*[]> (vecT.size());
		auto n = 0;
		for (auto it = vecT.begin (); it != vecT.end (); it++) {
			auto& arg = argS[n];
			strCpy (it->c_str(), arg);
			ppArgS[n] = arg.get();
			n++;
		}
		nRet = procCmdArgS (n, ppArgS.get());
	} while (0);
	return nRet;
}

int  )"<<pClassName<<R"(:: procCmdArgS (int nArg, char** argS)
{
	int nRet = 0;
	do {
		for (decltype (nArg) i = 0; i < nArg; i++) {
			std::unique_ptr<char[]> pArg;
			strCpy (argS[i], pArg);
			char* retS[3];
			auto pBuf = pArg.get ();
			auto nR = strR (pBuf, '=', retS, 3);
			if (2 != nR) {
				continue;
			}
			std::string strKey;
			std::string strVal;
			std::stringstream ssK (retS[0]);
			ssK>>strKey;
			std::stringstream ssV (retS[1]);
)"<<ssProcCmdArgS.str()<<R"(
		}
	} while (0);
	return nRet;
}
)";
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

