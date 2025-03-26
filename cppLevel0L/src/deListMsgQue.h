#ifndef _deListMsgQue_h_
#define _deListMsgQue_h_
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "packet.h"

class deListMsgQue
{
public:
	deListMsgQue();
	~deListMsgQue();
	bool pushPack (packetHead* pack);
	void getMsgS (packetHead* pH, udword milliSec);

private:
	packetHead	m_Head;
	std::mutex m_mtx;
	std::condition_variable m_cv;
};
#endif
