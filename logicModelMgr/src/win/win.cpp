#include <Windows.h>
#include "logicServer.h"
#include "strFun.h"

int logicServerMgr::getMyPath (std::unique_ptr<char[]>& myPath)
{
	int nRet = 0;
	wchar_t szPath[MAX_PATH];
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	if (GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath)) == 0)
	{
		return 1;
	}
	int len = (int)(wcslen (szPath));
	for (int i = len - 1; i > 0; i--) {
		if (szPath[i] == L'\\' || szPath[i] == L'/') {
			szPath[i + 1] = 0;
			break;
		}
	}
	myW2U8 (szPath, myPath);
	return nRet;
}
