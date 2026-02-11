#include "workServer.h"
#include "strFun.h"
#include "mLog.h"
#include "workServerMgr.h"
#include "tSingleton.h"
/*
static std::ostream& operator << (std::ostream& os, const packetHead& pack)
{
	auto pp = &pack;
	auto& p = *(P2NHead(pp));
    os<<"pAsk = "<<pack.packArg<<" sessionID = " <<pack.sessionID<<" loopId = "<<(int)(pack.loopId)<< " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pack = "<<pp;
    return os;
}

static std::ostream& operator << (std::ostream& os, const netPacketHead& pack)
{
	auto pp = &pack;
	auto& p = pack;
    os << " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pN = "<<pp;
    return os;
}
*/

workServer:: workServer ()
{
	m_sleepSetp = 1;
	// m_id = c_emptyLoopHandle;
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
	auto nRet = onLoopBegin(); // onMidLoopBegin(m_serverId);

    if (procPacketFunRetType_exitNow & nRet || procPacketFunRetType_exitAfterLoop & nRet) [[unlikely]]{
        mInfo ("onMidLoopBegin ret procPacketFunRetType_exitNow");
    } else {
        while(true)
        {
            auto bExit = onFrame();
            if (bExit) [[unlikely]]
            {
                break;
            }
        }
    }
	onLoopEnd ();// onMidLoopEnd(m_serverId);
}

void workServer::ThreadFun(workServer* pS)
{
	pS->run();
}

bool  workServer:: start()
{
	mTrace("startThread handle = "<<(int)m_serverId<<" this = "<<this);
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
	// try {
	m_timerMgr.onFrame ();
	
	auto myHandle = m_serverId; 
	//auto nQuit = ::onLoopFrame(myHandle);
	auto nQuit = onLoopFrame();
	if (procPacketFunRetType_exitNow & nQuit) [[unlikely]]{
		bExit = true;
	} else {
		if (procPacketFunRetType_exitAfterLoop & nQuit) {
			bExit = true;
		}
		packetHead head;
		auto pH = &head;
		m_slistMsgQue.getMsgS(pH, m_sleepSetp);
		auto n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto p = d->pPer;
			int nRet =  procPacketFunRetType_del;
			auto pN = P2NHead (d);
			nRet = processOncePack(d);
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
    /*
	if (m_sleepSetp) {
		std::this_thread::sleep_for(std::chrono::microseconds (m_sleepSetp));
	}
	} catch (const std::exception& e) {
		mError (" catch exception : "<<e.what());
	}
	*/
	return bExit;
}

int workServer::processOncePack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rSerMgr = tSingleton<workServerMgr>::single ();
		nRet = rSerMgr.onRecPacketFun ()(serverId(), pPack);
		if (nRet & procPacketFunRetType_stopBroadcast) {
			break;
		}
		auto pN = P2NHead(pPack);
		bool bIsRet = NIsRet(pN);
		auto pF = findMsg(pN->uwMsgID);
		if(!pF) [[unlikely]]{
			mWarn ("can not find proc function token: "
				<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength);
			break;
		}
		procPacketArg argP;
		argP.handle = serverId();
		argP.broadcast = false;
		if (bIsRet) { // pPack->pAsk put by other server
			auto pAskPack = (pPacketHead)(pPack->packArg);
			nRet = pF(pAskPack, pPack, &argP);
			if (!(nRet & procPacketFunRetType_doNotDel)) {
				freePack (pAskPack);
			}
		} else {
			packetHead* pRet = nullptr;
			nRet = pF(pPack, pRet, &argP);
			if (pRet) {
				pRet->packArg = (uqword)pPack;
				nRet = procPacketFunRetType_doNotDel;
				pRet->loopId = serverId ();
				/*
				if (procPacketFunRetType_exitNow == fRet || procPacketFunRetType_exitAfterLoop == fRet) {
					nRet |= fRet;
				}
				*/
				auto pSendServer = rSerMgr.getServer (pPack->loopId);
				if (pSendServer) {
					pSendServer->pushPack (pRet);
				} else {
					mWarn("can not find SendServer id : "<<(int)(pPack->loopId));
				}
			}
		}
	} while (0);
	return nRet;
}

bool workServer::pushPack (packetHead* pack)
{
	/*
	auto pN = P2NHead(pack);
	pN->uwMsgID;
	if (56797 == pN->uwMsgID) {
		mWarn(" find err pack 56797 pack is : ");
	}
	*/
	return m_slistMsgQue.pushPack (pack);
}

static bool logFpsFun (void* pData)
{
	auto pT = (workServer**)(pData);
	(*pT)->logFps();
	return true;
}

int workServer:: onLoopBegin()
{
    int  nRet = 0;
    do {
		void* pT = this;
		m_timerMgr.addComTimer(showFpsSetp (), logFpsFun, &pT, sizeof(pT));
		auto& rMgr = tSingleton<workServerMgr>::single();
		rMgr.incRunThNum (serverId());
		nRet = rMgr.onLoopBegin ()(m_serverId);
    } while (0);
    return nRet;
}

int workServer:: onLoopEnd()
{
    int  nRet = 0;
    do {
		auto& rMgr = tSingleton<workServerMgr>::single();
		rMgr.subRunThNum (serverId());
		rMgr.onLoopEnd()(m_serverId);
    } while (0);
    return nRet;
}

int workServer::onLoopFrame()
{
	int nRet = procPacketFunRetType_del;
	do {
		auto& rMgr = tSingleton<workServerMgr>::single();
		nRet = rMgr.onFrameLogicFun ()(m_serverId);
	}while(0);
	m_frameNum++;
	return nRet;
}

bool  workServer:: showFps ()
{
    return m_showFps;
}

void  workServer:: setShowFps (bool v)
{
    m_showFps = v;
}

udword  workServer:: showFpsSetp ()
{
    return m_showFpsSetp;
}

void  workServer:: setShowFpsSetp (udword v)
{
    m_showFpsSetp = v;
}

void    workServer:: setAttr(const char* txt)
{
    do {
		std::string strKey;
		std::string strValue;
		auto twoRet = stringToTwoValue (txt, strKey, strValue);
		if (!twoRet) {
			break;
		}
		stringMatchValue(strKey.c_str(), strValue.c_str(), "showFps", m_showFps);
		stringMatchValue(strKey.c_str(), strValue.c_str(), "showFpsSetp", m_showFpsSetp);
		stringMatchValue(strKey.c_str(), strValue.c_str(), "sleepSetp", m_sleepSetp);
		
    } while (0);
}

void workServer:: logFps ()
{
    do {
		auto show = showFps ();
		if (!show) {
			break;
		}
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

