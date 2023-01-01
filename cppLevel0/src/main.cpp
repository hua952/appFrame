#include "main.h"
#include "strFun.h"
#include <string.h>
#include <memory>
#include <thread>
#include <chrono>

int loadLevel0(const char* levelName, int cArg, const char* argS[]);
int main(int cArg, const char* argS[])
{
	auto& ws = std::cout;
	//std::this_thread::sleep_for(std::chrono:: milliseconds(11000));
	ws<<"start main"<<std::endl;
	int nRet = 0;
	std::string pLevel0Name;
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
		if (0 == strcmp(pRetBuf[0], "level0"))
		{
			ws << "level0 find" << std::endl;
			pLevel0Name = pRetBuf[1];
			break;
		}
	}
	if (pLevel0Name.empty())
	{
		std::cout<<"LevelName empty"<<std::endl;
		nRet = 1;
	}
	else
	{
		nRet = loadLevel0 (pLevel0Name.c_str(), cArg,  argS);
	}
	return nRet;
}
