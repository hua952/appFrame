#include "deListMsgQue.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "myAssert.h"

deListMsgQue::deListMsgQue()
{
	m_Head.pNext = &m_Head;
	m_Head.pPer = &m_Head;
}

 deListMsgQue::~deListMsgQue()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	auto pH = &m_Head;
	auto p = pH->pNext;
	auto& rMgr = getPhyCallback();
	while (p != pH)
	{
		auto d = p;
		p = p->pNext;
		rMgr.fnFreePack(d);
	}
}

bool deListMsgQue::pushPack (packetHead* pack)
{
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		auto pH = &m_Head;
		pack->pNext = pH->pNext;
		pH->pNext->pPer = pack;
		pack->pPer = pH;
		pH->pNext = pack;
	}
	return true;
}

void deListMsgQue::getMsgS (packetHead* pH)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	auto pMyH = &m_Head;
	if (pMyH->pNext == pMyH)
	{
		myAssert (pMyH->pPer = pMyH);
		pH->pNext = pH;
		pH->pPer = pH;
		return;
	}
	auto pEnd = pMyH->pPer;
	auto pBegin = pMyH->pNext;
	pH->pNext =  pBegin;
	pBegin->pPer = pH;
	pH->pPer = pEnd;
	pEnd->pNext = pH;
	pMyH->pNext = pMyH;
	pMyH->pPer = pMyH;
}
