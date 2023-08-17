#ifndef iPackSave_h__
#define iPackSave_h__
#include "packet.h"
#include "ISession.h"
struct tokenInfo
{
	NetTokenType   oldToken;
	SessionIDType  sessionId;
};
struct iPackSave
{
	virtual int netTokenPackInsert (packetHead* pack) = 0;
	virtual int netTokenPackErase (NetTokenType token) = 0;
	virtual packetHead* netTokenPackFind (NetTokenType token) = 0;

	virtual int oldTokenInsert (NetTokenType newToken, tokenInfo& rInfo) = 0;
	virtual int oldTokenErase (NetTokenType newToken) = 0;
	virtual tokenInfo* oldTokenFind (NetTokenType newToken) = 0;

};
#endif
