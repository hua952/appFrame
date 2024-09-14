#include "workServer.h"
#include "strFun.h"
#include "mLog.h"
#include "workServerMgr.h"
#include "tSingleton.h"

workServer:: workServer ()
{
	m_sleepSetp = 0;
	m_id = c_emptyLoopHandle;
	m_frameNum = 0;
}

workServer:: ~workServer ()
{
}

int  workServer::initWorkServer ()
{
	int nRet = 0;
	do
	{
	} while (0);
	return  nRet;
}

void workServer::run()
{
	onMidLoopBegin(m_id);
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		
	}
	onMidLoopEnd(m_id);
}

void workServer::ThreadFun(workServer* pS)
{
	pS->run();
}

bool  workServer:: start()
{
	mTrace("startThread handle = "<<m_id<<" this = "<<this);
	m_pTh =std::make_unique<std::thread>(workServer::ThreadFun, this);
	return true;
}

void workServer::detach ()
{
	myAssert (m_pTh);
	m_pTh->detach ();
}

procRpcPacketFunType workServer::findMsg(uword uwMsgId)
{
	procRpcPacketFunType pRet = nullptr;
	auto it = m_MsgMap.find(uwMsgId);
	if (m_MsgMap.end () != it) {
		pRet = it->second;
	}
	return pRet;
}

bool  workServer:: onFrame()
{
	bool bExit = false;
	m_timerMgr.onFrame ();
	
	auto myHandle = m_id; 
	auto nQuit = ::onLoopFrame(myHandle);
	if (procPacketFunRetType_exitNow & nQuit) {
		bExit = true;
	} else {
		if (procPacketFunRetType_exitAfterLoop & nQuit) {
			bExit = true;
		}
		packetHead head;
		auto pH = &head;
		m_slistMsgQue.getMsgS(pH);
		auto n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto p = d->pPer;
			int nRet =  procPacketFunRetType_del;
			auto pN = P2NHead (d);
			nRet = ::processOncePack(m_id, d);
			if (procPacketFunRetType_doNotDel & nRet) {
				p->pNext = n;
				n->pPer = p;
			}
			if (procPacketFunRetType_exitNow & nRet) {
				bExit = true;
				break;
			}
			if (procPacketFunRetType_exitAfterLoop & nRet) {
				bExit = true;
			}
		}
		n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			freePack(d);
		}
	}
	if (m_sleepSetp) {
		std::this_thread::sleep_for(std::chrono::microseconds (m_sleepSetp));
	}
	return bExit;
}

int workServer::processOncePack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rSerMgr = tSingleton<workServerMgr>::single ();
		auto alderHandle = rSerMgr.onRecPacketFun ()(serverId(), pPack);
		if (alderHandle & procPacketFunRetType_alderHandle) {
			nRet = alderHandle & (~procPacketFunRetType_alderHandle);
			break;
		}
		auto pN = P2NHead(pPack);
		bool bIsRet = NIsRet(pN);
		auto pF = findMsg(pN->uwMsgID);
		if(!pF) {
			mWarn ("can not find proc function token: "
				<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength);
			break;
		}
		procPacketArg argP;
		argP.handle = serverId();
		if (bIsRet) { // pPack->pAsk put by other server
			auto  pAsk = (pPacketHead)(pPack->pAsk);
			pPack->pAsk = (decltype (pPack->pAsk))pAsk;
			nRet = pF((pPacketHead)(pPack->pAsk), pPack, &argP);
		} else {
			packetHead* pRet = nullptr;
			auto fRet = pF(pPack, pRet, &argP);
			if (pRet) {
				pRet->pAsk = (uqword)pPack;
				nRet = procPacketFunRetType_doNotDel;
				pRet->loopId = serverId ();
				if (procPacketFunRetType_exitNow == fRet || procPacketFunRetType_exitAfterLoop == fRet) {
					nRet |= fRet;
				}
				auto pSendServer = rSerMgr.getServer (pPack->loopId);
				if (pSendServer) {
					pSendServer->pushPack (pRet);
				} else {
					mWarn("can not find SendServer id : "<<pPack->loopId);
				}
			}
		}
	} while (0);
	return nRet;
}

bool workServer::pushPack (packetHead* pack)
{
	return m_slistMsgQue.pushPack (pack);
}

int workServer:: onLoopBegin()
{
    int  nRet = 0;
    do {
		auto& rMgr = tSingleton<workServerMgr>::single();
		rMgr.incRunThNum (serverId());
    } while (0);
    return nRet;
}

int workServer:: onLoopEnd()
{
    int  nRet = 0;
    do {
		auto& rMgr = tSingleton<workServerMgr>::single();
		rMgr.subRunThNum (serverId());
    } while (0);
    return nRet;
}

int workServer::onLoopFrame()
{
	int nRet = procPacketFunRetType_del;
	do {
		
	}while(0);
	m_frameNum++;
	return nRet;
}

void workServer:: showFps ()
{
    do {
		auto& fps = m_fpsC;
		auto dFps = fps.update (m_frameNum);
		mInfo(" FPS : "<<dFps);
    } while (0);
}

udword  workServer:: sleepSetp ()
{
    return m_sleepSetp;
}

void  workServer:: setSleepSetp (udword v)
{
    m_sleepSetp = v;
}

bool workServer::regMsg(uword uwMsgId, procRpcPacketFunType pFun)
{
	bool bRet = true;
	m_MsgMap [uwMsgId] = pFun;
	return bRet;
}

cTimerMgr&  workServer:: getTimerMgr ()
{
	return m_timerMgr;
}

NetTokenType	workServer::nextToken ()
{
    return m_nextToken++;
}

ubyte  workServer:: serverId ()
{
    return m_serverId;
}

void  workServer:: setServerId (ubyte v)
{
    m_serverId = v;
}

