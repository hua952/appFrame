#ifndef _server_h__
#define _server_h__
#include <thread>
#include <memory>
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
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;
	loopHandleType  m_loopHandle;
};
typedef server* pserver;

class serverMgr
{
public:
	serverMgr();
	~serverMgr();
	int initFun (int cArg, const char* argS[]);
	serverIdType getServerNum();
	pserver* getServerS();
	PhyCallback&  getPhyCallback();
	loopHandleType	procId();
	loopHandleType	gropId();
	void			setProcId(loopHandleType proc);
	void			setGropId(loopHandleType grop);
private:
	int procArgS(int nArgC, const char* argS[]);
	loopHandleType	m_procId;
	loopHandleType	m_gropId;
	std::unique_ptr<pserver[]>	 g_serverS;
	uword			g_ServerNum;
	PhyCallback m_PhyCallback;
};
#endif
