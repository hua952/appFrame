#include <iostream>
#include "server.h"
#include "serverMgr.h"
#include "loop.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "strFun.h"
#include "myAssert.h"
#include "tSingleton.h"
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds
#include "cLog.h"
#include "cppLevel0LCom.h"
#include "framMsgId.h"

server::server()
{
	m_loopHandle = c_emptyLoopHandle;
	m_nextToken  = 0;
	m_curCallNum = 0;
	m_sleepSetp = 10;
}

server::~server()
{
	
}

server::serverSessionMapT&  server::  serverSessionMap()
{
	return m_serverSessionS;
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

loopHandleType  server::myHandle ()
{
	return m_loopHandle;
}

loopHandleType server:: myProId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (m_loopHandle, proId, serId);
	return proId;
}

loopHandleType server:: myLoopId ()
{
	loopHandleType  proId;
	loopHandleType  serId;
	fromHandle (m_loopHandle, proId, serId);
	return serId;
}

server::netMsgMap&  server::netMsgProcMap()
{
	return m_netMsgMap;
}

int sOnRegMyHandleAsk(server* pServer, ISession* session, packetHead* packet)
{
	int nRet = procPacketFunRetType_del;
    auto & rMap = pServer->serverSessionMap ();
	auto pN = P2NHead (packet);
	auto pU = (regMyHandleAskPack*)(N2User (pN));

	rMap [pU->myHandle] = session;
	auto& rServerMgr = tSingleton<serverMgr>::single ();
	auto& rCallback = rServerMgr.getPhyCallback ();
	auto allocFun = rCallback.fnAllocPack;
	auto pSend = allocFun (sizeof (regMyHandleRetPack));
	if (pSend) {
		auto pRN = P2NHead (pSend);
		pRN->uwMsgID = toFramMsgId(enFramMsgId_regMyHandleRet);
		pRN->dwToKen = pN->dwToKen;
		pRN->ubyDesServId = pN->ubySrcServId;
		pRN->ubySrcServId = pN->ubyDesServId;
		auto pRU = (regMyHandleRetPack*)(N2User (pRN));
		pRU->myHandle = pU->myHandle;
		pRU->result = 0;
		auto  fnSendPackToLoop = rCallback.fnSendPackToLoop;
		fnSendPackToLoop (pSend);
	}
	return nRet;
}

int sOnRegMyHandleRet(server* pServer, ISession* session, packetHead* packet)
{
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead (packet);
	auto pU = (regMyHandleRetPack*)(N2User (pN));
	rTrace (__FUNCTION__<<" token= "<<pN->dwToKen<<" result = "<<pU->result<<" myHandle = "<<(int)pU->myHandle);
	return nRet;
}

int server::init(serverNode* pMyNode)
{
	int nRet = 1;
	do {
		if (!pMyNode) {
			nRet = 1;
			break;
		}
		auto handle = pMyNode->handle;
		m_loopHandle = handle;
		setSleepSetp (pMyNode->sleepSetp);
	} while (0);
	return nRet;
}

thread_local  loopHandleType  server::s_loopHandleLocalTh = c_emptyLoopHandle;
bool server::start()
{
	m_pTh =std::make_unique<std::thread>(server::ThreadFun, this);
	return true;
}

void server::join()
{
	myAssert (m_pTh);
	m_pTh->join();
}

void server::run()
{
	s_loopHandleLocalTh = m_loopHandle;
	onMidLoopBegin(m_loopHandle);
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		if (m_sleepSetp) {
			std::this_thread::sleep_for(std::chrono::microseconds (m_sleepSetp));
		}
	}
	onMidLoopEnd(m_loopHandle);
	auto& os = std::cout;
	os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
}

bool server::pushPack (packetHead* pack)
{
	return m_slistMsgQue.pushPack (pack);
}

cTimerMgr&  server:: getTimerMgr ()
{
	return m_timerMgr;
}

bool server::onFrame()
{
	PUSH_FUN_CALL
	bool bExit = false;
	m_timerMgr.onFrame ();
	
	auto myHandle = m_loopHandle;
	auto nQuit = onLoopFrame(myHandle); // call by level 0
	if (procPacketFunRetType_exitNow == nQuit) {
		bExit = true;
	} else {
		if (procPacketFunRetType_exitAfterLoop == nQuit) {
			bExit = true;
		}
		packetHead head;
		auto pH = &head;
		m_slistMsgQue.getMsgS(pH);
		auto n = pH->pNext;
		auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto p = d->pPer;
			int nRet =  procPacketFunRetType_del;
			auto pN = P2NHead (d);
			nRet = processOncePack(m_loopHandle, d);// call by level 0
			if (procPacketFunRetType_doNotDel == nRet) {
				p->pNext = n;
				n->pPer = p;
			} else {
				if (procPacketFunRetType_exitNow == nRet) {
					bExit = true;
					break;
				}
				if (procPacketFunRetType_exitAfterLoop == nRet) {
					bExit = true;
				}
			}
		}
		n = pH->pNext;
		while (n != pH) {
			auto d = n;
			n = n->pNext;
			auto pN = P2NHead (d);
			if (NIsRet (pN)) {  // ask pack will delete by midFrame
				PUSH_FUN_CALL
				rTrace ("Befor free pack = "<<d);
				freePack(d);
				POP_FUN_CALL
			}
		}
	}
	POP_FUN_CALL
	return bExit;
}

NetTokenType	 server::nextToken ()
{
    return m_nextToken++;
}
void   server::      popFromCallStack ()
{
	if (m_curCallNum) {
		m_curCallNum--;
	}
}

void  server:: pushToCallStack (const char* szTxt)
{
	if (m_curCallNum < c_MaxStackSize) {
		strCpy (szTxt, m_callStack[m_curCallNum++]);
	}
}

void   server:: logCallStack (int nL)
{
	for (decltype (m_curCallNum) i = 0; i < m_curCallNum; i++) {
		rTrace (m_callStack[i].get());
	}
}

udword  server:: sleepSetp ()
{
    return m_sleepSetp;
}

void  server:: setSleepSetp (udword v)
{
    m_sleepSetp = v;
}

