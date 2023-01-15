#include <Windows.h>
#include <io.h>
#include<direct.h>
#include <imagehlp.h>
#include <cstring>
#include "comFun.h"
#include "strFun.h"
#include <memory>
#include <string>

bool  isPathExit (const char* szPath)
{
	return _access(szPath, 0) == 0;
}

int strCaseCmp (const char* str1, const char* str2)
{
	return _stricmp(str1, str2);
}

int  myMkdir (const char* szPath)
{
	//return _mkdir(szPath);
	auto nL = strlen (szPath);
	/*
	auto p = szPath;
	if (szPath[nL - 1] != '\\'|| szPath[nL - 1] != '/') {
		auto pB = std::make_unique<char[]> (nL + 2);
		auto pp = pB.get();
		std::strncpy (pp, szPath, nL + 1);
		pp[nL] = '\\';
		pp[nL + 1] = 0;
		p = pp;
	}
	auto bR = MakeSureDirectoryPathExists (p);
	return bR ? 0:-1;
	*/
	
	auto pB = std::make_unique<char[]> (nL + 2);
	auto pp = pB.get();
	for (decltype (nL) i = 0; i < nL; i++) {
		auto c = szPath[i];
		pp[i] = c;
		if ('\\' == c || '/' == c) {
			pp [i + 1] = 0;
			auto bE = isPathExit (pp);
			if (bE) {
				continue;
			}
			auto nR = _mkdir(pp);
			if (0 != nR) {
				return -1;
			}
		}
	}

	if (szPath[nL - 1] != '\\'|| szPath[nL - 1] != '/') {
		pp[nL] = '\\';
		pp[nL + 1] = 0;
		auto nR = _mkdir(pp);
		if (0 != nR) {
			return -1;
		}
	}
	return 0;
}
