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
