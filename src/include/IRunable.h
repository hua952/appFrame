#ifndef _IRunable_h__
#define _IRunable_h__
#include "packet.h"
struct IRunable
{
	virtual int stop() = 0;
	virtual int pushPacket(packetHead* pack) = 0;

};
#endif
