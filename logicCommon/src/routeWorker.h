#ifndef _routeWorker_h__
#define _routeWorker_h__
#include <memory>
#include "packet.h"
#include "internalRpc.h"
#include "logicWorker.h"
#include "comTcpNet.h"


class routeWorker:public logicWorker, public comTcpNet
{
public:
    routeWorker ();
    ~routeWorker ();
	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	virtual int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession);
	int processNetPackFun(ISession* session, packetHead* pack)override;

	int localProcessNetPackFun(ISession* session, packetHead* pack);

	virtual void sendHeartbeat () = 0;

	void onWritePack(ISession* session, packetHead* pack) override;
	int onLoopFrameBase() override;
	int onLoopBeginBase() override;

	virtual int  sendBroadcastPack (packetHead* pack) = 0;
private:
};
#endif
