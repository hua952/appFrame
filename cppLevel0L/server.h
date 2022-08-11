#ifndef _server_h__
#define _server_h__
#include <thread>
#include "deListMsgQue.h"
#include "mainLoop.h"

class server
{
public:
	server();
	~server();
	int init(loopHandleType  loopHandle);
	bool start();
	void run();
	void join();
	bool pushPack (packetHead* pack);
	virtual bool onFrame();
private:
	static void ThreadFun(server* pS);
	std::thread*	m_pTh;
	deListMsgQue	m_slistMsgQue;
	loopHandleType  m_loopHandle;
};
typedef server* pserver;
#endif
