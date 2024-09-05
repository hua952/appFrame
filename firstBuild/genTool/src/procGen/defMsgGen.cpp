#include "defMsgGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
#include "strFun.h"
#include "comFun.h"
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
#include <sstream>
#include "myAssert.h"
#include "rLog.h"
#include "mainLoop.h"
#include "msgGen.h"

defMsgGen:: defMsgGen ()
{
}

defMsgGen:: ~defMsgGen ()
{
}

int defMsgGen::loopHandleSGen ()
{
    int  nRet = 0;
    do {
		std::stringstream ssTmp;
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
		
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		auto& rModMgr = tSingleton<moduleFileMgr>::single ();
		int ip = 0;
		std::stringstream ssTem;
		auto pSSRoot = std::make_unique<std::stringstream>();
		auto& ssRoot = *pSSRoot;
		std::stringstream appTem;
		appTem<<R"(
enum appTmpID
{
)";
		std::stringstream  appOpenSS;
		bool bFirst = true;
		int  appIndex = 0;
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());
			int is = ip++ * LoopNum;
			rApp.setProcId (is);
			auto netType = rApp.netType ();
			auto& rModNameS = rApp.moduleFileNameS ();
			int mutI = 0;
			int sinI = 0;
			int lvI[c_serverLevelNum];
			for (int i = 0; i < c_serverLevelNum; i++) {
				lvI[i] = 0;
			}
			appTem<<"    appTmpId_"<<rApp.appName()<<","<<std::endl;

			std::stringstream appThTem;
			std::stringstream routeThTem;
			appThTem<<R"(enum )"<<rApp.appName()<<R"(ServerTmpID
{
)"<<std::endl;
			if (bFirst) {
				bFirst = false;
			} else {
				appOpenSS<<"+";
			}
			appOpenSS<<"appId:"<<appIndex++<<"*openNum:1";
			for (auto ite = rModNameS.begin ();
					rModNameS.end () != ite; ++ite) {
				auto& rMName = *ite;
				auto pMod = rModMgr.findModule (rMName.c_str ());
				myAssert (pMod);
				auto& rSS = pMod->orderS ();
				auto tmpId = 0;
				bool firstServer = true;
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
					if (route) {
						routeThTem<<rApp.appName()<<R"(ServerTmpID_)"<<pServer->serverName();
					} else {
						appThTem<<R"(    )"<<rApp.appName()<<R"(ServerTmpID_)"<<pServer->serverName()<<","<<std::endl;
						if (firstServer) {
							appOpenSS<<"*runServer:";
							firstServer = false;
						} else {
							appOpenSS<<"-";
						}
						appOpenSS<<tmpNum<<"/1";
					}
					if (appNetType_gate == netType && route) {
						myAssert (ssRoot.str().empty());
						ssRoot<<R"(#define  c_rootTmp )"<<pTmpHandle<<std::endl;
					}
					tmpId++;
				}
			}

			appThTem<<R"(    )"<<routeThTem.str()<<","<<std::endl;
			appThTem<<R"(    )"<<rApp.appName()<<R"(ServerTmpID_serverTmpNum
};
)";
	os<<appThTem.str()<<std::endl;
	os<<R"(#define )"<<rApp.appName()<<R"(RouteServerTmp )"<<routeThTem.str()<<std::endl;
	}
appTem<<R"(    appTmpId_appTmpNum
};
)";

		auto& rGlobal = tSingleton<globalFile>::single ();
		rGlobal.setAppRunWorkNum (appOpenSS.str().c_str());
		os<<appTem.str()<<std::endl;
		os<<ssTem.str()<<std::endl<<ssRoot.str()<<std::endl;

		os<<R"(#endif)";
	} while (0);
    return nRet;
}

int  defMsgGen:: startGen ()
{
    int  nRet = 0;
    do {
		int nR = 0;
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto pDefPmp = rGlobal.findMsgPmp ("defMsg");
		myAssert (pDefPmp);
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		strFilename += "/defMsg/src";
		myMkdir (strFilename.c_str ());
		nR = mkDir ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = loopHandleSGen ();
		if (nR) {
			nRet = 7;
			break;
		}
    } while (0);
    return nRet;
}

const char*  defMsgGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  defMsgGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

int  defMsgGen:: mkDir ()
{
    int  nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		strFilename += "/defMsg/src";
		setSrcDir (strFilename.c_str ());
		myMkdir (strFilename.c_str ());
    } while (0);
    return nRet;
}

