#ifndef _IScession_h__
#define _IScession_h__

#include "typeDef.h"
#include "packet.h"

typedef udword SessionIDType;

#define EmptySessionID 0xffffffff

enum SessionState
{
	SessionState_waitCon,
	SessionState_Offline,
	SessionState_Online,
	SessionState_close,
	SessionState_Unknow,
};

class ITcpServer;
class ISession
{
 public:
	virtual SessionState state() = 0;
    virtual int send(packetHead* pack) = 0;
    virtual int close() = 0;
	virtual void*   userData() = 0;
	virtual void    setUserData(void* pData, int len) = 0;
	//virtual void    setUserData(void* pData) = 0;
	virtual SessionIDType id() = 0;
	virtual void setId(SessionIDType id) = 0;
	virtual ITcpServer* getServer () = 0;
};

//typedef Session* PSession;

typedef packetHead* (*allocPackFTForSession)(udword udwSize);
typedef void		(*freePackFTForSession)(packetHead* pack);

struct SessionInitData
{
	allocPackFTForSession fnAllocPack;
	freePackFTForSession  fnFreePack;
};

struct SessionEndPoint
{
	udword  token;
	uword	port;
	char    ip[16];
};

#define MaxListenerNum 4


struct createSessionServerData
{
	udword		          m_listenerNum;
	udword				  m_connectorNum;
	SessionEndPoint       m_listenEndPointS[MaxListenerNum];
	SessionEndPoint       m_connectorEndPointS[1];
};

extern "C"
{
	int initSessionGlobalData(const SessionInitData& initData);
	int createSessionServer(const createSessionServerData& initData, udword& serverId);
}

#endif

