#ifndef _serverWorker_h__
#define _serverWorker_h__
#include <memory>
#include "routeWorker.h"

struct ISession;
class serverWorker: public routeWorker
{
public:
	enum serverWorkerState
	{
		serverWorkerState_unReg,
		serverWorkerState_Reged
	};
    serverWorker ();
    ~serverWorker ();
	int onLoopBeginBase() override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	void  sendHeartbeat () override;
	void  onRecRegAppRet ();
	int  sendBroadcastPack (packetHead* pack) override;
protected:
	std::unique_ptr<ISession*[]>	m_connectors;
	serverWorkerState  m_state{serverWorkerState_unReg};
private:
	std::unique_ptr<std::vector<packetHead*>>	m_unRegSendV{std::make_unique<std::vector<packetHead*>>()};
	ubyte     m_regRetNum{0};
};
#endif
