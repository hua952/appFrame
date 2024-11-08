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

	
	using channelValue = std::set<uqword>;
	using channelMap = std::map<channelKey, channelValue, cmpChannelKey>;

	using appMap = std::map<uword, ISession*>;
	using tokenMap = std::unordered_map<NetTokenType, tokenSaveInfo>;

	int  regAppRoute (ubyte group, SessionIDType sessionId);
	ISession*  getOnceAppSession(ubyte appGroup);

	int onLoopBeginBase() override;
	void onClose(ISession* session) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession) override;
	// int localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc) override;
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void  sendHeartbeat () override;

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
	int  sendBroadcastPack (packetHead* pack) override;
private:
	channelMap  m_channelMap;
	tokenMap   m_tokenMap;
	appMap  m_appMap;
};
#endif
