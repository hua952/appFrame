#ifndef libeventListener_h__
#define libeventListener_h__
#include <unordered_map>
#include <memory>
#include "comFun.h"
#include "libeventSessionCom.h"

class libeventServerCom;
class libeventServerSession;
class libeventListener
{
public:
	
    libeventListener ();
    ~libeventListener ();
	libeventServerCom*  getServer ();
	struct event_base*  getBase ();
	int    init(libeventServerCom* pServer, const char* ip, uword port);
    uqword    userData ();
    void  setUserData (uqword va);
private:
    uqword  m_userData;
	libeventServerCom*  m_pServer;
	char ip[16];
	int      m_shocket;
	uword    port;
};
#endif
