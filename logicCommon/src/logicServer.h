#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
#include <set>
#include <map>
#include "ISession.h"
#include "staticArrayMap.h"

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
	void          setServerId (serverIdType nId);
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
	int         sendPackToSession (packetHead* pack, SessionIDType sessionId);
	// int         sendPackToOnceOfTmp (packetHead* pack);
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
	// static void sLogicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	exitHandleSet&  exitHandleS ();
	bool  willExit ();
	void  setWillExit (bool v);
	int   createChannel (channelKey& rCh, serverIdType srcSer, SessionIDType seId);
	bool  route ();
	void  setRoute (bool v);
	bool  autoRun ();
	void  setAutoRun (bool v);
private:
	bool  m_autoRun;
	bool  m_route;
	serverIdType  m_serverId;
	bool  m_willExit;
	exitHandleSet  m_exitHandleS;
	channelSendMap  m_channelSendS;
	channelMap  m_channelS;
	serverNode   m_serverInfo;
	std::unique_ptr <char[]>  m_serverName;
};

#endif
