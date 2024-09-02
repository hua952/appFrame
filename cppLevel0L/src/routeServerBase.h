#ifndef _routeServerBase_h__
#define _routeServerBase_h__
#include <memory>

#include "workServer.h"
#include "comTcpNet.h"

class routeServerBase: public workServer, public comTcpNet
{
public:
    routeServerBase ();
    ~routeServerBase ();
protected:
	virtual int processToGatePack(packetHead* pPack) = 0;
	virtual int procToOtherAppServerPack(packetHead* pPack) = 0;
	int processOncePack(packetHead* pPack) override;
private:
};
#endif
