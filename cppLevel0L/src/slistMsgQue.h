#ifndef _slistMsgQue_h_
#define  _slistMsgQue_h_
#include <mutex>
#include "packet.h"
struct slistPackNode
{
	slistPackNode* pNext;
	packetHead* pPack;
};
class slistMsgQue
{
public:
	slistMsgQue();
	~slistMsgQue();
	bool pushPack (packetHead* pack);
	slistPackNode* getMsgS ();

	static slistPackNode* allocNode();
	static void			freeNode(slistPackNode* p);
private:
	slistPackNode*	m_Head;
	std::mutex m_mtx;
};
#endif
