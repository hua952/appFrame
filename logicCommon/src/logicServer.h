#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"


class CMsgBase;
class logicServer
{
public:
    serverNode&   serverInfo ();
	const char*   serverName ();
	serverIdType  serverId ();
	void          setServerName (const char* v);
	virtual int onFrameFun ();
	virtual int onServerInitGen(ForLogicFun* pForLogic);
	int         sendMsg (CMsgBase& rMsg);
	int         sendToServer (CMsgBase& rMsg, loopHandleType handle);
	int         sendPack (packetHead* pack);
	int         sendPackToServer (packetHead* pack, loopHandleType handle);

	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength);
	void addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength);
private:
	serverNode   m_serverInfo;
	std::unique_ptr <char[]>  m_serverName;
};
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	virtual void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	int   procArgS (int nArgC, const char* argS[]);
	ubyte  serverNum ();
	void   setServerNum (ubyte ubyNum);
	logicServer**   serverS ();
protected:
	ubyte  m_serverNum;
	std::unique_ptr <logicServer* []>  m_serverS;
};
#endif
