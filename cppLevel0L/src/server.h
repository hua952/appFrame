#ifndef _server_h__
#define _server_h__
#include <thread>
#include <memory>
#include "deListMsgQue.h"
#include "mainLoop.h"
#include "ITcpServer.h"
#include <map>
#include <unordered_map>
#include "typeDef.h"
#include "cTimer.h"
#include "iTokenSave.h"
#include "tokenSave_map.h"

class server
{
public:
	server();
	~server();
	int init(serverNode* pMyNode);
	bool start();
	void run();
	void join();
	bool pushPack (packetHead* pack); // Thread safety
	virtual bool onFrame();
    typedef std::map<loopHandleType, ISession*> serverSessionMapT;
	typedef int (*onProcNetPackT)(server* pServer, ISession* session, packetHead* packet);
	typedef std::map<int, onProcNetPackT>  netMsgMap;
    serverSessionMapT&    serverSessionMap();
    NetTokenType	nextToken ();
	netMsgMap&    netMsgProcMap();
	loopHandleType  myProId ();
	loopHandleType  myLoopId ();
	loopHandleType  myHandle ();
	cTimerMgr&   getTimerMgr ();
	void         pushToCallStack (const char* szTxt);
	void         popFromCallStack ();
	void         logCallStack (int nL);
	static thread_local  loopHandleType      s_loopHandleLocalTh;
	static const auto c_MaxStackSize = 20;
	udword  sleepSetp ();
	void  setSleepSetp (udword v);
private:
	udword  m_sleepSetp;
	std::unique_ptr<char[]>   m_callStack[c_MaxStackSize];	
	int       m_curCallNum;
	cTimerMgr          m_timerMgr;
    NetTokenType	m_nextToken;
    serverSessionMapT m_serverSessionS;
	static void ThreadFun(server* pS);
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;
	loopHandleType  m_loopHandle;
	netMsgMap       m_netMsgMap;
};
#endif
