#ifndef libeventSessionCom_h_
#define libeventSessionCom_h_
#define c_EmptySessionId 0xffffffff
#include "packet.h"
#include <memory>
#include "ISession.h"

class libeventServerSession;
class libeventListener;
class libeventConnector;
class libeventServerCom;
//typedef sessionId SessionIDType;
class libeventSessionCom: public ISession
{
public:
	libeventSessionCom();
	~libeventSessionCom();
	ubyte*   getBuff ();
	udword   getNeet ();
	packetHead*    afterReadBuff(udword nS);
	struct bufferevent*  getBev ();
	void  setBev (struct bufferevent* bev);

	SessionState state() override;
	void  setState (SessionState s);
    int send(packetHead* pack) override;
    //virtual int close() = 0;
	void*			 userData () override;
	void			 setUserData(void* pData, int len) override;
	SessionIDType id() override;
	void setId(SessionIDType id) override;
	void trySend ();
	void cleanWritePack ();
	ITcpServer* getServer () override;
	netPacketHead    netHead;
	packetHead*       pack;
	struct bufferevent* bev;
	std::unique_ptr<char[]>              m_pUserData;
	udword			 curIndexRead;
	udword			 m_exitTimePoint;
    libeventServerCom*    serverCom ();
    void  setServerCom (libeventServerCom* va);
private:
    libeventServerCom*  m_serverCom;
	packetHead	m_writeH;
	SessionIDType m_id;
	SessionState m_state;
};

#endif
