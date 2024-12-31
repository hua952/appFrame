#include "main.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/structFileMgr.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/toolServerEndPointInfoMgr.h"
#include "xmlGlobalLoad.h"
#include "xmlCommon.h"
#include "rLog.h"
#include "configMgr.h"
#include "myAssert.h"
#include "procGen/globalGen.h"
#include <sstream>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace std::filesystem;

int main (int argNum, char* argS[])
{
	int nRet = 0;
	tSingleton<globalFile>::createSingleton();
	tSingleton<xmlGlobalLoad>::createSingleton();
	tSingleton<appFileMgr>::createSingleton ();
	tSingleton<moduleFileMgr>::createSingleton ();
		
	tSingleton<xmlCommon>::createSingleton ();
	tSingleton<globalGen>::createSingleton ();
	tSingleton<toolServerEndPointInfoMgr>::createSingleton ();

	initLogGlobal ();
	auto nInitLog = initLog ("genTool", "logs/genTool.log", 0);
	do {
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			nRet = 3;
			break;
		}
		
		tSingleton <configMgr>::createSingleton ();
		auto& rConfig = tSingleton <configMgr>::single ();
		auto nR = rConfig.procArgS (argNum, argS);
		if (nR) {
			rError ("rConfig.procArgS ret error nR = "<<nR);
			nRet = 4;
			break;
		}
		
		
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto defFile = rConfig.defFile ();
		if (!defFile && argNum >= 2) {

			std::filesystem::path entry(argS[1]);
			if (entry.is_relative()) {
				entry = std::filesystem::absolute(entry);
			}
			rConfig.setDefFile (entry.string().c_str());
			defFile = rConfig.defFile ();
		}
		auto projectDir = rConfig.projectDir ();
		if (projectDir ) {
			rGlobalFile.setProjectDir(projectDir);
		} else {
			path entry(defFile);
			std::string frameInstallDir = entry.parent_path().string();
			std::replace(frameInstallDir.begin(), frameInstallDir.end(), '\\', '/');
			rGlobalFile.setProjectDir(frameInstallDir.c_str());
		}
		auto projectName = rConfig.projectName ();
		if (projectName) {
			rGlobalFile.setProjectName(projectName);
		} else {
			path entry(defFile);
			rGlobalFile.setProjectName (entry.stem().string().c_str());
		}
		std::string strProjectHome = rGlobalFile.projectDir();
		strProjectHome += "/";
		strProjectHome += rGlobalFile.projectName ();
		rGlobalFile.setProjectHome (strProjectHome.c_str());
		auto frameInstallPath = rConfig.frameInstallPath ();
		if (frameInstallPath) {
			rGlobalFile.setFrameInstallPath(frameInstallPath);
		} else {
			unique_ptr<char[]> temp;
			auto nR = getCurModelPath (temp);
			myAssert (0 == nR);
			path entry(temp.get());
			std::string frameInstallDir = entry.parent_path().parent_path().string();
			std::replace(frameInstallDir.begin(), frameInstallDir.end(), '\\', '/');
			rGlobalFile.setFrameInstallPath(frameInstallDir.c_str());
		}
		auto projectInstallDir = rConfig.projectInstallDir();
		if (projectInstallDir) {
			rGlobalFile.setProjectInstallDir(projectInstallDir);
		} else {
			std::string str = rGlobalFile.projectDir ();
			str += "/";
			str += rGlobalFile.projectName();
			str += "Install";
			rGlobalFile.setProjectInstallDir(str.c_str());
		}
		auto projectHome = rGlobalFile.projectHome ();
		create_directories (projectHome);
		auto& rXmlGlobal = tSingleton <xmlGlobalLoad>::single ();
		nR = rXmlGlobal.xmlLoad (defFile);
		if (nR) {
			rError ("xmlLoad return error nR = "<<nR);
			nRet = 5;
			break;
		}
		
		auto& rGlobalGen = tSingleton <globalGen>::single ();
		nR = rGlobalGen.startGen ();
		if (nR) {
			nRet = 6;
			rError ("rGlobalGen.startGen return error nR = "<<nR);
			break;
		}
		nR = rGlobalGen.secondGen ();
		if (nR) {
			nRet = 7;
			rError ("rGlobalGen.secondGen return error nR = "<<nR);
			break;
		}
	} while (0);
	std::cout<<" tool will end nRet = "<<nRet<<std::endl;
	return nRet;
}
