#ifndef _SingleConnectorWorker_h__
#define _SingleConnectorWorker_h__
#include <memory>
#include "routeWorker.h"

class SingleConnectorWorker: public routeWorker
{
public:
    SingleConnectorWorker ();
    ~SingleConnectorWorker ();

	int onLoopBeginBase() override;

	int getGateServerEndpoint(endPoint& endP);
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession) override;
	void  sendHeartbeat () override;
	
	// int  sendBroadcastPack (packetHead* pack) override;
private:
	ISession*  m_session{nullptr};

};
#endif
