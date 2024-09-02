#ifndef _gateRouteServer_h__
#define _gateRouteServer_h__
#include <memory>
#include "routeServerBase.h"

class gateRouteServer: public routeServerBase
{
public:
    gateRouteServer ();
    ~gateRouteServer ();
protected:
	int processToGatePack(packetHead* pPack) override;
	int procProx(packetHead* pPack) override;
	int processBroadcastPack(packetHead* pPack);
private:
};
#endif
