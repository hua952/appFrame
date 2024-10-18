#ifndef _gateRouteWorker_h__
#define _gateRouteWorker_h__
#include <memory>
#include "comTcpNet.h"
#include "routeWorker.h"
#include <map>

class gateRouteWorker: public routeWorker
{
public:
    gateRouteWorker ();
    ~gateRouteWorker ();
	
	using appMap = std::map<uword, ISession*>;
	int onLoopBeginBase() override;
	int  regAppRoute (ubyte group, SessionIDType sessionId);

	int sendPackToRemoteAskProc(packetHead* pPack, sendPackToRemoteRet& rRet) override;
	int localProcessNetPackFun(ISession* session, packetHead* pack, bool& bProc) override;
private:
	appMap  m_appMap;
};
#endif
