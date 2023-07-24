#ifndef iPackSave_h__
#define iPackSave_h__
#include "packet.h"
struct iPackSave
{
	virtual int threadTokenPackInsert (packetHead* pack) = 0;
	virtual int threadTokenPackErase (NetTokenType token) = 0;
	virtual packetHead* threadTokenPackFind (NetTokenType token) = 0;


	virtual int netTokenPackInsert (packetHead* pack) = 0;
	virtual int netTokenPackErase (NetTokenType token) = 0;
	virtual packetHead* netTokenPackFind (NetTokenType token) = 0;
};
#endif
