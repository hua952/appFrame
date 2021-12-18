#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int OnLoopFrame(loopHandleType* pThis)
{
	auto pTH = (impLoop*)pThis;
	auto nRet = pTH->OnLoopFrame();
	return 0;
}

int processOncePack(loopHandleType* pThis, packetHead* pPack)
{
	auto pTH = (impLoop*)pThis;
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
		nRet = pF(pNetPack);
	}
	return nRet;
}

int impLoop::init(const char* szName)
{
	auto nL = strlen(szName);
	auto pN = std::make_unique<char[]>(nL + 1);
	strcpy(pN.get(), szName);
	m_name = std::move(pN);
	return 0;
}


ModelIDType		impLoop::id()
{
	return m_id;
}

void	impLoop::setId(ModelIDType id)
{
	m_id = id;
}

const char* impLoop::name()
{
	return m_name.get();
}
	
void impLoop::clean()
{
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
