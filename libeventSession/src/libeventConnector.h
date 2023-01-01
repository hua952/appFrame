#ifndef libeventConnector_h_
#define libeventConnector_h_
#include "libeventSessionCom.h"
#include "comFun.h"

struct event_base;
class libeventServerCom;
class libeventConnector: public libeventSessionCom
{
public:
    libeventConnector ();
    ~libeventConnector ();
	int    init(libeventServerCom* pServer,  const char* ip, uword port);
    int close() override;
    //libeventSessionCom&    sessionCom ();
    //libeventServerCom*    server ();
	int      connect();
    uword    port ();
    void  setPort (uword va);
	const char* ip();
	void  setIp (const char* szIp);
    //uqword    userData ();
    //void  setUserData (uqword va);
private:
    //uqword  m_userData;
	char   m_ip[16];
    uword  m_port;
    //libeventServerCom*  m_server;
    //libeventSessionCom  m_sessionCom;
};
#endif
