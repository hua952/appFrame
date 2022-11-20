#include <windows.h>
#include <iostream>
#include "myAssert.h"

int loadLevel0(const char* levelName, int cArg, const char* argS[])
{
	int nRet = 0;
	//auto handle = dlopen(levelName, RTLD_LAZY| RTLD_LOCAL);
	auto handle = LoadLibraryA(levelName);
	//myAssert(handle);
	auto& ws = std::cout;
	do
	{
		if(!handle) {
			nRet = 11;
			ws<<"load module "<<levelName<<" error"<<std::endl;
			break;
		}
		typedef int (*initFunType) (int cArg, const char* argS[]);
		//auto funOnLoad = (initFunType)(dlsym(handle, "initFun"));
		auto funOnLoad = (initFunType)(GetProcAddress(handle, "initFun"));
		myAssert(funOnLoad);
		if(!funOnLoad)
		{
			std::cout<<"Level0 initFun empty error is"<<std::endl;
			nRet = 12;
			break;
		}
		funOnLoad (cArg, argS);
	}while(0);
	return nRet;
}
