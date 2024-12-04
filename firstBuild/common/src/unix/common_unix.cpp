#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>
//#include <dirent.h>
#include "comFun.h"
#include "strFun.h"
#include "depOSFun.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <random>
#include <iomanip>
#include "myAssert.h"
#include "modelLoder.h"
#include <iostream>
#include<filesystem>
using namespace std::filesystem;

bool  isPathExit (const char* szPath)
{
	return	access(szPath,R_OK) ==0;
}

int strCaseCmp (const char* str1, const char* str2)
{
	return strcasecmp (str1, str2);
}

int  myMkdir (const char* szPath)
{
	create_directories (szPath);
	// return mkdir(szPath, S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
}

void* loadDll (const char* szFile)
{
	void* pRet = nullptr;
	auto szName = szFile;
	pRet = dlopen(szName, RTLD_NOW | RTLD_LOCAL);
	// pRet = dlopen(szName, RTLD_LAZY|RTLD_LOCAL|RTLD_DEEPBIND);
	// pRet = dlopen(szName, RTLD_LAZY|RTLD_GLOBAL|RTLD_DEEPBIND);
	if (!pRet) {
		std::cout<<" load dll fail error : "<< dlerror()<<std::endl;
	}
	return pRet;
}

void* getFun (void* handle, const char* szFunName)
{
	void* pRet = nullptr;
	pRet = (dlsym(handle, szFunName));
	if (!pRet) {
		std::cout<<" getFun fail error : "<< dlerror()<<std::endl;
	}
	return pRet;
}

int   unloadDll (void* handle)
{
	int nRet = 0;
	dlclose(handle);
	return nRet;
}

void fun1()
{
}

int             getCurModelPath (std::unique_ptr<char[]>& pathBuf)
{
	/*
	Dl_info info;
	int rc = dladdr((void*)fun1, &info);
	auto nL = strlen (info.dli_fname);
	strCpy (info.dli_fname, pathBuf);
	return 0;
	*/
	const auto pMAX = 256;
	char exePath[pMAX];
    ssize_t count = readlink("/proc/self/exe", exePath, pMAX);
	myAssert(0 < count && count < pMAX);
    if (count < 0 || count >= pMAX) {
        return 1;
    }
    exePath[count] = '\0'; // Null-terminate the string
	strCpy(exePath, pathBuf);
	return 0;
}

bool isPathInterval (char c)
{
	return  c == '/';
}

const char*  dllExtName ()
{
	return ".so";
}

const char*  exeExtName ()
{
	return "";
}

const char*  dllInstallDir ()
{
	return "lib";
}

const char*  dllPerName ()
{
	return "lib";
}

void wcsNCpy(wchar_t* szD, int nSize, const wchar_t* szS)
{
	wcsncpy (szD, szS, nSize);
	szD[nSize - 1] = 0;
}
