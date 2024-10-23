#ifndef  libeventServerSession_h__
#define  libeventServerSession_h__
#include "libeventSessionCom.h"

class libeventListener;
class libeventServerSession:public  libeventSessionCom
{
public:
    libeventServerSession ();
    ~libeventServerSession ();
    //libeventListener*    listener ();
    //void  setListener (libeventListener* va);
    // int close() override;
	//ITcpServer* getServer () override;
private:
    //libeventListener*  m_listener;
};
#endif
