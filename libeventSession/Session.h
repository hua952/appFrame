#ifndef _Session_h
#define _Session_h
#include "ISession.h"
#include "typeDef.h"
#include "packet.h"

typedef udword SessionIDType;

class Session:public ISession
{
 public:
    Session();
    ~Session();
	SessionState state() overwrite;
    void send(packetHead* pack) overwrite;
    int close() overwrite;
	void*   userData()  overwrite;
	void    setUserData(void* pData)  overwrite;
	SessionIDType id()  overwrite;
	void setId(SessionIDType id)  overwrite;
	void onReadBuff();
 protected:
	bool procOncePack();
	void pushPackToSendList(packetHead* pack);
	void afterSend(packetHead* pack);
	virtual bool onlyWritePack(ubyte* pBuff, udword udwSize) = 0;
	virtual udword onlyReadPack(ubyte* pBuff, udword udwSize) = 0;
	//virtual void realSend(packetHead* pack) = 0;
	bool tryReadBuff();
 private:
	udword  m_udwCurIndex;
	packetHead	m_Head;
	void*       m_pUserData;
    SessionIDType  m_ID;
	SessionState   m_state;
};
typedef Session* PSession;

class PSessionCmp
{
public:
	bool operator()(const PSession& pA, const PSession& pB);
};

struct SessionComDate
{
	allocPackFTForSession fnAllocPack;
	freePackFTForSession  fnFreePack;
};

SessionInitData& getCallBackS();
#endif
