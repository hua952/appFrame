#include "workServer.h"
#include "strFun.h"
#include "mLog.h"

workServer:: workServer ()
{
	m_sleepSetp = 0;
	m_id = c_emptyLoopHandle;
	m_frameNum = 0;
}

workServer:: ~workServer ()
{
}

int  workServer::initWorkServer (ServerIDType id)
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

void workServer::ThreadFun(server* pS)
{
	pS->run();
}

bool  workServer:: start()
{
	mTrace("startThread handle = "<<m_id<<" this = "<<this);
	m_pTh =std::make_unique<std::thread>(server::ThreadFun, this);
	return true;
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
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead(pPack);
	do {
		myAssert (c_emptyLoopHandle != pN->ubyDesServId);
		auto bInOncePro = packInOnceProc(pPack);
		if (bInOncePro) {
			myAssert (myHandle == pN->ubyDesServId);
			nRet = processLocalServerPack (pPack);    /*  一定是本进程其它线程发给本线程的 */
			break;
		}
		/*  以下处理其它进程到本线程的包    */
		myAssert (myHandle == pN->ubyDesServId);  /* 如果不等在接到网络数据包时就转发了不会到这, 到此是一定要处理了,不会再转(本线程只是普通线程,要转也转不了)  */
		nRet = processOtherAppPack (pPack);  /*   其它进程发给本线程处理的包(通过本进程的其它线程从网络将收再转交给本线程处理)    */
	} while (0);
	return nRet;
}

int  workServer:: processLocalServerPack(packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
	do {
		auto& rSerMgr = tSingleton<serverMgr>::single ();
		auto pN = P2NHead(pPack);
		myAssert(c_emptyLoopHandle != pN->ubyDesServId);
		auto fromId = pN->ubySrcServId;
		auto toId = pN->ubyDesServId;
		bool bIsRet = NIsRet(pN);
		auto pF = findMsg(pN->uwMsgID);
		auto myHandle = id ();
		if(!pF) {
			if (pN->uwMsgID < 60000) {
				mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			}
			break;
		}
		procPacketArg argP;
		argP.handle = id ();
		if (bIsRet) { // pPack->pAsk put by other server
			auto  pAsk = (pPacketHead)(pPack->pAsk);
			pPack->pAsk = (decltype (pPack->pAsk))pAsk;
			nRet = pF((pPacketHead)(pPack->pAsk), pPack, &argP);
		} else {
			packetHead* pRet = nullptr;
			auto fRet = pF(pPack, pRet, &argP);
			if (pRet) {
				pRet->pAsk = (uqword)pPack;
				auto pRN = P2NHead (pRet);
				pRN->ubySrcServId = toId;
				pRN->ubyDesServId = fromId;
				pRN->dwToKen = pN->dwToKen;

				/* 不能删除ask包,因为其他线程处理ret包时还要用   */
				nRet = procPacketFunRetType_doNotDel;

				if (procPacketFunRetType_exitNow == fRet || procPacketFunRetType_exitAfterLoop == fRet) {
					nRet |= fRet;
				}
				rSerMgr.pushPackToLoop (pRN->ubyDesServId, pRet);
			}
		}
	} while (0);
	return nRet;
}

ServerIDType workServer::id()
{
	return m_id;
}

int workServer:: onLoopBegin()
{
    int  nRet = 0;
    do {
    } while (0);
    return nRet;
}

int workServer:: onLoopEnd()
{
    int  nRet = 0;
    do {
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

