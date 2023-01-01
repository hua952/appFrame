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

class server
{
public:
	server();
	~server();
	int init(serverNode* pMyNode);
	bool start();
	void run();
	void join();
	bool pushPack (packetHead* pack);
    int sendBySession(packetHead* pack);
	bool procProx(packetHead* pack);
	virtual bool onFrame();
    typedef std::map<loopHandleType, ISession*> serverSessionMapT;
	typedef int (*onProcNetPackT)(server* pServer, ISession* session, packetHead* packet);
	typedef std::map<int, onProcNetPackT>  netMsgMap;
	//typedef std::map<SessionIDType, ISession*>  allSessionMap;
    serverSessionMapT&    serverSessionMap();
    //allSessionMap&    allSessionS ();
	//typedef std::unordered_map<NetTokenType, NetTokenType>  tokenMap;
    ISession*    defSession ();
    void  setDefSession (ISession* va);
    NetTokenType	nextToken ();
	netMsgMap&    netMsgProcMap();
	loopHandleType  myProId ();
	loopHandleType  myLoopId ();
	loopHandleType  myHandle ();
    //tokenMap&    tokenS ();
	cTimerMgr&   getTimerMgr ();
private:
    //tokenMap  m_tokenS;
	cTimerMgr          m_timerMgr;
    NetTokenType	m_nextToken;
    ISession*  m_defSession;
    //allSessionMap  m_allSessionS;
    serverSessionMapT m_serverSessionS;
	ITcpServer*			m_pTcpServer;
	static void ThreadFun(server* pS);
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;
	loopHandleType  m_loopHandle;
	netMsgMap       m_netMsgMap;
};
#endif
