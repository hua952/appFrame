#include "moduleMgrGen.h"
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/appFile.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/msgPmpFile.h"
#include "tSingleton.h"
#include "rLog.h"
#include "configMgr.h"
#include <string>
#include <fstream>
#include <vector>

moduleMgrGen:: moduleMgrGen ()
{
}

moduleMgrGen:: ~moduleMgrGen ()
{
}

int   moduleMgrGen:: startGen (appFile& rApp)
{
    int   nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strProc = thisRoot ();
		strProc += "/src/proc";
		setProcSrcDir (strProc.c_str());
		int nR = 0;
		nR = myMkdir (strProc.c_str());
		std::string strDir = thisRoot ();
		strDir += "/src/gen";
		setGenSrcDir (strDir.c_str ());
		std::string strWin = strDir;
		strWin += "/win";
		nR = myMkdir (strWin.c_str());
		std::string strUnix = thisRoot ();
		strUnix += "/src/unix";
		nR = myMkdir (strUnix.c_str());
		std::string strRWin = thisRoot ();
		strRWin += "/src/win";
		nR = myMkdir (strRWin.c_str());
		nR = writeCMakeLists (rApp);
		if (nR) {
			rError (" writeCMakeLists return error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = writeDefFile ();
		if (nR) {
			rError ("writeDefFile error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = writeExportFunH ();
		if (nR) {
			rError ("writeExportFunH error nR = "<<nR);
			nRet = 4;
			break;
		}
		nR = writeExportFunCpp ();
		if (nR) {
			rError ("writeExportFunCpp error nR = "<<nR);
			nRet = 5;
			break;
		}
		nR = writeLogicServerH ();
		if (nR) {
			rError ("writeLogicServerH error nR = "<<nR);
			nRet = 6;
			break;
		}
		nR = writeLogicServerCpp ();
		if (nR) {
			rError ("writeLogicServerCpp error nR = "<<nR);
			nRet = 7;
			break;
		}
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeCMakeLists (appFile& rApp)
{
	int   nRet = 0;
	do {
		std::string fileName = thisRoot ();
		fileName += "/CMakeLists.txt";
		auto bE = isPathExit (fileName.c_str());
		if (bE) {
			break;
		}
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto& rConfig = tSingleton<configMgr>::single ();
		auto structBadyType = rConfig.structBadyType ();
		std::string strProtobufSer;
		if (structBadyTime_proto == structBadyType) {
			strProtobufSer = "protobufSer";
		}
		std::string mgrName = rApp.appName ();
		mgrName += "ModuleMgr";
os<<"SET(prjName "<<mgrName<<")"<<std::endl;
	// <<"SET(CMAKE_INSTALL_PREFIX "<<installPath<<")"<<std::endl;
		const char* szCon = R"(set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp src/proc/*.cpp)
set(defS)
set(osSrc)
set(libPath)
set(libDep)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc src/unix/*.cpp)
elseif (WIN32)
	
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)

	file(GLOB defS src/gen/win/*.def)
	file(GLOB osSrc src/win/*.cpp)
	include_directories()";
	auto depInc = rGlobalFile.depIncludeHome ();
	os<<szCon<<depInc<<")"<<std::endl;
	auto depLib = rGlobalFile.depLibHome ();

	os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()

	include_directories()";
	//auto framePath = rGlobalFile.frameHome ();
	std::string frameInPath =  rGlobalFile.frameInstallPath ();
	auto prjName = rGlobalFile.projectName ();
	os<<szC2<<std::endl
	<<"    ${CMAKE_SOURCE_DIR}/defMsg/src"<<std::endl
	<<"    "<<frameInPath<<"include/appFrame"<<std::endl
	
	<<")"<<std::endl;

	auto libPath = rGlobalFile.frameLibPath ();
	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl
	<<R"(link_directories(${libPath} ${libDep})
	add_library(${prjName} SHARED ${genSrcS} ${defS} ${osSrc})
	target_link_libraries(${prjName} PUBLIC
	common
	)"<<strProtobufSer<<R"(
	logicCommon
	cLog
	)";
	auto bH = rGlobalFile.haveServer ();
	if (bH) {
		os<<R"(defMsg
		)";
	}
	os<<R"(
	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	)";
	} while (0);
    return nRet;
}

const char*  moduleMgrGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  moduleMgrGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}

int   moduleMgrGen:: writeDefFile ()
{
    int   nRet = 0;
    do {
		std::string strDef = genSrcDir();
		strDef += "/win/mgr.def";
		std::ofstream os(strDef.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strDef.c_str ());
			break;
		}
		os<<R"(LIBRARY
EXPORTS
	afterLoad	@1
	logicOnAccept	@2
	logicOnConnect  @3
	onLoopBegin		@4
	onLoopEnd		@5
	beforeUnload	@6)";
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeExportFunH ()
{
    int   nRet = 0;
    do {
		std::string strExport = genSrcDir ();
		strExport += "/exportFun.h";
		std::ofstream os(strExport.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strExport.c_str ());
			break;
		}
		os<<R"(#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"
class logicServer;
extern "C"
{
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
	void  beforeUnload();

	void onLoopBegin	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
}
#endif)";
    } while (0);
    return nRet;
}

int   moduleMgrGen:: writeExportFunCpp ()
{
    int   nRet = 0;
    do {
		std::string strExport = genSrcDir ();
		strExport += "/exportFun.cpp";
		std::ofstream os(strExport.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strExport.c_str ());
			break;
		}
		os<<R"(#include <iostream>
#include "exportFun.h"
#include "myAssert.h"
#include "allLogicServerMgr.h"
#include "tSingleton.h"
#include "msg.h"
#include "cLog.h"
)";

		auto& rGlobal = tSingleton<globalFile>::single ();
		auto bH = rGlobal.haveServer ();
		if (bH) {

			os<<R"(#include "rpcInfo.h"
)";
		}
os<<R"(
dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	setForMsgModuleFunS (pForLogic);
	)";
	os<<R"(	tSingleton<allLogicServerMgr>::createSingleton();
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.afterLoad(nArgC, argS, pForLogic);
}
void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.logicOnAccept(fId, sessionId, userData);
}

void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.logicOnConnect(fId, sessionId, userData);
}

void onLoopBegin	(serverIdType	fId)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.onLoopBegin	(fId);
}

void onLoopEnd	(serverIdType	fId)
{
	auto &rMgr = tSingleton<allLogicServerMgr>::single();
	return rMgr.onLoopEnd(fId);
}

void  beforeUnload()
{
})";
    } while (0);
    return nRet;
}

const char*  moduleMgrGen:: genSrcDir ()
{
    return m_genSrcDir.get ();
}

void  moduleMgrGen:: setGenSrcDir (const char* v)
{
    strCpy (v, m_genSrcDir);
}

int   moduleMgrGen:: writeLogicServerH ()
{
    int   nRet = 0;
	do {
		std::string fileName = genSrcDir ();
		fileName += "/allLogicServerMgr.h";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		os<<R"(#ifndef _allLogicServerMgr_h_
#define  _allLogicServerMgr_h_
#include <memory>
#include "mainLoop.h"
#include <string>
#include <map>
#include "arrayMap.h"

class logicServer;
class allLogicServerMgr
{
public:
	struct moduleInfo
	{
		void* handle;
		afterLoadFunT fnAfterLoadFun;
		logicOnAcceptFT  fnLogicOnAccept;
		logicOnConnectFT  fnLogicOnConnect;
		beforeUnloadFT fnBeforeUnload;
		onLoopBeginFT  fnOnLoopBegin;
		onLoopEndFT  fnOnLoopEnd;
	};
	using moduleMap = std::map<std::string, moduleInfo>;
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
	moduleMap&  moduleS ();
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);

	void onLoopBegin	(serverIdType	fId);
	void onLoopEnd	(serverIdType	fId);
private:
	moduleMap  m_moduleS;
	ForLogicFun m_ForLogicFun;
};
#endif)";
	} while (0);
    return nRet;
}


int   moduleMgrGen:: writeLogicServerCpp ()
{
    int   nRet = 0;
    do {
		auto& rGlobal = tSingleton<globalFile>::single ();
		auto pPmp = rGlobal.findMsgPmp ("defMsg");
		auto serializePackFunStName = pPmp->serializePackFunStName ();

		std::string fileName = genSrcDir ();
		std::string logicFile = procSrcDir();
		logicFile += "/logicFile.cpp";
		auto bE = isPathExit (logicFile.c_str());
		if (!bE) {
			std::ofstream lf (logicFile.c_str());
			lf<<R"(int initLogic(int argNum, char** argS)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}
	)";
		}
		fileName += "/allLogicServerMgr.cpp";
		std::ofstream os(fileName.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<fileName.c_str ());
			break;
		}
		auto& rConfig = tSingleton<configMgr>::single ();
		auto structBadyType = rConfig.structBadyType ();
		std::stringstream strProtobufSerSS;
		std::string strProtoSerInc;

		auto bH = rGlobal.haveServer ();
		if (bH) {
			strProtoSerInc = R"(#include ")";
			strProtoSerInc += serializePackFunStName;
			strProtoSerInc += R"(.h")";
			strProtobufSerSS<<R"(	static )"<<serializePackFunStName<<R"(   s_FunS;
			int  getSerializeFunS ()"<<serializePackFunStName<<R"(* pFunS, ForLogicFun* pForLogic);
			getSerializeFunS (&s_FunS, pForLogic);
			auto serNum = sizeof (s_FunS) / sizeof (pSerializePackFunType3);
			if (serNum) {
				s_SerFunSet.init ((pSerializePackFunType3*)(&s_FunS), serNum);
			}
			m_ForLogicFun.pSerFunSPtr = &s_FunS;
			pForLogic->fromNetPack = sFromNetPack;
			pForLogic->toNetPack = sToNetPack;
	)";
		}

		os<<R"(#include <iostream>
#include <string>
#include "allLogicServerMgr.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
)";
if (bH) {
	os<<strProtoSerInc<<std::endl;
	os<<R"(#include "loopHandleS.h"
#include "rpcInfo.h"
)";
}
os<<R"(
#include "strFun.h"
#include "comFun.h"
#include "modelLoder.h"
#include "mainLoop.h"
#include <string>
#include <sstream>
#include <vector>

// typedef serializePackFunType pSerializePackFunType3[3];
struct pSerializePackFunType3
{
	serializePackFunType f[3];
};

class pSerializePackFunType3Cmp 
{
public:
	bool operator () (const pSerializePackFunType3& a, const pSerializePackFunType3& b) const
	{
		return a.f[0] < b.f[0];
	}
};

using  msgSerFunSet = arraySet<pSerializePackFunType3, pSerializePackFunType3Cmp>;
static msgSerFunSet s_SerFunSet;

static pSerializePackFunType3* sGetNode (udword msgId)
{
	auto & rArr = s_SerFunSet;
	pSerializePackFunType3 temp;
	temp.f[0] = (serializePackFunType)msgId;
	auto pRet = rArr.GetNode (temp);
	return pRet;
}

static int sFromNetPack (packetHead* p, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		myAssert (p);
		auto pN = P2NHead(p);
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[1];
			if (fun) {
				auto nR = fun (p, pNew);
				if (nR) {
					nRet = 2;
				}
			}
		}
	} while (0);
	return nRet;
}

static int sToNetPack (packetHead* p, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		myAssert (p);
		auto pN = P2NHead(p);
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[2];
			auto nR = fun (p, pNew);
			if (nR) {
				nRet = 2;
			}
		}
	} while (0);
	return nRet;
}

void getModelS (int nArgC, char** argS, std::vector<std::string>& vModelS,
	std::string& strWorkDir, bool& dumpMsg, std::unique_ptr<char[]>& checkMsg)
{
	char* pRetBuf[3];
	for (int i = 1; i < nArgC; i++) {
		auto pa = argS [i];
		auto nL = strlen (pa);
		auto pB = std::make_unique<char[]> (nL + 1);
		auto pBuf = pB.get ();
		strNCpy (pBuf, nL + 1, pa);
		auto nNum = strR(pBuf, '=', pRetBuf, 3);
		if (2 != nNum) {
			gWarn("arg num error"<< "2 != nNum = " << nNum);
			continue;
		}
		std::stringstream ssKey (pRetBuf[0]);
		std::string strKey;
		ssKey>>strKey;
		if (strKey == "logicModel") {
			vModelS.push_back (pRetBuf[1]);
		} else if (strKey == "workDir") {
			strWorkDir = pRetBuf[1];
		} else if (strKey == "dumpMsg") {
			dumpMsg = atoi (pRetBuf[1]);
		} else if (strKey == "checkMsg") {
			strCpy(pRetBuf[1], checkMsg);
		}
	}
}

allLogicServerMgr::moduleMap& allLogicServerMgr:: moduleS ()
{
	return m_moduleS;
}

void allLogicServerMgr::logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnLogicOnAccept(fId, sessionId, userData);
	}
}

void allLogicServerMgr::logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnLogicOnConnect(fId, sessionId, userData);
	}
}

void allLogicServerMgr::onLoopBegin	(serverIdType	fId)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnOnLoopBegin(fId);
	}
}

void allLogicServerMgr::onLoopEnd(serverIdType	fId)
{
	auto& modS = moduleS ();
	for (auto it = modS.begin(); modS.end() != it; ++it) {
		it->second.fnOnLoopEnd(fId);
	}
}

dword allLogicServerMgr::afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	dword nRet = 0;
	m_ForLogicFun = *pForLogic;
	)"<<strProtobufSerSS.str()<<R"(
	auto pForMsg = &m_ForLogicFun;
	// auto& rFunS = getForMsgModuleFunS();
	// rFunS = *pForLogic;
)";
	if (bH) {
		os<<R"(regRpcS (pForLogic);
)";
	}
os<<R"(
	std::vector<std::string> vModelS;
	std::string strWorkDir;
	bool dumpMsg = false;
	std::unique_ptr <char[]> checkMsg;
	getModelS (nArgC, argS, vModelS, strWorkDir, dumpMsg, checkMsg);
	gInfo ("logicModelNum = "<<vModelS.size());
	do {
		int initLogic(int argNum, char** argS);
		auto nR = initLogic (nArgC, argS);
		if (nR) {
			nRet = 5;
			break;
		}
)";
	if (bH) {
os<<R"(
		if (dumpMsg) {
			dumpStructS ();
			break;
		}
		if (checkMsg) {
			auto nR = checkStructS (checkMsg.get());
			if (nR) {
				gError ("checkStructS ret error nR = " <<nR);
				nRet = 1;
				break;
			}
		}
)";
	}
		os<<R"(auto& modS = moduleS ();
		for (auto it = vModelS.begin (); vModelS.end () != it; ++it) {
			std::string strDll = strWorkDir;
			strDll += "/bin/";
			strDll += *it;
			strDll += dllExtName();
			gInfo ("will load "<<strDll.c_str ());
			moduleInfo info;
			info.handle = loadDll (strDll.c_str());
			if (!info.handle) {
				gError ("load dll fail fileName = "<<strDll.c_str());
				nRet = 2;
				break;
			}
			info.fnAfterLoadFun = (afterLoadFunT)(getFun(info.handle, "afterLoad"));
			if (!info.fnAfterLoadFun) {
				gError ("get fun afterLoad fail");
				nRet = 3;
				break;
			}
			info.fnLogicOnAccept= (logicOnAcceptFT)(getFun(info.handle, "logicOnAccept"));
			if (!info.fnLogicOnAccept) {
				gError ("get fun fnLogicOnAccept fail");
				nRet = 4;
				break;
			}
			info.fnLogicOnConnect = (logicOnConnectFT)(getFun(info.handle, "logicOnConnect"));
			if (!info.fnLogicOnConnect) {
				gError ("get fun fnLogicOnConnect fail");
				nRet = 5;
				break;
			}
			info.fnBeforeUnload = (beforeUnloadFT)(getFun(info.handle, "beforeUnload"));
			if (!info.fnBeforeUnload) {
				gError ("get fun fnFindServer fail");
				nRet = 6;
				break;
			}
			info.fnOnLoopBegin = (onLoopBeginFT)(getFun(info.handle, "onLoopBegin"));
			if (!info.fnOnLoopBegin) {
				gWarn ("fun onLoopBegin empty error is");
				nRet = 7;
				break;
			}
			info.fnOnLoopEnd = (onLoopEndFT)(getFun(info.handle, "onLoopEnd"));
			if (!info.fnOnLoopEnd) {
				gWarn ("fun onLoopEnd empty error is");
				nRet = 8;
				break;
			}
			gInfo ("befor call onLoad");
			info.fnAfterLoadFun(nArgC, argS, pForMsg);
			gInfo ("after call onLoad");
			auto inRet = modS.insert(std::make_pair(strDll, info));
			myAssert(inRet.second);
		}
	} while (0);
    return nRet;
})";
    } while (0);
    return nRet;
}

const char*  moduleMgrGen:: procSrcDir ()
{
    return m_procSrcDir.get ();
}

void  moduleMgrGen:: setProcSrcDir (const char* v)
{
    strCpy (v, m_procSrcDir);
}

