#ifndef _gateRouteWorker_h__
#define _gateRouteWorker_h__
#include <memory>
#include "comTcpNet.h"
#include "routeWorker.h"
#include <map>
#include <unordered_map>
#include <set>

class gateRouteWorker: public routeWorker
{
public:
    gateRouteWorker ();
    ~gateRouteWorker ();
	struct tokenSaveInfo
	{
		SessionIDType sessionId;
		NetTokenType  oldToken;
	};
	using sessionChannelKey = std::pair<uqword, channelKey>;
	class cmpSessionChannelKey
	{
	public:
		bool operator ()(const sessionChannelKey& k1,const sessionChannelKey& k2)const;
	};
	using channelValue = std::set<uqword>;
	using channelMap = std::map<channelKey, channelValue, cmpChannelKey>;
	// using channelSet = std::set<channelKey, cmpChannelKey>;

	using appMap = std::map<uqword, ISession*>;
	using tokenMap = std::unordered_map<NetTokenType, tokenSaveInfo>;
	using sessionChannelMap = std::set<sessionChannelKey, cmpSessionChannelKey>;

	int  regAppRoute (ubyte group, SessionIDType sessionId);
	ISession*  getOnceAppSession(ubyte appGroup);

	int onLoopBeginBase() override;
	void onClose(ISession* session) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession) override;
	// int localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc) override;
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void  sendHeartbeat () override;
	int processOncePack(packetHead* pPack)override;

	int processBroadcastPackFun(ISession* session, packetHead* pack);
	int processNtfBroadcastPackFun(packetHead* pack);

	int subscribeChannel (const channelKey& rKey, SessionIDType sessionId, serverIdType	serverId);

	int onCreateChannelAsk(packetHead* pack, pPacketHead* ppRet);
	int onDeleteChannelAsk(packetHead* pack, pPacketHead* ppRet);
	int onSubscribeChannelAsk(packetHead* pack, pPacketHead* ppRet);
	int onSayToChannelAsk(packetHead* pack, pPacketHead* ppRet);
	int onLeaveChannelAsk(packetHead* pack, pPacketHead* ppRet);
	// ISession* onRecHeadIsNeetForward(ISession* session, netPacketHead* pN) override;
	tokenMap&  getTokenMap ();
	// int  sendBroadcastPack (packetHead* pack) override;
	int  addChannel (const channelKey& rCh);
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId) override;
private:
	int leaveChannel (const channelKey& rKey, SessionIDType sessionId, serverIdType	serverId);
	sessionChannelMap  m_sessionChannelMap;
	channelMap  m_channelMap;
	tokenMap   m_tokenMap;
	appMap  m_appMap;
};
#endif
