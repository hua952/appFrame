#include "slistMsgQue.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "server.h"
#include "serverMgr.h"
#include "tSingleton.h"

slistMsgQue::slistMsgQue()
{
	m_Head = allocNode();
	m_Head->pNext = m_Head;
	m_Head->pPack = nullptr;
}

slistMsgQue::~slistMsgQue()
{
	auto p = m_Head->pNext;
	//auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
	while (p != m_Head)
	{
		auto d = p;
		p = p->pNext;
		freePack(d->pPack);
		freeNode(d);
	}
	freeNode(m_Head);
	m_Head = nullptr;
}
slistPackNode* slistMsgQue::allocNode()
{
	return new slistPackNode;
}

void	slistMsgQue::freeNode(slistPackNode* p)
{
	delete p;
}

bool slistMsgQue::pushPack (packetHead* pack)
{
	auto p = allocNode();
	p->pPack = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		m_Head->pPack = pack;
		p->pNext = m_Head->pNext;
		m_Head->pNext = p;
		m_Head = p;
	}
	return true;
}

slistPackNode* slistMsgQue::getMsgS ()
{
	slistPackNode* r = nullptr;
	auto p = allocNode();
	p->pNext = p;
	p->pPack = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		r = m_Head;
		m_Head = p;
	}
	return r;
}
