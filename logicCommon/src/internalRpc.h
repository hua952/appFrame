#ifndef _internalRpc_h
#define _internalRpc_h

#include "msg.h"

class sendPackToRemoteAskMsg :public CMsgBase
{
public:
	sendPackToRemoteAskMsg ();
	sendPackToRemoteAskMsg (packetHead* p);
};

struct sendPackToRemoteRet
{
    udword    m_result;
};
class sendPackToRemoteRetMsg :public CMsgBase
{
public:
	sendPackToRemoteRetMsg ();
	sendPackToRemoteRetMsg (packetHead* p);
};

class recRemotePackForYouAskMsg :public CMsgBase
{
public:
	recRemotePackForYouAskMsg ();
	recRemotePackForYouAskMsg (packetHead* p);
};

struct regAppRouteAsk 
{
    ubyte m_appGrupId;
};

class regAppRouteAskMsg :public CMsgBase
{
public:
	regAppRouteAskMsg ();
	regAppRouteAskMsg (packetHead* p);
};


struct regAppRouteRet
{
    udword    m_result;
};

class regAppRouteRetMsg :public CMsgBase
{
public:
	regAppRouteRetMsg ();
	regAppRouteRetMsg (packetHead* p);
};
#endif


