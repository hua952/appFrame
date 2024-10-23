#ifndef _serverWorker_h__
#define _serverWorker_h__
#include <memory>
#include "routeWorker.h"

struct ISession;
class serverWorker: public routeWorker
{
public:
    serverWorker ();
    ~serverWorker ();
	int onLoopBeginBase() override;
	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet) override;
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
protected:
	std::unique_ptr<ISession*[]>	m_connectors;
private:
};
#endif
