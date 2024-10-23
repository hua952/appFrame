#ifndef _logicWorker_h__
#define _logicWorker_h__
#include <memory>
#include <unordered_map>
#include "packet.h"
#include "msg.h"

class logicWorker
{
public:
	using tokenMapType = std::unordered_map<NetTokenType, packetHead*>;
	// using netMsgProcMap = std::unordered_map<uword, procRpcPacketFunType>;

	using channelKey =  keyValue <uqword, uqword>;
    logicWorker ();
    ~logicWorker ();

	int    sendMsg (CMsgBase& rMsg);
	int    sendMsg (CMsgBase& rMsg, loopHandleType appGroupId, loopHandleType threadGroupId);

	int  sendPacket (packetHead* pPack);
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId);
	int  pushPacketToLocalServer(packetHead* pPack, loopHandleType threadGroupId);
	virtual int  recPacketProcFun (ForLogicFun* pForLogic); /*   注册一些数据包的处理函数   */
	int  onSendPackToRemoteRet (packetHead* pPack);
	int  onRecRemotePackNtf (packetHead* pPack);
	virtual int onWorkerInitGen(ForLogicFun* pForLogic);
	virtual int onWorkerInit(ForLogicFun* pForLogic);
	virtual int processOncePack(packetHead* pPack);
	virtual int onLoopBegin();
	virtual int onLoopEnd();
	virtual int onLoopFrame();

	virtual int onLoopBeginBase();
	virtual int onLoopEndBase();
	virtual int onLoopFrameBase();
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
	void  delSendPack (NetTokenType  token);
	NetTokenType  newToken();
private:
	tokenMapType   m_tokenMap;
	// netMsgProcMap  m_netMsgProcMap;
	bool  m_willExit{false};
	ubyte  m_serverGroup {0};
	ubyte  m_serverId;
};
#endif
