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
	virtual int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet);
	int processNetPackFun(ISession* session, packetHead* pack)override;

	virtual int localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc);
	void onWritePack(ISession* session, packetHead* pack) override;
	int onLoopFrameBase() override;

private:
};
#endif
