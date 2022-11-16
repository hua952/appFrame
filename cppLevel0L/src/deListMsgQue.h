#ifndef _deListMsgQue_h_
#define _deListMsgQue_h_
#include <mutex>
#include "packet.h"

class deListMsgQue
{
public:
	deListMsgQue();
	~deListMsgQue();
	bool pushPack (packetHead* pack);
	void getMsgS (packetHead* pH);

private:
	packetHead	m_Head;
	std::mutex m_mtx;
};
#endif
