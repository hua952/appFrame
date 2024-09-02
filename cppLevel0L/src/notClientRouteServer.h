#ifndef _notClientRouteServer_h__
#define _notClientRouteServer_h__
#include <memory>
#include "routeServerBase.h"

class notClientRouteServer:public routeServerBase
{
public:
    notClientRouteServer ();
    ~notClientRouteServer ();
private:
	virtual int procToOtherAppServerPack(packetHead* pPack) = 0;
};
#endif
