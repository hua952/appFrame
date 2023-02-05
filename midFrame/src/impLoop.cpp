#include "impMainLoop.h"
#include "tSingleton.h"
#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mLog.h"

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
	/*
	auto pN = P2NHead (pPack);
	mTrace (__FUNCTION__<<" handle = "<<(int)pThis<<"  msgId = "<<pN->uwMsgID
						<<"  ubyDesServId ="<<(int)pN->ubyDesServId<<" pTH = "<<pTH);
	int hTH = pTH->id ();
	mTrace (__FUNCTION__<<" hTH = "<<hTH);
	*/
	auto nRet = pTH->processOncePack(pPack);
	//mTrace (__FUNCTION__<<" after call pTH->processOncePack hTH = "<<hTH);
	return nRet;
}

int onWriteOncePack(loopHandleType pThis, packetHead* pPack)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	return pTH->onWriteOncePack (pPack);
}

int impLoop::onWriteOncePack(packetHead* pPack)
{
	auto pN = P2NHead (pPack);
	mTrace (__FUNCTION__<<" msgId = "<<pN->uwMsgID);

	auto freeFun =  tSingleton<PhyInfo>::single().getForLogicFun().fnFreePack;
	freeFun (pPack);
	return 0;
}

impLoop::impLoop()//:m_MsgMap(8)
{
	m_funOnFrame = nullptr;
	m_pArg = nullptr;
	m_id = c_emptyModelId;
	m_serverNode.listenerNum = 0;
	m_serverNode.connectorNum = 0;
}

impLoop::~impLoop()
{
}

int impLoop::processOncePack(packetHead* pPack)
{
	//mTrace (__FUNCTION__<<" 000");
	int nRet = 0;
	auto pN = P2NHead(pPack);
	auto pF = findMsg(pN->uwMsgID);
	/*
	mTrace (__FUNCTION__<<" 222 msgId = "<<pN->uwMsgID
						<<"  ubyDesServId ="<<(int)pN->ubyDesServId<<" handle = "<<(int)m_id<<" pF = "<<pF);
						*/
	if(pF) {
		procPacketArg argP;
		argP.handle = id ();
		nRet = pF(pPack, &argP);
	} else {
		mWarn (__FUNCTION__<<" not find fun msgId = "<<pN->uwMsgID
						<<"  ubyDesServId ="<<(int)pN->ubyDesServId<<" handle = "<<(int)m_id
						<<" msgNum = "<<m_MsgMap.size ());
	}
	return nRet;
}

serverNode*  impLoop:: getServerNode ()
{
	return    &m_serverNode;
}
cTimerMgr&  impLoop::getTimerMgr()
{
	return m_timerMgr;
}
int impLoop::init(const char* szName, ServerIDType id, serverNode* pNode,
		frameFunType funOnFrame, void* argS)
{
	auto nL = strlen(szName);
	auto pN = std::make_unique<char[]>(nL + 1);
	strcpy(pN.get(), szName);
	m_name = std::move(pN);
	m_id = id;
	m_funOnFrame = funOnFrame;
	m_pArg = argS;
	if (pNode) {
		m_serverNode = *pNode;
	}
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
	m_timerMgr.onFrame ();
	if (m_funOnFrame)
	{
		nRet = m_funOnFrame(m_pArg);
	}
	return nRet;
}

bool impLoop::regMsg(uword uwMsgId, procPacketFunType pFun)
{
	//bool bRet = m_MsgMap.insert(uwMsgId, pFun);	
	//myAssert(bRet);
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
	mTrace(__FUNCTION__<<" msgId = "<<uwMsgId<<" pFun = "<<pFun);
	return bRet;
}

bool impLoop::removeMsg(uword uwMsgId)
{
	bool bRet = m_MsgMap.erase(uwMsgId);
	return bRet;
}

procPacketFunType impLoop::findMsg(uword uwMsgId)
{
	procPacketFunType pRet = nullptr;
	auto it = m_MsgMap.find(uwMsgId);
	if (m_MsgMap.end () != it) {
		pRet = it->second;
	}
	return pRet;
}
