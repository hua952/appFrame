#include "impMainLoop.h"
#include "tSingleton.h"

int onInit(int nArgC, char* argS[], PhyCallback* pCallbackS)
{
	tSingleton<PhyInfo>::createSingleton();
	auto& rMgr = tSingleton<PhyInfo>::single();
	rMgr.init(nArgC, argS, *pCallbackS);
	return 0;
}

int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum)
{
	return 0;
}

PhyInfo::PhyInfo()
{
}

int PhyInfo::init(int nArgC, char* argS[], PhyCallback& info)
{
	m_callbackS = info;
	return 0;
}
