#ifndef _logicWorker_h__
#define _logicWorker_h__
#include <memory>
#include <unordered_map>
#include <map>
#include "packet.h"
#include "msg.h"

int  createUuid (char* outPut);
packetHead* nClonePack(netPacketHead* pN);
class logicWorker
{
public:
	using tokenMapType = std::unordered_map<NetTokenType, packetHead*>;

	struct channelKey 
	{
		uqword  first;
		uqword  second;
	};
	class cmpChannelKey
	{
	public:
		bool operator ()(const channelKey& k1,const channelKey& k2)const;
	};

	using createChannelMap = std::map<channelKey, uword, cmpChannelKey>;
    logicWorker ();
    ~logicWorker ();

	int    sendMsg (CMsgBase& rMsg);
	int    sendMsg (CMsgBase& rMsg, loopHandleType appGroupId, loopHandleType threadGroupId);

	int  sendPacket (packetHead* pPack);
	virtual int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId);
	// int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId, SessionIDType sessionId);
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
	virtual int onCreateChannelRet(const channelKey& rKey, udword result) = 0;

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
	int  createChannel (const channelKey& rKey);
	int  createChannel (const channelKey& rKey, bool sayToMe);
	int  deleteChannel (const channelKey& rKey);
	int  subscribeChannel (const channelKey& rKey);
	int  sayToChannel (const channelKey& rKey, packetHead* pack);
	int  leaveChannel (const channelKey& rKey);
    void ntfOtherLocalServerExit(bool incMe = false);
	void*  userData ();
	void  setUserData (void* v);
	createChannelMap&  getCreateChannelMap (); 
	static void stringToChannel (const char* szCh, channelKey& rCh);
protected:
	int  sendToAllGate (packetHead* pack);
private:
	void*  m_userData;
	tokenMapType   m_tokenMap;
	createChannelMap  m_createChannelMap;
	bool  m_willExit{false};
	ubyte  m_serverGroup {0};
	ubyte  m_serverId;
};
#endif
