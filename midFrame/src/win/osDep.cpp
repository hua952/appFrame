#include <windows.h>
#include <memory>
#include "strFun.h"

int getMidDllPath (std::unique_ptr<char[]>& pathBuf)
{
	const auto c_pathMax = 512;
	auto tempBuf = std::make_unique<char[]>(c_pathMax + 1);
	int nRet = 0;
	auto pBuff = tempBuf.get();
	MEMORY_BASIC_INFORMATION mbi;
    HMODULE dllHandle = ((::VirtualQuery(getMidDllPath, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
	if (GetModuleFileNameA(dllHandle, pBuff, c_pathMax) == 0) {
		nRet = 1;
	} else {
		int len = (int)(strlen(pBuff));
		auto resLen = len;
		for (int i = len - 1; i > 0; i--) {
			if (pBuff[i] == '\\' || pBuff[i] == '/') {
				pBuff[i + 1] = 0;
				resLen = i + 1;
				break;
			}
		}
		pathBuf = std::make_unique<char[]>(resLen + 1);
		strNCpy (pathBuf.get(), resLen + 1, pBuff);
	}
	return nRet;
}

