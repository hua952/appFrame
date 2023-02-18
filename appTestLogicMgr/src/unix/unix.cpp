#include "logicServer.h"
#include <dlfcn.h>
#include <memory>
#include "strFun.h"

void fun1()
{
}

int logicServerMgr::getMyPath (std::unique_ptr<char[]>& myPath)
{
	int nRet = 0;
	Dl_info info;
	int rc = dladdr((void*)fun1, &info);
	strCpy(info.dli_fname, myPath);
	return nRet;
}

