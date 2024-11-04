#ifndef _routeWorker_h__
#define _routeWorker_h__
#include <memory>
#include "packet.h"
#include "internalRpc.h"
#include "logicWorker.h"
#include "comTcpNet.h"

packetHead* nClonePack(netPacketHead* pN);
packetHead* clonePack(packetHead* p);

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
private:
};
#endif
