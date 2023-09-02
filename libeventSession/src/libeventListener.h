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
    void* userData ();
    void  setUserData (void* userData, udword userDataLen);
	udword  userDataLen ();
	void  setUserDataLen (udword v);
private:
	udword  m_userDataLen;
	std::unique_ptr<char[]>              m_pUserData;
    // void* m_userData;
	libeventServerCom*  m_pServer;
	char m_ip[16];
	int      m_shocket;
	uword    m_port;
};
#endif
