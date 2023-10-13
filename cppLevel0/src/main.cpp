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
// static std::unique_ptr<char[]> g_home;
int main(int cArg, char** argS)
{
	auto& ws = std::cout;
	//std::this_thread::sleep_for(std::chrono:: milliseconds(11000));
	ws<<"start main"<<std::endl;
	int nRet = 0;
	nRet = beginMain(cArg, argS);
	do {
		if (nRet) {
			ws<<" beginMain error nRet = "<<nRet<<std::endl;
			break;
		}
		std::string pLevel0Name;
		// auto nHomeR = getCurModelPath (g_home);
		// myAssert (0 == nHomeR);
		// ws<<"home is : "<<g_home.get()<<std::endl;
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
			// g_home.reset ();
			auto handle = loadDll (strDll.c_str());
			//myAssert(handle);
			auto& ws = std::cout;
			do {
				if(!handle) {
					nRet = 11;
					//auto las = GetLastError ();
					//ws<<"load module "<<levelName<<" error lastErr = "<<las<<std::endl;
					ws<<"load module "<<pLevel0Name<<" error"<<std::endl;
					break;
				}
				typedef int (*initFunType) (int cArg, char** argS);
				//auto funOnLoad = (initFunType)(dlsym(handle, "initFun"));
				auto funOnLoad = (initFunType)(getFun(handle, "initFun"));
				myAssert(funOnLoad);
				if(!funOnLoad) {
					std::cout<<"Level0 initFun empty error is"<<std::endl;
					nRet = 12;
					break;
				}
				funOnLoad (cArg, argS);
			}while(0);
			//nRet = loadLevel0 (pLevel0Name.c_str(), cArg,  argS);
		}
		endMain();
	} while (0);
	ws<<" exit main"<<std::endl;
	return nRet;
}
