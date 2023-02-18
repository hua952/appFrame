#include <unistd.h>
//#include <dirent.h>
#include "comFun.h"
#include "strFun.h"
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

int             getCurDllPath (std::unique_ptr<char[]>& pathBuf)
{
}

