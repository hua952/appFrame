#include "main.h"
#include "strFun.h"
#include <string.h>
#include <memory>

int loadLevel0(const char* levelName, int cArg, const char* argS[]);
int main(int cArg, const char* argS[])
{
	std::cout<<"start main"<<std::endl;
	int nRet = 0;
	std::string pLevel0Name;
	for (int i = 1; i < cArg; i++)
	{
		auto pArg = argS[i];
		auto nArgL = strlen(pArg);
		std::unique_ptr<char[]>	 buf = std::make_unique<char[]>(nArgL + 1);
		auto pBuf = buf.get();
		strcpy(pBuf, pArg);
		char* pRetBuf[3];
		auto nNum = strR(pBuf, '=', pRetBuf, 3);
		if (2 != nNum)
		{
			continue;
		}
		if (0 == strcmp(pRetBuf[0], "level0"))
		{
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
