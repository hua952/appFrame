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


void onFreePack(loopHandleType pThis, packetHead* pPack)
{
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop (pThis);
	pTH->onFreePack(pPack);
}

int impLoop::onWriteOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_doNotDel;
	auto pN = P2NHead (pPack);
	//mTrace (__FUNCTION__<<" msgId = "<<pN->uwMsgID);
	auto& rTS = tokenMsgS ();
	auto it = rTS.find (pN->dwToKen);
	if (rTS.end () == it) {
		//auto freeFun =  tSingleton<PhyInfo>::single().getForLogicFun().fnFreePack;
		//freeFun (pPack);
		nRet = procPacketFunRetType_del;
	}
	return nRet;
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

void impLoop::onFreePack(packetHead* pPack)
{
	auto pN = P2NHead (pPack);
	mTrace ("  msgId = "<<pN->uwMsgID<<" token = "<<pN->dwToKen);
	if (! NIsRet (pN)) {
		mTrace (" erase token = "<<pN->dwToKen<<" handle = "<<id());
		auto& rMap = tokenMsgS ();
		rMap.erase (pN->dwToKen);
	}
}

impLoop::tokenMsgMap&  impLoop:: tokenMsgS ()
{
    return m_tokenMsgS;
}

//static procMsgRetType on_acceptPeaceAsk(packetHead* pAsk, pPacketHead& pRet)

int impLoop::processOncePack(packetHead* pPack)
{
	//mTrace (__FUNCTION__<<" 000");
	int nRet = procPacketFunRetType_del;
	auto& rPho = tSingleton<PhyInfo>::single();
	auto& rCS = rPho.getPhyCallback();
	auto allocFun = rCS.fnAllocPack;
	auto freeFun = rCS.fnFreePack;
	auto sendFun = rCS.fnSendPackToLoop;
	auto pN = P2NHead(pPack);
	auto getCurServerHandleFun = rCS.fnGetCurServerHandle;
	auto cHandle = getCurServerHandleFun ();
	myAssert (cHandle == pN->ubyDesServId);
	do {
		auto pF = findMsg(pN->uwMsgID);
		if(!pF) {
			mWarn ("can not find proc function token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength);
			break;
		}

		auto& rMsgInfoMgr = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		bool bIsRet = rMsgInfoMgr.isRetMsg (pN->uwMsgID);
		procPacketArg argP;
		argP.handle = id ();
		if (bIsRet) {
			auto& rMgr = tSingleton<loopMgr>::single ();
			auto pS = rMgr.getLoop(pN->ubyDesServId);
			myAssert (pS);
			auto& rTM = pS->tokenMsgS ();
			auto it = rTM.find (pN->dwToKen);
			myAssert (rTM.end () != it);
			if (rTM.end () == it) {
				mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
						<<" length = "<<pN->udwLength);
				break;
			}
			auto pAsk = it->second;
			nRet = pF(pAsk, pPack, &argP);
			rTM.erase (it);
			freeFun (pAsk);
		} else {
			auto pF = findMsg(pN->uwMsgID);
			if(pF) {
				packetHead* pRet = nullptr;
				nRet = pF(pPack, pRet, &argP);
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = pN->ubyDesServId;
				pRN->ubyDesServId = pN->ubySrcServId;
				pRN->dwToKen = pN->dwToKen;
				if (procPacketFunRetType_del == nRet) {
					serverIdType	srcP = 0;
					serverIdType	srcS = 0;
					serverIdType	desP = 0;
					serverIdType	desS = 0;
					fromHandle (pN->ubySrcServId, srcP, desS);
					fromHandle (pN->ubyDesServId, desP, desS);
					if (srcP == desP) {
						nRet = procPacketFunRetType_doNotDel;
					}
				}
				sendFun (pRet);
			} else {
				mWarn (__FUNCTION__<<" not find fun msgId = "<<pN->uwMsgID
						<<"  ubyDesServId ="<<(int)pN->ubyDesServId<<" handle = "<<(int)m_id
						<<" msgNum = "<<m_MsgMap.size ());
			}
		}
	} while (0);
	//mTrace (" 0000000 ");
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
	//mTrace ("At the begin");
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
	//mTrace ("At the end");
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

bool impLoop::regMsg(uword uwMsgId, procRpcPacketFunType pFun)
{
	//bool bRet = m_MsgMap.insert(uwMsgId, pFun);	
	//myAssert(bRet);
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
	//mTrace(__FUNCTION__<<" msgId = "<<uwMsgId<<" pFun = "<<pFun);
	return bRet;
}

bool impLoop::removeMsg(uword uwMsgId)
{
	bool bRet = m_MsgMap.erase(uwMsgId);
	return bRet;
}

procRpcPacketFunType impLoop::findMsg(uword uwMsgId)
{
	procRpcPacketFunType pRet = nullptr;
	auto it = m_MsgMap.find(uwMsgId);
	if (m_MsgMap.end () != it) {
		pRet = it->second;
	}
	return pRet;
}
