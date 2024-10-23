#ifndef _gateRouteWorker_h__
#define _gateRouteWorker_h__
#include <memory>
#include "comTcpNet.h"
#include "routeWorker.h"
#include <map>
#include <unordered_map>

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

	using appMap = std::map<uword, ISession*>;
	using tokenMap = std::unordered_map<NetTokenType, tokenSaveInfo>;

	int  regAppRoute (ubyte group, SessionIDType sessionId);
	ISession*  getOnceAppSession(ubyte appGroup);

	int onLoopBeginBase() override;
	void onClose(ISession* session) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet) override;
	int localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc) override;
	int processNetPackFun(ISession* session, packetHead* pack)override;

	// ISession* onRecHeadIsNeetForward(ISession* session, netPacketHead* pN) override;
	tokenMap&  getTokenMap ();
private:
	tokenMap   m_tokenMap;
	appMap  m_appMap;
};
#endif
