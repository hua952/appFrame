/*
#include "main.h"
#include "comFun.h"
#include "strFun.h"
#include <string.h>
#include <sstream>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include "myAssert.h"
#include "modelLoder.h"

int  beginMain(int argC, char** argV);
void endMain();
int loadLevel0(const char* levelName, int cArg, const char* argS[]);
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
		std::string pLevel0Name;
		std::string strFrameHome;
		for (int i = 1; i < cArg; i++)
		{
			auto pArg = argS[i];
			ws << "proc " << pArg << std::endl;
			auto nArgL = strlen(pArg);
			std::unique_ptr<char[]>	 buf = std::make_unique<char[]>(nArgL + 1);
			auto pBuf = buf.get();
			strcpy(pBuf, pArg);
			char* pRetBuf[3];
			auto nNum = strR(pBuf, '=', pRetBuf, 3);
			if (2 != nNum)
			{
				ws << "2 != nNum = " << nNum << std::endl;
				continue;
			}
			std::stringstream sst(pRetBuf[0]);
			std::string strKey;
			sst>>strKey;
			if (strKey == "level0")
			{
				ws << "level0 find" << std::endl;
				pLevel0Name = pRetBuf[1];
			} else if (strKey == "frameHome"){
				strFrameHome = pRetBuf[1];
			}
		}
		if (pLevel0Name.empty()) {
			std::cout<<"LevelName empty"<<std::endl;
			nRet = 1;
		}
		else {
			std::string strDll;
			if (!strFrameHome.empty()) {
				strDll += strFrameHome;
				strDll += "/bin/";
			}
			strDll += pLevel0Name;
			strDll += dllExtName ();
			auto handle = loadDll (strDll.c_str());
			auto& ws = std::cout;
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
				funOnLoad (cArg, argS);
			}while(0);
		}
		endMain();
	} while (0);
	ws<<" exit main"<<std::endl;
	return nRet;
}
*/
#include "comFun.h"
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
			procArgS.push_back("netLib=libeventSession");
			procArgS.push_back("workDir=C:/work/testSysMsg//../testSysMsgInstall/bin/");
			procArgS.push_back("logLevel=2");
			procArgS.push_back("level0=cppLevel0L");
		std::string pLevel0Name;
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
				if (setMut.find (strKey) == setMut.end()) {
					kvMap.insert(std::make_pair(strKey, pRetBuf[1]));
				} else {
					vArgS.push_back(*it);
				}
			} else {
				vArgS.push_back(*it);
			}
		}
		for (auto it = kvMap.begin(); kvMap.end() != it; ++it) {
			std::string str = it->first;
			str+="=";
			str+=it->second;
			vArgS.push_back (str);
		}
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
				auto nnR = funOnLoad (reArgNum, reArgS.get());
				if (nnR) {
					std::cout<<"funOnLoad error nnR = "<<nnR<<std::endl;
					break;
				}
					
			}while(0);
		}
		endMain();
	} while (0);
	std::cout<<" on end app"<<std::endl;
	return nRet;
}
