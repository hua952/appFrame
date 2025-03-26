#include <chrono>
#include "deListMsgQue.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "myAssert.h"
#include "tSingleton.h"
#include "workServerMgr.h"

deListMsgQue::deListMsgQue()
{
	m_Head.pNext = &m_Head;
	m_Head.pPer = &m_Head;
}

 deListMsgQue::~deListMsgQue()
{
	packetHead* d = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		auto pH = &m_Head;
		auto p = pH->pNext;
		while (p != pH)
		{
			d = p;
			p = p->pNext;
			freePack (d);
		}
	}
}

bool deListMsgQue::pushPack (packetHead* pack)
{
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		auto pH = &m_Head;
		pH->pPer->pNext = pack;
		pack->pPer = pH->pPer;
		pack->pNext = pH;
		pH->pPer = pack;
		m_cv.notify_one();
	}
	return true;
}

void deListMsgQue::getMsgS (packetHead* pH, udword milliSec)
{
	pH->pNext = pH;
	pH->pPer = pH;
	// auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(milliSec);
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		if (m_cv.wait_for(lock, std::chrono::microseconds(milliSec), [this] { return m_Head.pNext != &m_Head;})) {
			auto pMyH = &m_Head;
			auto pEnd = pMyH->pPer;
			auto pBegin = pMyH->pNext;
			pH->pNext =  pBegin;
			pBegin->pPer = pH;
			pH->pPer = pEnd;
			pEnd->pNext = pH;
			pMyH->pNext = pMyH;
			pMyH->pPer = pMyH;
		} 
	}
}
