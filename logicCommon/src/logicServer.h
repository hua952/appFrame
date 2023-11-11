#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
#include <set>
#include <map>
#include "ISession.h"


class CMsgBase;
class logicServer
{
public:
	using exitHandleSet = std::set<serverIdType>;
	using channelKey =  keyValue <uqword, uqword>;
	class cmpChannelKey
	{
	public:
		bool operator ()(const channelKey& k1,const channelKey& k2)const;
	};
	using channelValue = std::set<uqword>;
	using channelMap = std::map<channelKey, channelValue, cmpChannelKey>;
	using channelSendValue = std::set<uqword>;
	using channelSendMap = std::map<udword, channelSendValue>;
	logicServer ();
    serverNode&   serverInfo ();
	const char*   serverName ();
	serverIdType  serverId ();
	channelMap&  channelS ();
	void          setServerName (const char* v);
	virtual int onFrameFun ();
	virtual int onServerInitGen(ForLogicFun* pForLogic);

	virtual void onLoopBegin();
	virtual void onLoopEnd();
	int         sendMsg (CMsgBase& rMsg);
	int         sendToServer (CMsgBase& rMsg, loopHandleType handle);
	int         sendToAllGateServer (CMsgBase& rMsg);
	int         sendPack (packetHead* pack);
	int         sendPackToServer (packetHead* pack, loopHandleType handle);
	int         sendPackToSomeServer(packetHead* pack, serverIdType* pSerS, udword serverNum);
	int         sendMsgToSomeServer(CMsgBase& rMsg, serverIdType* pSerS, udword serverNum);
	int         sendPackToSomeLocalServer(packetHead* pack, serverIdType* pSerS, udword serverNum);
	int         sendMsgToSomeLocalServer(CMsgBase& rMsg, serverIdType* pSerS, udword serverNum);
	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength);
	void addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength);
	channelSendMap&  channelSendS ();

	virtual void logicOnAcceptSession(SessionIDType sessionId, uqword userData);
	virtual void logicOnConnect(SessionIDType sessionId, uqword userData);
	static void sLogicOnAcceptSession(serverIdType	fId, SessionIDType sessionId, uqword userData);
	static void sLogicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	exitHandleSet&  exitHandleS ();
	bool  willExit ();
	void  setWillExit (bool v);
	int   createChannel (channelKey& rCh, serverIdType srcSer, SessionIDType seId);
private:
	bool  m_willExit;
	exitHandleSet  m_exitHandleS;
	channelSendMap  m_channelSendS;
	channelMap  m_channelS;
	serverNode   m_serverInfo;
	std::unique_ptr <char[]>  m_serverName;
};
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	virtual dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
	int   procArgS (int nArgC, char** argS);
	ubyte  serverNum ();
	void   setServerNum (ubyte ubyNum);
	logicServer*  findServer(serverIdType	serverId);
	logicServer**   serverS ();

	ForLogicFun&     forLogicFunSt ();
protected:
	ForLogicFun*     m_pForLogicFun;
	ubyte  m_serverNum;
	std::unique_ptr <logicServer* []>  m_serverS;
};
#endif
