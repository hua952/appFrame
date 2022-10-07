#ifndef _libevent_session_h_
#define _libevent_session_h_
#include"Session.h"

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>  

class libeventSession:public Session{
public:
	libeventSession();
	virtual ~libeventSession();
    void close() overwrite;
	void   clearBev();
	bufferevent* bufEvent();
	void		 setBufEvent(bufferevent* pB);
protected:
	bool onlyWritePack(ubyte* pBuff, udword udwSize) overwrite;
	udword onlyReadPack(ubyte* pBuff, udword udwSize) overwrite;
	bufferevent *m_pBev;
};

#endif
