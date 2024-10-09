#ifndef _logicWorker_h__
#define _logicWorker_h__
#include <memory>
#include "packet.h"
#include "msg.h"

class logicWorker
{
public:
    logicWorker ();
    ~logicWorker ();

	int    sendMsg (CMsgBase& rMsg);
	int    sendMsg (CMsgBase& rMsg, loopHandleType appGroupId, loopHandleType threadGroupId);

	int  sendPacket (packetHead* pPack);
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId);
	int  pushPacketToLocalServer(packetHead* pPack, loopHandleType threadGroupId);
	virtual int onWorkerInitGen(ForLogicFun* pForLogic);
	virtual int onWorkerInit(ForLogicFun* pForLogic);
	virtual int processOncePack(packetHead* pPack);
	virtual int onLoopBegin();
	virtual int onLoopEnd();
	virtual int onLoopFrame();
	ubyte  serverId ();
	void  setServerId (ubyte v);
	ubyte  serverGroup ();
	void  setServerGroup (ubyte v);
	bool  willExit ();
	void  setWillExit (bool v);

	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUserData);
	void addTimer(udword udwSetp, ComTimerFun pF, void* pUserData, udword udwLength);
	void addTimer(udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUserData, udword udwLength);
private:
	bool  m_willExit{false};
	ubyte  m_serverGroup {0};
	ubyte  m_serverId;
};
#endif
