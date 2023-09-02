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
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());
			int is = ip++ * LoopNum;
			rApp.setProcId (is);
			auto& rModNameS = rApp.moduleFileNameS ();
			for (auto ite = rModNameS.begin ();
					rModNameS.end () != ite; ++ite) {
				auto& rMName = *ite;
				auto pMod = rModMgr.findModule (rMName.c_str ());
				myAssert (pMod);

				// auto& rSS = pMod->serverS ();
				auto& rSS = pMod->orderS ();

				for (auto iter = rSS.begin ();
						rSS.end () != iter; ++iter) {
					auto pServer = iter->get (); // iter->second.get ();
					auto pSName = pServer->strHandle ();
					os<<R"(#define  )"<<pSName<<"  "<<is++<<std::endl;
				}
			}
		}
		auto& rGlobal = tSingleton<globalFile>::single ();
		auto& rRootV = rGlobal.rootServerS ();
		os<<R"(static serverIdType s_RootSer[] = {)";
		for (auto it = rRootV.begin(); rRootV.end() != it; ++it) {
			if (it != rRootV.begin()) {
				os<<",";
			}
			os<<*it;
		}
		os<<"};"<<std::endl;
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

