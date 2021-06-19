#ifndef _IAsyScession_h__
#define _IAsyScession_h__

#include "typeDef.h"
#include "packet.h"
enum IAsyScessionState
{
};
struct IAsyScession
{
	virtual int send(packetHead* pack) = 0;
};
#endif 
