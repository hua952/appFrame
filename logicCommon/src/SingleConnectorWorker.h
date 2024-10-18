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
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet) override;
private:
	ISession*  m_session{nullptr};

};
#endif
