#ifndef _appFileCreator_h_
#define _appFileCreator_h_
#include "constVar.h"

class appFileCreator
{
public:
	appFileCreator(const char* szSrcDir, const char* szAppName);
	dword writeH();
	dword writeCpp();
private:
	char  m_szSrcDir[DirSize];
	char  m_szAppName[NameSize];
};
#endif
