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
#include "mLog.h"
#include "impLoop.h"

int  InitMidFrame(int nArgC, const char* argS[], PhyCallback* pCallbackS)
{
	tSingleton<PhyInfo>::createSingleton();
	tSingleton<loopMgr>::createSingleton();
	auto& rMgr = tSingleton<PhyInfo>::single();
	auto nRet = rMgr.init(nArgC, argS, *pCallbackS);
	return nRet;
}

int getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	mInfo (" At then begin of getAllLoopAndStart");
	auto& rMgr = tSingleton<loopMgr>::single();
	return rMgr.getAllLoopAndStart(pBuff, nBuffNum);
}

PhyInfo::PhyInfo()
{
}

int PhyInfo::createServer (const char* szName, loopHandleType serId, serverNode* pNode, frameFunType funFrame, void* arg)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pRet = rMgr.createServer(szName, serId, pNode, funFrame, arg);
	return pRet ;
}

int PhyInfo::regMsg(loopHandleType handle, uword uwMsgId, procPacketFunType pFun)
{
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
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
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
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

static int sAddComTimer(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	int nL = pThis;
	//mTrace ("at then begin of sAddComTimer pThis = "<<nL);
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop(pThis);
	//mTrace ("at then begin of sAddComTimer pTH = "<<pTH);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}


int PhyInfo::init(int nArgC, const char* argS[], PhyCallback& info)
{
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	forLogic.fnCreateLoop = PhyInfo::createServer;
	forLogic.fnAllocPack = info.fnAllocPack;
	forLogic.fnFreePack = info.fnFreePack;
	forLogic.fnRegMsg = regMsg;
	forLogic.fnSendPackToLoop = info.fnSendPackToLoop;
	forLogic.fnLogMsg = info.fnLogMsg;
	forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
	//forLogic.fnRemoveMsg = removeMsg;
	auto nRet = procArgS(nArgC, argS);
	do
	{
		if(0 != nRet)
		{
			break;
		}
		mInfo ("before load moduleS moduleNum = "<<m_ModuleNum);
		auto& rModuleS = m_ModuleS;
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			//mTrace(" Before load modle "<<i);
			rM.load(&forLogic );
			//mTrace(" After load modle "<<i);
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
			if(0 == strcmp(buff[0], "addLogic")) {
				//auto nRR = strR(buff[1], '+', buff, c_BuffNum);
				auto bI = moduleS.insert(buff[1]);
				myAssert (bI.second);
			} else if(0 == strcmp(buff[0], "gropId")) {
				int nId = atoi (buff[1]);
				tSingleton<loopMgr>::single().setGropId (nId);
			} else if(0 == strcmp(buff[0], "procId")) {
				int nId = atoi (buff[1]);
				tSingleton<loopMgr>::single().setProcId(nId);
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


CModule* PhyInfo::getModuleS (int& ModuleNum)
{
	ModuleNum = m_ModuleNum;
	return m_ModuleS.get();
}

PhyCallback& PhyInfo::getPhyCallback()
{
	return   m_callbackS;
}

ForLogicFun&  PhyInfo::getForLogicFun()
{
	return m_forLogic;
}

loopMgr::loopMgr():m_CurLoopNum(0), m_procId(0), m_gropId(0)
{
	 //m_loopS = std::make_unique<impLoop[]> (c_MaxLoopNum);
}

loopMgr::~loopMgr()
{
}

int loopMgr::createServerS()
{
	int nRet = 0;
	/*
	auto& rPhyInfo = tSingleton<PhyInfo>::single();
	int ModuleNum = 0;
	CModuleS* pMs =  getModuleS (ModuleNum);
	for (
	*/
	return nRet;
}
/*
static inline loopHandleType toHandle(loopHandleType g, loopHandleType p, loopHandleType l)
{
	loopHandleType nRet = g;
	nRet &= GroupMark;
	nRet <<= ProcNumBitLen;
	p &= ProcMark;
	nRet |= p;
	nRet <<= LoopNumBitLen;
	l &= LoopMark;
	nRet |= l;
	return nRet;
}
*/
int loopMgr::createServer(const char* szName, loopHandleType serId,  serverNode* pNode, frameFunType funFrame, void* arg)
{
	loopHandleType pRet = c_emptyLoopHandle;

	/*
	auto uwId = m_CurLoopNum;
	myAssert (uwId < c_MaxLoopNum);
	if (uwId >= c_MaxLoopNum)
	{
		return pRet;
	}
	myAssert (m_loopS[uwId] == nullptr);
	if (m_loopS[uwId] != nullptr)
	{
		return pRet;
	}
	m_loopS[uwId] = std::make_unique<impLoop>;
	auto& p = m_loopS[uwId];
	auto gId = gropId();
	auto pId = procId();
	pRet = toHandle(pId, uwId);
	*/
	loopHandleType pid = 0;
	loopHandleType sid = 0;
	fromHandle (serId, pid, sid);
	auto& p = m_loopS[sid];
	myAssert (!p);
	p = std::make_unique<impLoop> ();
	p->init(szName, serId, pNode, funFrame, arg);
	m_CurLoopNum++;
	mInfo ("createServer szName = "<<szName);
	pRet = serId;
	return pRet;
}

loopHandleType	loopMgr::procId()
{
	return m_procId;
}

loopHandleType	loopMgr::gropId()
{
	return m_gropId;
}

void		loopMgr::	setProcId(loopHandleType proc)
{
	m_procId = proc;
}

void			loopMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

int		loopMgr::init(loopHandleType	procId, loopHandleType	gropId)
{
	m_procId = procId;
	m_gropId = gropId;
	return 0;
}

impLoop*  loopMgr::getLoop(loopHandleType id)
{
	loopHandleType pid = 0;
	loopHandleType lid = 0;
	fromHandle (id, pid, lid);
	//mTrace ("getLoop m_CurLoopNum = "<<m_CurLoopNum);
	//return lid < m_CurLoopNum ? &m_loopS[id] : nullptr;
	return m_loopS[lid].get();
}

int loopMgr::getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	int i = 0;
	auto pid = procId ();
	for (; i < m_CurLoopNum && i < nBuffNum; i++)
	{
		auto &p = m_loopS[i];
		auto &node = pBuff[i];
		auto pNode = p->getServerNode ();
		if (pNode) {
			node = *pNode;
		} else {
			node.listenerNum = 0;
			node.connectorNum = 0;
		}
		auto sid = p->id ();
		node.handle = sid;//toHandle (pid, id);
	}

	/*
	auto& rMap =  getTempLoopNameMap();
	auto nS = rMap.size();
	for (auto it = rMap.begin(); rMap.end() != it; ++it)
	{
		auto v = it->second;
		myAssert (v < myAssert);
		if (v >= nS)
		{
			return 1;
		}
	}
	*/
	std::cout<<"loopMgr::getAllLoopAndStart i = "<<i<<" m_CurLoopNum = "<<m_CurLoopNum<<std::endl;
	return i;
}
/*
loopMgr::tempLoopMap&  loopMgr::getTempLoopNameMap()
{
	return m_tempLoopMap;
}
*/
