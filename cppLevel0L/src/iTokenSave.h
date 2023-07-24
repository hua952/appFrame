#ifndef iTokenSave_h__
#define iTokenSave_h__
#include "packet.h"
#include "ISession.h"

struct tokenInfo
{
	NetTokenType   oldToken;
	SessionIDType  sessionId;
};
struct iTokenSave
{
	virtual int netTokenPackInsert (NetTokenType token, const tokenInfo& info) = 0;
	virtual int netTokenPackErase (NetTokenType token) = 0;
	virtual tokenInfo* netTokenPackFind (NetTokenType token) = 0;
};
#endif
