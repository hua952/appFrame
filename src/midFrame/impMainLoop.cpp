#include <vector>
#include <string>
#include <sstream>
#include "impMainLoop.h"
#include "tSingleton.h"
#include "strFun.h"
#include "CModule.h"
#include "impLoop.h"

int onInit(int nArgC, char* argS[], PhyCallback* pCallbackS)
{
	tSingleton<PhyInfo>::createSingleton();
	auto& rMgr = tSingleton<PhyInfo>::single();
	auto nRet = rMgr.init(nArgC, argS, *pCallbackS);
	return nRet;
}

int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum)
{
	return 0;
}

PhyInfo::PhyInfo()
{
}

loopHandleType PhyInfo::createLoop(char* szName, frameFunType funFrame, void* arg)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pRet = rMgr.createLoop(szName, funFrame, arg);
	return pRet ;
}

int PhyInfo::init(int nArgC, char* argS[], PhyCallback& info)
{
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	forLogic.fnCreateLoop = PhyInfo::createLoop;
	forLogic.fnAllocPack = info.fnAllocPack;
	forLogic.fnFreePack = info.fnFreePack;
	auto nRet = procArgS(nArgC, argS);
	do
	{
		if(0 != nRet)
		{
			break;
		}
		auto& rModuleS = m_ModuleS;
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			rM.load();
		}
	}while(0);
	return nRet;
}

int PhyInfo::procArgS(int nArgC, char* argS[])
{
	using tempModuleInfo = std::vector<std::string>;
	tempModuleInfo moduleS;
	for(int i = 1; i < nArgC; i++)
	{
		auto pC = argS[i];
		auto nL = strlen(pC);
		std::unique_ptr<char[]>	 name = std::make_unique<char[]>(nL + 1);
		strcpy(name.get(), pC);
		const int c_BuffNum = 3;
		char* buff[c_BuffNum];
		auto nR = strR(name.get(), '=', buff, c_BuffNum);
		if(2 == nR)
		{
			if(0 == strcmp(buff[0], "addLogic"))
			{
				moduleS.push_back(buff[1]);
			}
		}
	}
	auto nS = moduleS.size();
	m_ModuleNum = nS;
	m_ModuleS = std::make_unique<CModule[]>(nS);
	for(auto i = 0; i < nS; i++)
	{
		auto& rM = m_ModuleS[i];
		rM.init(moduleS[i].c_str());
	}
	return 0;
}


PhyCallback& PhyInfo::getPhyCallback()
{
	return   m_callbackS;
}

ForLogicFun&  PhyInfo::getForLogicFun()
{
	return m_forLogic;
}

loopHandleType loopMgr::createLoop(char* szName, frameFunType funFrame, void* arg)
{
	auto& rMap =  getTempLoopMap();
	std::string strKey = szName;
	auto it = rMap.find(strKey);
	myAssert(rMap.end() == it);
	loopHandleType pRet = NULL;
	if(rMap.end() == it)
	{
		auto p = new impLoop;
		rMap[strKey] = p;
		pRet = p;
	}
	return pRet;
}

loopMgr::tempLoopMap&  loopMgr::getTempLoopMap()
{
	return m_tempLoopMap;
}