#include "impMainLoop.h"
#include "tSingleton.h"
#include "impLoop.h"
#include "mainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mLog.h"
#include "mArgMgr.h"

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

int impLoop::onWriteOncePack(packetHead* pPack) // 无用
{
	int nRet = procPacketFunRetType_doNotDel;
	auto pN = P2NHead (pPack);
	iPackSave* pIPackSave = getIPackSave ();
	auto pF = pIPackSave->netTokenPackFind (pN->dwToKen);
	
	if (!pF) {
		nRet = procPacketFunRetType_del;
	}
	mTrace (__FUNCTION__<<" msgId = "<<pN->uwMsgID<<" token = "<<pN->dwToKen<<" pack = "
			<<pPack<<" nRet = "<<nRet<<" map.size = "<<0<<" server handle = "<<id());
	return nRet;
}

impLoop::impLoop()//:m_MsgMap(8)
{
	m_funOnFrame = nullptr;
	m_pArg = nullptr;
	m_packSave = nullptr;
	m_packSave = nullptr;
	m_id = c_emptyModelId;
	m_serverNode.listenerNum = 0;
	m_serverNode.connectorNum = 0;
	m_frameNum = 0;
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
		// auto& rMap = tokenMsgS ();
		// rMap.erase (pN->dwToKen);
		auto pIPackSave = getIPackSave ();
		pIPackSave->netTokenPackErase (pN->dwToKen);
	}
}

int impLoop::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto& rPho = tSingleton<PhyInfo>::single();
	auto& rCS = rPho.getPhyCallback();
	auto allocFun = rCS.fnAllocPack;
	auto freeFun = rCS.fnFreePack;
	auto sendFun = rCS.fnSendPackToLoop;
	auto pN = P2NHead(pPack);
	auto getCurServerHandleFun = rCS.fnGetCurServerHandle;
	auto cHandle = getCurServerHandleFun ();
	auto myHandle = id ();
	myAssert (cHandle == myHandle);
	myAssert (cHandle == pN->ubyDesServId);
	do {
		auto pF = findMsg(pN->uwMsgID);
		if(!pF) {
			mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			break;
		}

		auto& rMsgInfoMgr = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		bool bIsRet = rMsgInfoMgr.isRetMsg (pN->uwMsgID);
		procPacketArg argP;
		argP.handle = id ();
		auto bInOncePro = packInOnceProc(pPack);
		if (bInOncePro) {
			if (bIsRet) {
				nRet = pF(pPack->pAsk, pPack, &argP);
			} else {
				packetHead* pRet = nullptr;
				pF(pPack, pRet, &argP);
				if (pRet) {
					pRet->pAsk = pPack;
					auto pRN = P2NHead (pRet);
					pRN->ubySrcServId = pN->ubyDesServId;
					pRN->ubyDesServId = pN->ubySrcServId;
					pRN->dwToKen = pN->dwToKen;
					nRet = procPacketFunRetType_doNotDel;
					sendFun (pRet);
				}
			}
		} else {
			if (bIsRet) {
				auto& rMgr = tSingleton<loopMgr>::single ();
				auto pS = rMgr.getLoop(pN->ubyDesServId);
				myAssert (pS);
				auto pIPackSave = pS->getIPackSave ();
				auto pAskPack = pIPackSave->netTokenPackFind (pN->dwToKen);
				
				if (!pAskPack) {
					mWarn ("send packet can not find by token: "<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
							<<" length = "<<pN->udwLength);
					break;
				}
				myAssert (pAskPack);
				pPack->pAsk = pAskPack;
				nRet = pF(pAskPack, pPack, &argP);
				pIPackSave->netTokenPackErase (pN->dwToKen);
			} else {
				packetHead* pRet = nullptr;
				nRet = pF(pPack, pRet, &argP);
				if (pRet) {
					auto pRN = P2NHead (pRet);
					pRN->ubySrcServId = pN->ubyDesServId;
					pRN->ubyDesServId = pN->ubySrcServId;
					pRN->dwToKen = pN->dwToKen;
					sendFun (pRet);
				}
			}
		}
	} while (0);
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
static bool sShowFps (void* pUserData)
{
	auto pServer =	(impLoop*)(*(void**)(pUserData));
	pServer->showFps ();
	return true;
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
	auto& rArgS = tSingleton<mArgMgr>::single ();
	auto packSaveTag = rArgS.savePackTag ();
	if (0 == packSaveTag) {
		m_pImpPackSave_map = std::make_unique <impPackSave_map> ();
		m_packSave = m_pImpPackSave_map.get();
	}

	if (pNode->fpsSetp) {
		auto& rTimer = getTimerMgr();
		auto pSer = this;
		rTimer.addComTimer (pNode->fpsSetp, sShowFps, &pSer, sizeof(pSer));
	}
	return 0;
}


void   impLoop:: showFps ()
{
    do {
		auto& fps = fpsC ();
		auto dFps = fps.update (m_frameNum);
		mInfo(" FPS : "<<dFps);
    } while (0);
}

fpsCount&  impLoop:: fpsC ()
{
    return m_fpsC;
}

ServerIDType impLoop::id()
{
	return m_id;
}

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
	if (m_funOnFrame) {
		nRet = m_funOnFrame(m_pArg);
	}
	m_frameNum++;
	return nRet;
}

bool impLoop::regMsg(uword uwMsgId, procRpcPacketFunType pFun)
{
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
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

iPackSave*     impLoop:: getIPackSave ()
{
	return m_packSave;
}

