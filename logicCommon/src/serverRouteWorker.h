#ifndef _serverRouteWorker_h__
#define _serverRouteWorker_h__
#include <memory>
#include <vector>
#include "routeWorker.h"

struct ISession;
class serverRouteWorker: public routeWorker
{
public:
	enum serverRouteWorkerState
	{
		serverRouteWorkerState_unReg,
		serverRouteWorkerState_Reged
	};
    serverRouteWorker ();
    ~serverRouteWorker ();
	int onLoopBeginBase() override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	void  sendHeartbeat () override;
	void  onRecRegAppRet ();
	// int  sendBroadcastPack (packetHead* pack) override;
	int  sendPacket (packetHead* pPack, loopHandleType appGroupId, loopHandleType threadGroupId) override;
protected:
	std::unique_ptr<ISession*[]>	m_connectors;
	serverRouteWorkerState  m_state{serverRouteWorkerState_unReg};
private:
	std::unique_ptr<std::vector<packetHead*>>	m_unRegSendV{std::make_unique<std::vector<packetHead*>>()};
	ubyte     m_regRetNum{0};
};
#endif
