#include "server.h"
#include "loop.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "myAssert.h"

#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds

server::server()
{
	m_loopHandle = c_emptyLoopHandle;
	m_pTh = nullptr;
}

server::~server()
{
	SD(m_pTh);
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

int server::init(loopHandleType  loopHandle)
{
	m_loopHandle = loopHandle;
	return 0;
}

bool server::start()
{
	m_pTh = new std::thread (server::ThreadFun, this);
	return true;
}

void server::join()
{
	myAssert (m_pTh);
	m_pTh->join();
}

void server::run()
{
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono:: milliseconds(1));
	}
}

bool server::onFrame()
{
	OnLoopFrame(m_loopHandle); // call by level 0
	packetHead head;
	auto pH = &head;
	m_slistMsgQue.getMsgS(pH);
	auto n = pH->pNext;
	bool bExit = false;
	auto& rMgr = getPhyCallback();
	auto fnFreePack = rMgr.fnFreePack;
	while (n != pH)
	{
		auto d = n;
		n = n->pNext;
		auto nRet = processOncePack(m_loopHandle, d);// call by level 0
		if (procPacketFunRetType_doNotDel == nRet)
		{
			auto p = d->pPer;
			p->pNext = n;
			n->pPer = p;
		}
		else
		{
			if (procPacketFunRetType_exitNow)
			{
				bExit = true;
				break;
			}
			if (procPacketFunRetType_exitAfterLoop == nRet)
			{
				bExit = true;
			}
		}
	}

	n = pH->pNext;
	while (n != pH)
	{
		auto d = n;
		n = n->pNext;
		fnFreePack (d);
	}
	return bExit;
}


