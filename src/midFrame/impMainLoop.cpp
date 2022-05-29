#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "impMainLoop.h"
#include "tSingleton.h"
#include "strFun.h"
#include "CModule.h"
#include "impLoop.h"
#include "myAssert.h"

int  InitMidFrame(int nArgC, const char* argS[], PhyCallback* pCallbackS)
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

int PhyInfo::regMsg(loopHandleType handle, uword uwMsgId, procPacketFunType pFun)
{
	int nRet = 0;
	auto pLoop = (impLoop*)(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->regMsg(uwMsgId, pFun);
	} while(0);
	return nRet;
}

int PhyInfo::removeMsg(loopHandleType handle, uword uwMsgId)
{
	int nRet = 0;
	auto pLoop = (impLoop*)(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->removeMsg(uwMsgId);
	} while(0);
	return nRet;
}

int PhyInfo::init(int nArgC, const char* argS[], PhyCallback& info)
{
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	forLogic.fnCreateLoop = PhyInfo::createLoop;
	forLogic.fnAllocPack = info.fnAllocPack;
	forLogic.fnFreePack = info.fnFreePack;
	forLogic.fnRegMsg = regMsg;
	forLogic.fnRemoveMsg = removeMsg;
	auto nRet = procArgS(nArgC, argS);
	do
	{
		if(0 != nRet)
		{
			break;
		}
		std::cout<<"in PhyInfo::init"<<std::endl;
		auto& rModuleS = m_ModuleS;
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			rM.load(&forLogic );
		}
	}while(0);
	return nRet;
}

int PhyInfo::procArgS(int nArgC, const char* argS[])
{
	using tempModuleInfo = std::set<std::string>;
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
				auto bI = moduleS.insert(buff[1]);
				myAssert (bI.second);
			}
		}
	}
	auto nS = moduleS.size();
	m_ModuleNum = nS;
	m_ModuleS = std::make_unique<CModule[]>(nS);
	auto i = 0;
	auto& forLogic = getForLogicFun();
	for(auto it = moduleS.begin(); moduleS.end() != it; ++it)
	{
		auto& rM = m_ModuleS[i++];
		rM.init(it->c_str());
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

loopMgr::~loopMgr()
{
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
