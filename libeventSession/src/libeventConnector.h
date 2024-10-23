#ifndef libeventConnector_h_
#define libeventConnector_h_
#include "libeventSessionCom.h"
#include "comFun.h"

struct event_base;
class libeventServerCom;
typedef std::pair<libeventServerCom*, SessionIDType> forConInfo;
class libeventConnector: public libeventSessionCom
{
public:
    libeventConnector ();
    ~libeventConnector ();
	int    init(libeventServerCom* pServer,  const char* ip, uword port);
    // int close() override;
	int      connect();
    uword    port ();
    void  setPort (uword va);
	const char* ip();
	void  setIp (const char* szIp);
	void  onOffline();
private:
	char   m_ip[16];
    uword  m_port;
};
#endif
