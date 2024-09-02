#ifndef _clientRouteServer_h__
#define _clientRouteServer_h__
#include <memory>
#include "routeServerBase.h"
#include "comTcpNet.h"

class clientRouteServer: public routeServerBase, public comTcpNet
{
public:
    clientRouteServer ();
    ~clientRouteServer ();
protected:
	int processToGatePack(packetHead* pPack) override;
	int procToOtherAppServerPack(packetHead* pPack) override;
private:
	ISession*  m_defSession;
};
#endif
