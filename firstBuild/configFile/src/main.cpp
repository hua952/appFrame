#include <iostream>
#include <memory>
#include "xmlLoad.h"
#include "configSrcGen.h"
#include "itemMgr.h"
#include "item.h"
#include "strFun.h"
#include "myAssert.h"

int main (int nC, char* argS [])
{
	int nRet = 0;
	
	do {
		auto& rC = std::cout;

		std::string incDir = R"(${firstBuildInc})";
		std::string libDir = R"(${firstBuildLib})";
		std::string strFile;
		std::string projectDir;
		std::string projectName;
		std::string className;
		int nR = 0;
		for (decltype (nC) i = 1; i < nC; i++) {
			std::unique_ptr<char[]> buf;
			strCpy(argS[i], buf);
			char* retS[3];
			nR = strR (buf.get(), '=', retS, 3);
			if (2 != nR) {
				continue;
			}
			if (strcmp(retS[0], "firstBuildInc") == 0) {
				incDir = retS[1];
			} else if (strcmp(retS[0], "firstBuildLib") == 0) {
				libDir = retS[1];
			} else if (strcmp(retS[0], "file") == 0) {
				strFile = retS[1];
			} else if (strcmp(retS[0], "projectDir") == 0) {
				projectDir = retS[1];
			} else if (strcmp(retS[0], "projectName") == 0) {
				projectName = retS[1];
			} else if (strcmp(retS[0], "className") == 0) {
				className = retS[1];
			}
		}
		if (!strFile.empty()) {
			xmlLoad loader;
			nR =  loader.load (strFile.c_str());
			if (nR) {
				nRet = 1;
				break;
			}
		}
		auto& rMgr = itemMgr::mgr ();
		auto szProjectDir = rMgr.projectDir ();
		if (!szProjectDir) {
			if (projectDir.empty()) {
				nRet = 2;
				rC<<" error projectDir null"<<std::endl;
				break;
			} else {
				rMgr.setProjectDir(projectDir.c_str());
			}
		}
		auto projectNameSZ = rMgr.projectName ();
		if (!projectNameSZ) {
			if (projectName.empty()) {
				nRet = 3;
				rC<<" error projectName null"<<std::endl;
				break;
			} else {
				rMgr.setProjectName (projectName.c_str());
			}
		}
		auto classNameSZ = rMgr.className ();
		if (!classNameSZ) {
			if (className.empty()) {
				nRet = 4;
				rC<<" error className null"<<std::endl;
				break;
			} else {
				rMgr.setClassName(className.c_str());
			}
		}
		auto& rMap = rMgr.itemS ();
		auto modelSName = "modelS";
		auto it = rMap.find (modelSName);
		if (rMap.end() == it) {
			auto pI = std::make_shared <item> ();
			pI->setItemName (modelSName);
			pI->setItemType ("string");
			pI->setItemValue ("");
			pI->setDataType (BigDataType_string);
			auto ret = rMap.insert (std::make_pair(modelSName, pI));
			myAssert(ret.second);
		}
		
		rMgr.setIncludeDir (incDir.c_str());
		rMgr.setLibDir (libDir.c_str());
		auto& rConfig = configSrcGen::mgr ();
		nR = rConfig.startGen ();
		if (nR) {
			nRet = 3;
			break;
		}
	} while (0);
	return nRet;
}
