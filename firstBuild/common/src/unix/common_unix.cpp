#include <unistd.h>
#include <dlfcn.h>
//#include <dirent.h>
#include "comFun.h"
#include "strFun.h"
#include "depOSFun.h"
#include <sys/stat.h>
#include <sys/types.h>

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
	return mkdir(szPath, S_IRWXU | S_IRWXG | S_IRWXO);
}

void* loadDll (const char* szFile)
{
	void* pRet = nullptr;
	auto szName = szFile;
	pRet = dlopen(szName, RTLD_NOW | RTLD_LOCAL);
	return pRet;
}

void* getFun (void* handle, const char* szFunName)
{
	void* pRet = nullptr;
	pRet = (dlsym(handle, szFunName));
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
	Dl_info info;
	int rc = dladdr((void*)fun1, &info);
	auto nL = strlen (info.dli_fname);
	strCpy (info.dli_fname, pathBuf);
	return 0;
}

bool isPathInterval (char c)
{
	return  c == '/';
}

int   createUuid (char* outPut)
{
    int   nRet = 0;
    do {
    } while (0);
    return nRet;
}
const char*  dllExtName ()
{
	return ".so";
}

const char*  exeExtName ()
{
	return "";
}

void wcsNCpy(wchar_t* szD, int nSize, const wchar_t* szS)
{
	wcsncpy (szD, szS, nSize);
	szD[nSize - 1] = 0;
}
