#include "mainGen.h"
#include "strFun.h"
#include "fromFileData/appFile.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/serverFile.h"
#include "comFun.h"
#include "appGen.h"
#include "tSingleton.h"
#include "rLog.h"
#include <string>
#include <fstream>

mainGen:: mainGen ()
{
}

mainGen:: ~mainGen ()
{
}

int  mainGen:: startGen (appFile& rApp)
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string projectHome = rGlobalFile.projectHome ();
		std::string strAppPath = projectHome;
		auto szAppName = rApp.appName ();
		strAppPath += szAppName;
		std::string strFile = strAppPath;
		strFile += R"(/main)";
		myMkdir (strFile.c_str ());
		setThisRoot (strFile.c_str ());
		strFile += R"(/src)";
		myMkdir (strFile.c_str());
		setSrcDir (strFile.c_str());
		auto strFileU = strFile;
		strFileU += R"(/unix)";
		myMkdir (strFileU.c_str());
		strFile += R"(/win)";
		myMkdir (strFile.c_str());
		auto pL = userLogicDir();
		myMkdir (pL);
		auto nR = writeCmake (rApp);
		if (nR) {
			rError ("writeCmake error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = writeUnixDep ();
		if (nR) {
			rError ("writeUnixDep error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = writeWinDep ();
		if (nR) {
			rError ("writeWinDep error nR = "<<nR);
			nRet = 3;
			break;
		}
		nR = writeMain (rApp);
		if (nR) {
			rError ("writeMain error nR = "<<nR);
			nRet = 4;
			break;
		}
		nR = writeUserLogic ();
		if (nR) {
			rError ("writeUserLogic error nR = "<<nR);
			nRet = 4;
			break;
		}
    } while (0);
    return nRet;
}

int  mainGen:: writeUserLogic ()
{
    int  nRet = 0;
    do {
		auto pDir = userLogicDir();
		std::string strFile = pDir;
		strFile += R"(/userLogic.cpp)";
		auto bE = isPathExit (strFile.c_str());
		if (bE) {
			break;
		}
		std::ofstream os(strFile.c_str());
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(
#include<cstdlib>
#include<cstdio>
#include<vector>
#include<string>
#include<memory>

int  initUserLogic (std::vector<std::string>& vArgS)
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

int  mainGen:: writeCmake (appFile& rApp)
{
    int  nRet = 0;
    do {
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		std::string strFile = thisRoot ();
		strFile += "/CMakeLists.txt";
		auto bE = isPathExit (strFile.c_str());
		if (bE) {
			break;
		}
		std::ofstream os(strFile.c_str());
		std::string strExcName = rApp.appName ();
		strExcName += "Exc";
		os<<R"(SET(prjName )"<<strExcName<<R"()
set(srcS)
file(GLOB srcS src/*.cpp src/userLogic/*.cpp)
set(libS common)
set(osSrc)
set(libPath)
set(libDep)
if (UNIX)
    MESSAGE(STATUS "unix")
	#add_definitions(-w)
	file(GLOB osSrc src/unix/*.cpp)
elseif (WIN32)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W3)

	list(APPEND libS ws2_32)
	file(GLOB osSrc src/win/*.cpp)
endif ()
)";

	auto bH = rGlobalFile.haveServer ();
	auto libPath = rGlobalFile.frameLibPath ();
	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl;

os<<R"(link_directories(${libPath} ${libDep})
add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							src
							)";
							if (bH) {
								os<<R"(${CMAKE_SOURCE_DIR}/defMsg/src)";
							}
							os<<R"(
							)";
	auto frameInPath = rGlobalFile.frameInstallPath ();
	// auto prjName = rGlobalFile.projectName ();
		//auto framePath = rGlobalFile.frameHome ();
		os<<frameInPath<<"include/appFrame"<<std::endl
		/*
	<<"    "<<framePath<<"common/src"<<std::endl
	<<"    "<<framePath<<"common/src"<<std::endl<<R"(
	*/
	<<R"(
                           )

target_link_libraries(${prjName} PUBLIC  ${libS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
)";
    } while (0);
    return nRet;
}

int  mainGen:: writeUnixDep ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/unix";
		strFile += "/depSys_unix.cpp";
		std::ofstream os (strFile.c_str());
		os<<R"(int   beginMain(int argC, char** argV)
{
    int   nRet = 0;
    do {
    } while (0);
    return nRet;
}

void endMain()
{
    do {
    } while (0);
})";
    } while (0);
    return nRet;
}

int  mainGen:: writeMain (appFile& rApp)
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/main.cpp";
		auto bE = isPathExit (strFile.c_str());
		if (bE) {
			// break;
		}
		std::ofstream os(strFile.c_str());
		os<<R"(#include "comFun.h"
#include "strFun.h"
#include <string.h>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include "myAssert.h"
#include "modelLoder.h"
)";

	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto  bHave = rGlobalFile.haveServer ();
	if (bHave) {
		os<<R"(#include "loopHandleS.h"
)";
	}
os<<R"(
int  beginMain(int argC, char** argV);
void endMain();
int main(int cArg, char** argS)
{
	auto& ws = std::cout;
	ws<<"start main"<<std::endl;
	int nRet = 0;
	nRet = beginMain(cArg, argS);
	do {
		if (nRet) {
			ws<<" beginMain error nRet = "<<nRet<<std::endl;
			break;
		}
		std::set<std::string> setMut;
		setMut.insert("logicModel");
		setMut.insert("endPoint");
		using argVector = std::vector<std::string>;
		argVector vArgS;
		argVector  procArgS;
		for (decltype (cArg) i = 0; i < cArg; i++) {
			procArgS.push_back(argS[i]);
		}
		)";
		auto& rMainArgS = rApp.mainArgS();
		auto  mainArgSize = rMainArgS.size();
		os<<R"(auto defArgS = std::make_unique<char*[]>()"<<mainArgSize + 1<<R"();
		)";
		auto defLen = 0;
		for (auto it = rMainArgS.begin(); rMainArgS.end() != it; ++it) {
			defLen += it->length() + 1;
		}
		os<<R"(
			auto defArgTxt = std::make_unique<char[]>()"<<defLen<<R"();
			int curDef = 0;
			char* pCur = defArgTxt.get();
		)";
		for (auto it = rMainArgS.begin(); rMainArgS.end() != it; ++it) {
			os<<R"(defArgS[curDef++] = pCur;
		pCur += (sprintf (pCur, "%s", ")"<<it->c_str()<<R"(") + 1);
		)";
		}
		os<<R"(std::string pLevel0Name;
		char* pRetBuf[3];
		std::map<std::string, std::string> kvMap;
		for (auto it = procArgS.begin(); procArgS.end() != it; ++it) {
			std::unique_ptr<char[]> kvBuf;
			strCpy(it->c_str(), kvBuf);
			auto pBuf = kvBuf.get();
			auto nNum = strR(pBuf, '=', pRetBuf, 3);
			if (2 == nNum) {
				std::stringstream ts(pRetBuf[0]);
				std::string strKey;
				ts>>strKey;
				std::stringstream vs(pRetBuf[1]);
				std::string strValue;
				vs>>strValue;
				if (setMut.find (strKey) == setMut.end()) {
					kvMap.insert(std::make_pair(strKey, strValue));
				} else {
					std::string strIn = strKey;
					strIn += "=";
					strIn += strValue;
					vArgS.push_back(strIn);
				}
			} else {
				vArgS.push_back(*it);
			}
		}
		procArgS.clear();
		for (auto it = kvMap.begin(); kvMap.end() != it; ++it) {
			std::string str = it->first;
			str+="=";
			str+=it->second;
			vArgS.push_back (str);
		}
		kvMap.clear();
		auto argBufSize = 0;
		for (auto it = vArgS.begin(); vArgS.end() != it; ++it) {
			argBufSize += it->length();
			argBufSize++;
		}
		auto argBuf = std::make_unique<char[]>(argBufSize);
		auto reArgS = std::make_unique<char*[]>(vArgS.size());
		auto pArgBuf = argBuf.get();
		auto reArgNum = 0;
		for (auto it = vArgS.begin(); vArgS.end() != it; ++it) {
			auto nL = it->length();
			memcpy(pArgBuf, it->c_str(), nL);
			pArgBuf[nL] = 0;
			reArgS[reArgNum++] = pArgBuf;
			pArgBuf += (nL + 1);
		}
		int  initUserLogic (std::vector<std::string>& vArgS);
		int nURet = initUserLogic (vArgS);
		if (nURet) {
			break;
		}
		vArgS.clear();
		std::string strFrameHome;
		for (decltype (reArgNum) i = 1; i < reArgNum; i++) {
			auto pArg = reArgS[i];
			ws << "proc " << pArg << std::endl;
			auto nArgL = strlen(pArg);
			std::unique_ptr<char[]>	 buf = std::make_unique<char[]>(nArgL + 1);
			auto pBuf = buf.get();
			strcpy(pBuf, pArg);
			auto nNum = strR(pBuf, '=', pRetBuf, 3);
			if (2 != nNum) {
				ws << "2 != nNum = " << nNum << std::endl;
				continue;
			}
			std::stringstream sst(pRetBuf[0]);
			std::string strKey;
			sst>>strKey;
			if (strKey == "level0") {
				ws << "level0 find" << std::endl;
				pLevel0Name = pRetBuf[1];
			} else if (strKey == "frameHome"){
				strFrameHome = pRetBuf[1];
			}
		}
		if (pLevel0Name.empty()) {
			std::cout<<"LevelName empty"<<std::endl;
			nRet = 1;
		} else {
			std::string strDll;
			if (!strFrameHome.empty()) {
				strDll += strFrameHome;
				strDll += "/bin/";
			}
			strDll += pLevel0Name;
			strDll += dllExtName ();
			auto handle = loadDll (strDll.c_str());
			myAssert(handle);
			do {
				if(!handle) {
					nRet = 11;
					ws<<"load module "<<pLevel0Name<<" error"<<std::endl;
					break;
				}
				typedef int (*initFunType) (int cArg, char** argS);
				auto funOnLoad = (initFunType)(getFun(handle, "initFun"));
				myAssert(funOnLoad);
				if(!funOnLoad) {
					std::cout<<"Level0 initFun empty error is"<<std::endl;
					nRet = 12;
					break;
				}
				auto nnR = funOnLoad (reArgNum, reArgS.get(), )"<<mainArgSize<<R"(, defArgS.get());
				if (nnR) {
					std::cout<<"funOnLoad error nnR = "<<nnR<<std::endl;
					break;
				}
				)";
				auto bDet = rApp.detachServerS ();
				if (bDet) {
					auto mainLoopServer = rApp.mainLoopServer();
					if (bHave ) {
						myAssert(mainLoopServer);
					}
					if (mainLoopServer) {
					os<<R"(typedef int (*loopBeginFT)(loopHandleType pThis);
	auto funLoopBegin = (loopBeginFT)(getFun(handle, "onPhyLoopBegin"));
	typedef int (*loopEndFT)(loopHandleType pThis);
	auto funLoopEnd = (loopEndFT)(getFun(handle, "onPhyLoopEnd"));
	typedef bool (*loopFrameFT)(loopHandleType pThis);
	auto funLoopFrame = (loopFrameFT)(getFun(handle, "onPhyFrame"));

	typedef bool (*loopFrameFT)(loopHandleType pThis);

	typedef int  (*runThNumFT) (char*, int);
	auto funRunThNum = (runThNumFT)(getFun(handle, "onPhyGetRunThreadIdS"));

	funLoopBegin ()"<<mainLoopServer <<R"();
	while(1) {
		auto bE = funLoopFrame ()"<<mainLoopServer <<R"();
		if (bE) {
			break;
		}
	}
	funLoopEnd ()"<<mainLoopServer <<R"();
	int curRunNum = 0;
	const auto c_tempSize = 256;
	auto tempBuf = std::make_unique<char[]>(c_tempSize);
	auto pTemp = tempBuf.get();
	do {
		curRunNum = funRunThNum (pTemp, c_tempSize);
		if (curRunNum) {
			std::cout<<" leaf run num is: "<<curRunNum<<" run serverS is : "<<pTemp<<std::endl;
			std::this_thread::sleep_for(std::chrono::microseconds (500000));
		}
	} while (curRunNum);
					)";
				}
		}
				os<<R"(
				std::cout<<pTemp<<std::endl;
			}while(0);
		}
		endMain();
	} while (0);
	std::this_thread::sleep_for(std::chrono::microseconds (1000000));
	std::cout<<" app exit now"<<std::endl;
	return nRet;
})";
    } while (0);
    return nRet;
}

int  mainGen:: writeWinDep ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/win";
		strFile += "/depSys_win.cpp";
		std::ofstream os (strFile.c_str());
		os<<R"(#include <Winsock2.h>
#include <windows.h>

int   beginMain(int argC, char** argV)
{
    int   nRet = 0;
    do {
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			nRet = 1;
		}
    } while (0);
    return nRet;
}

void endMain()
{
    do {
    } while (0);
})";
    } while (0);
    return nRet;
}

const char*  mainGen:: thisRoot ()
{
    return m_thisRoot.get ();
}

void  mainGen:: setThisRoot (const char* v)
{
    strCpy (v, m_thisRoot);
}

const char*  mainGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  mainGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
	std::string strL = v;
	strL += R"(/userLogic)";
    strCpy (strL.c_str(), m_userLogicDir);
}

const char*  mainGen:: userLogicDir ()
{
    return m_userLogicDir.get ();
}


