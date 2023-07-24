#include "argMgr.h"
#include "strFun.h"

argMgr:: argMgr ()
{
}

argMgr:: ~argMgr ()
{
}
int  argMgr:: procArgS(int nArgC, const char* argS[])
{
    int  nRet = 0;
    do {
		for (int i = 1; i < nArgC; i++) {
			char* tempP[3];
			auto p = argS[i];
			std::unique_ptr<char[]> buf;
			strCpy (p, buf);
			auto pBuf = buf.get();
			auto nRet = strR(pBuf, '=', tempP, sizeof(tempP)/sizeof(tempP[0]));
			if (2 == nRet) {
				onCmpKey (tempP);
			}
		}
    } while (0);
    return nRet;
}

void argMgr::onCmpKey (char* argv[])
{
}

bool argMgr:: procStrA(const char* cKey, char* argv[], std::unique_ptr<char[]>& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		auto nL = strlen (argv[1]);
		var = std::make_unique<char[]> (nL + 1);
		strcpy_s (var.get(), nL + 1, argv[1]);
		return true;
	}
	return false;
}

void argMgr::  procUdwordA(const char* cKey, char* argv[], udword& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		ss>>var;
	}
}

void  argMgr ::procUwordA(const char* cKey, char* argv[], uword& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		ss>>var;
	}
}

void   argMgr:: procBoolA(const char* cKey, char* argv[], bool& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		int nT = 0;
		ss>>nT;
		var = 0 != nT;
	}
}
