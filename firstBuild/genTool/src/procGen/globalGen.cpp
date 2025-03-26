#include <vector>
#include<filesystem>
#include "globalGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/msgPmpFile.h"

#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/msgGroupFile.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/structFileMgr.h"
#include "fromFileData/structFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/appFile.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/serverFile.h"
#include "fromFileData/dataFile.h"

#include "protobufSerGen.h"
#include "tSingleton.h"
#include "msgGen.h"
#include "defMsgGen.h"
#include "projectCMakeListGen.h"
#include "appLibGen.h"
#include "appModuleGen.h"
#include "rLog.h"
#include<filesystem>
using namespace std::filesystem;

globalGen:: globalGen ()
{
}

globalGen:: ~globalGen ()
{
}

int   globalGen:: writeGenCmakelist ()
{
    int   nRet = 0;
	do {
		auto &rGlobalFile = tSingleton<globalFile>::single();
		auto  projectHome = rGlobalFile.projectHome ();
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();

		std::string strFile = projectHome;
		strFile += "/gen";
		myMkdir(strFile.c_str());
		strFile += "/CMakeLists.txt";

		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("writeGenCmakelist error file name is : "<<strFile.c_str ());
			break;
		}
		auto szPrjName = rGlobalFile.projectName ();
		// bool haveMsg = rGlobalFile.haveMsg();
		// if(haveMsg){

		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		// auto& rApps = rAppMgr.appS ();
		if (pPmp) {
			os<<R"(
add_subdirectory (defMsg)
)";
			if (rAppS.size() > 1) {
				os<<R"(
add_subdirectory (protobufSer)
)";
			}
		}
os<<R"(add_subdirectory ()"<<szPrjName <<R"(Config))"<<std::endl;
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());
			auto appName = rApp.appName ();
			os<<R"(add_subdirectory ()"<<appName<<R"())"<<std::endl;
		}
	} while (0);
    return nRet;
}

int   globalGen:: startGen ()
{
    int   nRet = 0;
    do {
		int nR = 0;
		nR = makePath ();
		projectCMakeListGen  projectGen;
		nR = projectGen.startGen ();
		if (nR) {
			rError ("projectGen.startGen error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = configLibGen ();
		if (nR) {
			rError ("projectGen.configLibGen error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = writeGenCmakelist ();
		if (nR) {
			rError ("writeGenCmakelist  error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = 0;
		auto &rGlobal = tSingleton<globalFile>::single();
		auto bH = rGlobal.haveServer ();
		bool haveMsg = rGlobal.haveMsg();
		
		std::vector <std::shared_ptr<msgGen>> gv;
		if (bH && haveMsg) {
			protobufSerGen  protoGen;
			nR = protoGen.startGen ();
			if (nR) {
				rError ("projectGen.startGen error nR = "<<nR);
				nRet = 4;
				break;
			}
			auto &rPmpS = rGlobal.msgFileS ();
			for (auto it = rPmpS.begin(); rPmpS.end() != it; ++it) {
				auto& rPmp = *(it->second.get());
				auto pP = std::make_shared <msgGen> (rPmp);
				msgGen  &gen = *pP;
				nR = gen.startGen ();
				gv.push_back (pP);
				if (nR) {
					break;
				}
			}
			if (nR) {
				nRet = 5;
				rError ("msg gen error nRet = "<<nR);
				break;
			}
			defMsgGen msgGenD;
			nR = msgGenD.startGen ();
			if (nR) {
				rError ("defMsg gen error nR = "<<nR);
				nRet = 6;
				break;
			}
		}

		nR = runWorkNumGen ();
		if (nR) {
			nRet = 7;
			break;
		}

		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());

			appLibGen libGen (rApp);
			nR = libGen.startGen ();
			if (nR) {
				rError(" libGen.startGen error nR = "<<nR);
				nRet = 8;
				break;
			}
			appModuleGen moduleGen (rApp);
			nR = moduleGen.startGen ();
			if (nR) {
				rError(" moduleGen.startGen error nR = "<<nR);
				nRet = 9;
				break;
			}
		}
		if (nRet) {
			break;
		}
		nR = 0;
		for (auto it = gv.begin(); gv.end() != it; ++it) {
			msgGen&  gen = *(it->get());
			nR = gen.rpcInfoCppGen ();
			if (nR) {
				break;
			}
		}
		if (nR) {
			rError ("rpcInfoCppGen gen error nR = "<<nR);
			break;
		}
    } while (0);
    return nRet;
}

int globalGen::runWorkNumGen ()
{
    int  nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		std::stringstream ssTmp;
		/*
		std::string strFile = srcDir ();
		strFile += "/loopHandleS.h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os <<R"(#ifndef _loopHandleS_h__
#define _loopHandleS_h__
#include "packet.h"

)";
	*/	
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		auto& rModMgr = tSingleton<moduleFileMgr>::single ();
		int ip = 0;
		std::stringstream ssTem;
		auto pSSRoot = std::make_unique<std::stringstream>();
		auto& ssRoot = *pSSRoot;
		/*
		std::stringstream appTem;
		appTem<<R"(
enum appTmpID
{
)";
*/
		int  appIndex = 0;
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());
			int is = ip++ * LoopNum;
			rApp.setProcId (is);
			auto netType = rApp.netType ();
			int mutI = 0;
			int sinI = 0;
			int lvI[c_serverLevelNum];
			for (int i = 0; i < c_serverLevelNum; i++) {
				lvI[i] = 0;
			}
			rApp.setAppGroupIdInt (appIndex);
			// appTem<<rApp.appGroupId()<<" = "<<appIndex<<","<<std::endl;

			if (appNetType_gate == netType) {
				rGlobal.setGateAppGroupId ((uword)appIndex);
			}
			appIndex++;

			/*
			std::stringstream routeThTem;
			std::stringstream appThTem;
			appThTem<<R"(enum )"<<rApp.appName()<<R"(ServerTmpID
{
)"<<std::endl;
*/
			auto tmpId = 0;
			serverFile* pRouteServer = nullptr;
			
				auto& rSS = rApp.orderS ();
				bool firstServer = true;
				auto pRunSS = std::make_unique<std::stringstream>();
				auto& runSS = *pRunSS;
				runSS<<"worker:";
				for (auto iter = rSS.begin ();
						rSS.end () != iter; ++iter) {
					auto pServer = iter->get (); // iter->second.get ();
					auto pSName = pServer->strHandle ();
					auto pTmpHandle = pServer->strTmpHandle ();
					
					auto openNum = pServer->openNum ();
					serverIdType nLevel = 0xffff;
					for (serverIdType i = 0; i < c_serverLevelNum; i++) {
						if (openNum <= c_levelMaxOpenNum[i]) {
							nLevel = i;
							break;
						}
					}
					myAssert (nLevel < c_serverLevelNum);

					auto serId = nLevel * c_onceServerLevelNum + c_levelMaxOpenNum [nLevel] * lvI[nLevel]++;
					serId += is;
					decltype (nLevel) tmpNum = serId;
					pServer->setTmpNum (tmpNum);
					ssTem<<R"(#define  )"<<pTmpHandle<<" "<<serId<<std::endl;
					auto route = pServer->route ();
					auto runNum = pServer->openNum ();
					int autoRun = pServer->autoRun () ? 1:0;
					if (route) {
						myAssert (!pRouteServer);
						pRouteServer = pServer;
						// routeThTem<<pServer->strServerGroupId ();
					} else {
						// appThTem<<R"(    )"<<pServer->strServerGroupId ()<<" = "<<tmpId<<","<<std::endl;
						if (firstServer) {
							firstServer = false;
						} else {
							runSS<<"-";
						}
						runSS<<tmpId<<"*"<<runNum<<"*"<<autoRun<<"*"<<pServer->sleepSetp();
						pServer->setServerGroupId (tmpId);
						tmpId++;
					}
					if (appNetType_gate == netType && route) {
						myAssert (ssRoot.str().empty());
						ssRoot<<R"(#define  c_rootTmp )"<<pTmpHandle<<std::endl;
					}
				}
				if (pRouteServer) {
					auto runNum = appNetType_gate == netType ? rGlobal.netNum() : 1;
					int autoRun = pRouteServer->autoRun () ? 1:0;
					if (!firstServer) {
						runSS<<"-";
					}
					runSS<<tmpId<<"*"<<runNum<<"*"<<autoRun<<"*"<<pRouteServer->sleepSetp()<<"+routeWorker:"<<tmpId;
					pRouteServer->setServerGroupId (tmpId);
					if (appNetType_gate == netType) {
						rGlobal.setGateRouteServerGroupId (tmpId);
					}
				}
				rApp.setRunWorkNum (runSS.str().c_str());
			/*
			if (!routeThTem.str().empty()) {
				appThTem<<R"(    )"<<routeThTem.str()<<" = "<<tmpId<<","<<std::endl;
				tmpId++;
			}
			appThTem<<R"(    )"<<rApp.appName()<<R"(ServerTmpID_serverTmpNum
};
)";
*/
			/*
	os<<appThTem.str()<<std::endl;
	if (pRouteServer) {
		os<<R"(#define )"<<rApp.appName()<<R"(RouteServerTmp )"<<routeThTem.str()<<std::endl;
	}
	*/
	}
	/*
appTem<<R"(    appTmpId_appTmpNum
};
)";

		os<<appTem.str()<<std::endl;
		os<<ssTem.str()<<std::endl<<ssRoot.str()<<std::endl;

		os<<R"(#endif)";
		*/
	} while (0);
    return nRet;
}

int   globalGen:: configLibGen ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto configDef = rGlobalFile.configDef ();
		std::string strFile = rGlobalFile.xmlDir ();
		strFile += "/";
		std::unique_ptr<char[]> temp;
		auto nR = getCurModelPath (temp);
		myAssert(!nR);
		std::filesystem::path entry(temp.get());

		std::stringstream cmdOs;
		cmdOs<<entry.parent_path().string()<<"/configFile ";
		if (configDef) {
			strFile += configDef;
			auto bE = isPathExit (strFile.c_str());
			if (bE) {
				cmdOs<<"file="<<strFile<<" ";
			}
		}
		auto projectDir = rGlobalFile.projectHome ();
		// auto projectName = rGlobalFile.projectName ();
		auto  strPrjConfig = rGlobalFile.configClassName ();
		// create_directories (strPrjConfig);
		cmdOs<<"projectDir="<<projectDir<<"/gen projectName="<<strPrjConfig<<" className="<<strPrjConfig;
		rInfo(" will exc : "<<cmdOs.str());
		system (cmdOs.str().c_str());
    } while (0);
    return nRet;
}

int   globalGen:: secondGen ()
{
    int   nRet = 0;
    do {
    } while (0);
    return nRet;
}

int   globalGen:: makePath ()
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton <globalFile>::single ();
		auto szProject = rGlobalFile.projectHome ();
		std::string build = szProject;
		build += "/build";
		// rInfo ("will mkdir : "<<szProject);
		nRet = myMkdir (build.c_str());
    } while (0);
    return nRet;
}

