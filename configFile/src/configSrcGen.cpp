#include "configSrcGen.h"
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
		// const char* szSrcDir = "./gen";
		// auto& rMgr = itemMgr::mgr ();
		// auto& rMap = rMgr.itemS ();
		int nR = 0;
		nR = writeClassH ();
		if (nR) {
			nRet = 1;
			break;
		}
    } while (0);
    return nRet;
}

int  configSrcGen:: writeClassH ()
{
    int  nRet = 0;
    do {
		const char* szSrcDir = "./gen/";
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();
		auto pClassName = rMgr.className ();
		std::string strFilename = szSrcDir;
		strFilename += pClassName;
		strFilename += ".h";

		std::ofstream os (strFilename.c_str ());
		if (!os) {
			nRet = 1;
			break;
		}
		os<< R"---(#ifndef _)---"<<pClassName<<R"(__h_
#define _)"<<pClassName<<R"(__h_)"<<std::endl
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
			ssF<<"  "<<writeDataType<<"  "<<itemName<<" ();"<<std::endl
				<<"  "<<"void  set"<<itemName<<" ("<<writeDataType<<" v);"<<std::endl;
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

int  configSrcGen:: writeClassCpp ()
{
    int  nRet = 0;
    do {
		const char* szSrcDir = "./gen/";
		auto& rMgr = itemMgr::mgr ();
		auto& rMap = rMgr.itemS ();
		auto pClassName = rMgr.className ();
		std::string strFilename = szSrcDir;
		strFilename += pClassName;
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

		os<<R"(#include ")"<<pClassName<<R"(")"<<std::endl;
		for (auto it = rMap.begin (); rMap.end () != it; ++it) {
			auto pItem = it->second;
			auto itemName = it->first;
			auto dataType = pItem->dataType ();
			auto writeDataType = pItem->itemType ();
			if (1 == dataType) {
				writeDataType = "const char*";
			}
			funOs<<writeDataType<<"  "<<itemName<<" ()"<<std::endl
				<<"{"<<std::endl
				<<"    return m_"<<itemName;
			if (1 == dataType) {
				funOs<<".get()";
			}
			funOs<<";"<<std::endl
				<<"}"<<std::endl<<std::endl;

			funOs<<"void  set"<<itemName<<" ("<<writeDataType<<" v)"<<std::endl
				<<"{"<<std::endl;
			if (1 == dataType) {
				// funOs<<
			}

			writeDataType = pItem->itemType ();
			if (1 == dataType) {
				writeDataType = "std::unique_ptr <char[]>";
			}
			ssM<<"  "<<writeDataType<<"  m_"<<itemName<<";"<<std::endl;
		}
    } while (0);
    return nRet;
}

configSrcGen& configSrcGen:: mgr()
{
	static configSrcGen s_mgr;
	return s_mgr;
}
