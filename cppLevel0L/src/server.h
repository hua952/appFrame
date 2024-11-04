#ifndef _server_h__
#define _server_h__
/*
#include <thread>
#include <memory>
#include "deListMsgQue.h"
#include "mainLoop.h"
#include "ITcpServer.h"
#include <set>
#include <map>
#include <unordered_map>
#include "typeDef.h"
#include "cTimer.h"
#include "comTcpNet.h"
#include "fpsCount.h"
#include "impPackSave_map.h"

struct iPackSave;
class server:public comTcpNet
{
public:
	server();
	~server();
	int init(serverNode* pMyNode);
	bool start();
	void run();
	void join();
	void detach ();
	bool pushPack (packetHead* pack); // Thread safety
	virtual bool onFrame();
    typedef std::map<loopHandleType, ISession*> serverSessionMapT;
	typedef int (*onProcNetPackT)(server* pServer, ISession* session, packetHead* packet);
	typedef std::map<int, onProcNetPackT>  netMsgMap;
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
	bool  autoRun ();
	void  setAutoRun (bool v);
private:
	udword  m_sleepSetp;
	bool  m_autoRun;
	std::unique_ptr<char[]>   m_callStack[c_MaxStackSize];	
	int       m_curCallNum;
	cTimerMgr          m_timerMgr;
    NetTokenType	m_nextToken;
    serverSessionMapT m_serverSessionS;
	static void ThreadFun(server* pS);
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;
	netMsgMap       m_netMsgMap;

public:
	int initMid(const char* szName, ServerIDType id, serverNode* pNode, frameFunType funOnFrame = nullptr, void* argS = nullptr);
	void clean();

	bool regMsg(uword uwMsgId, procRpcPacketFunType pFun);
	bool	removeMsg(uword uwMsgId);
	procRpcPacketFunType findMsg(uword uwMsgId);
	const char* name();

	int onLoopBegin();
	int onLoopEnd();
	int onLoopFrame();
	int processOncePack(packetHead* pPack);

	int processLocalServerPack(packetHead* pPack);
	int processOtherAppToMePack(ISession* session, packetHead* pPack);
	int processOtherAppPack(packetHead* pPack);
	int processAllGatePack(ISession* session, packetHead* pPack);
	int procProx(packetHead* pPack);
	int forward(packetHead* pPack);
	// int forwardForOtherServer(packetHead* pPack);
	int sendPackToSomeSession(netPacketHead* pack, uqword* pSessS, udword sessionNum);
	ServerIDType id();
	serverNode*   getServerNode ();
	typedef std::map<uword, procRpcPacketFunType> MsgMap;
	typedef std::map<NetTokenType, packetHead*>  tokenMsgMap;
	iPackSave*    getIPackSave ();
	void  showFps ();
	fpsCount&  fpsC ();
	int clonePackToOtherNetThread (packetHead* pack, bool excMe = true);	
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void onAcceptSession(ISession* session, void* userData)override;
	void onConnect(ISession* session, void* userData)override;
	void onClose(ISession* session)override;
	void onWritePack(ISession* session, packetHead* pack)override;
	// using serverSessionMapMid = std::map<ServerIDType, SessionIDType>;
	using processSessionMap = std::set<uqword>;

	// serverSessionMapMid&  serverSessionS ();
	ISession*  defSession ();
	void  setDefSession (ISession* v);
	ISession*  getSession(SessionIDType sid);
	// ISession*  getServerSession(ServerIDType sid);

	static const auto c_userQuewordNum = 2;
	int regRoute (ServerIDType objServer, SessionIDType sessionId, udword onlyId);
	bool  canUpRoute ();
	void  setCanUpRoute (bool v);
	bool  canDownRoute ();
	void  setCanDownRoute (bool v);
	processSessionMap&  processSessionS ();
	ISession*  getProcessSession(ServerIDType pid);
	ISession*  getProcessSessionByFullServerId(ServerIDType fullId);
	bool  route ();
	void  setRoute (bool v);
private:
	bool  m_route;
	processSessionMap  m_processSessionS;
	bool  m_canDownRoute;
	bool  m_canUpRoute;
	ISession*  m_defSession;
	// serverSessionMapMid  m_serverSessionSMid;
	uqword    m_frameNum;
	fpsCount  m_fpsC;
	std::unique_ptr<impPackSave_map>	 m_pImpPackSave_map;
	iPackSave*							 m_packSave;
    tokenMsgMap  m_tokenMsgS;
	serverNode		   m_serverNode;
	MsgMap	m_MsgMap;
	frameFunType	m_funOnFrame;
	void*			m_pArg;
	ServerIDType m_id;
	std::unique_ptr<char[]>	 m_name;
};
*/
#endif
