#ifndef _testRpc_h__
#define _testRpc_h__
#include "testLogicMsg.h"
struct testLogicAsk1Pak
{
	udword  accId;
	ubyte   nameNum;
	char    name[22];
};

netPacketHead* newTestLogicAsk1Obj();
void	delTestLogicAsk1Obj();
#endif
