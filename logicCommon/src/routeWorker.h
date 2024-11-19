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
	virtual int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet, SessionIDType objSession);
	virtual void sendHeartbeat () = 0;
	// virtual int  sendBroadcastPack (packetHead* pack) = 0;

	int  recPacketProcFun (ForLogicFun* pForLogic) override;
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void onWritePack(ISession* session, packetHead* pack) override;
	int onLoopFrameBase() override;
	int onLoopBeginBase() override;
private:
	int localProcessNetPackFun(ISession* session, packetHead* pack);
};
#endif
