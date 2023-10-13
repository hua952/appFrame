#include "configMgr.h"
#include "strFun.h"

configMgr:: configMgr ()
{
	m_reProc = false;
}

configMgr:: ~configMgr ()
{
}

const char*  configMgr:: defFile ()
{
    return m_defFile.get ();
}

void  configMgr:: setDefFile (const char* v)
{
    strCpy (v, m_defFile);
}

int  configMgr:: procArgS(int cArg, char** argS)
{
    int  nRet = 0;
    do {
		for (int i = 1; i < cArg; i++) {
			char* tempP[3];
			std::unique_ptr<char[]> tempPtr;
			strCpy (argS[i], tempPtr);
			auto p = tempPtr.get();
			auto nRet = strR(p, '=', tempP, sizeof(tempP)/sizeof(tempP[0]));
			if (2 == nRet) {
				procStrA ("defFile", tempP, m_defFile);
				procBoolA ("reProc", tempP,   m_reProc);
			}
		}
    } while (0);
    return nRet;
}

bool configMgr:: procStrA(const char* cKey, char* argv[], std::unique_ptr<char[]>& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		strCpy (argv[1], var);
		return true;
	}
	return false;
}

void configMgr::procBoolA(const char* cKey, char* argv[], bool& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		int nT = 0;
		ss>>nT;
		var = 0 != nT;
	}
}

void configMgr::  procUdwordA(const char* cKey, char* argv[], udword& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		ss>>var;
	}
}

void   configMgr::procUwordA(const char* cKey, char* argv[], uword& var)
{
	if (strcmp(cKey, argv[0]) == 0) {
		std::stringstream ss(argv[1]);
		ss>>var;
	}
}

bool  configMgr:: reProc ()
{
    return m_reProc;
}

void  configMgr:: setReProc (bool v)
{
    m_reProc = v;
}


