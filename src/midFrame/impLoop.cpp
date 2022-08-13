#include "impMainLoop.h"
#include "tSingleton.h"
#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int OnLoopFrame(loopHandleType pThis)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->OnLoopFrame();
	return nRet;
}

int processOncePack(loopHandleType pThis, packetHead* pPack)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	auto nRet = pTH->processOncePack(pPack);
	return nRet;
}

impLoop::impLoop():m_MsgMap(8)
{
	m_funOnFrame = nullptr;
	m_pArg = nullptr;
	m_id = c_emptyModelId;
}

impLoop::~impLoop()
{
}

int impLoop::processOncePack(packetHead* pPack)
{
	int nRet = 0;
	auto pNetPack = P2NHead(pPack);
	auto pF = findMsg(pNetPack->uwMsgID);
	if(pF)
	{
		nRet = pF(pPack);
	}
	return nRet;
}

int impLoop::init(const char* szName, ServerIDType id, frameFunType funOnFrame, void* argS)
{
	auto nL = strlen(szName);
	auto pN = std::make_unique<char[]>(nL + 1);
	strcpy(pN.get(), szName);
	m_name = std::move(pN);
	m_id = id;
	m_funOnFrame = funOnFrame;
	m_pArg = argS;
	return 0;
}


ServerIDType impLoop::id()
{
	return m_id;
}
/*
void	impLoop::setId(ModelIDType id)
{
	m_id = id;
}
*/
const char* impLoop::name()
{
	return m_name.get();
}
	
void impLoop::clean()
{
}

int impLoop::OnLoopFrame()
{
	int nRet = 0;
	if (m_funOnFrame)
	{
		nRet = m_funOnFrame(m_pArg);
	}
	return nRet;
}

bool impLoop::regMsg(uword uwMsgId, procPacketFunType pFun)
{
	bool bRet = m_MsgMap.insert(uwMsgId, pFun);	
	myAssert(bRet);
	return bRet;
}

bool impLoop::removeMsg(uword uwMsgId)
{
	bool bRet = m_MsgMap.erase(uwMsgId);
	return bRet;
}

procPacketFunType impLoop::findMsg(uword uwMsgId)
{
	auto pRet = m_MsgMap.find(uwMsgId);
	return *pRet;
}
