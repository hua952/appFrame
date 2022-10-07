#ifndef _libevListener_h__
#define _libevListener_h__

#include "typeDef.h"

#include<event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class libevListener
{
public:
	libevListener();
	virtual ~libevListener();
	virtual void release();
	int  listen(const char* szIP, udword udwPort, udword udwQuNum);
	evconnlistener* evListener();
	virtual libeventSession* _newLibeventSession();
	udword  token();
	void    setToken(udword udwT);
protected:
	udword            m_token;
	uword             m_port;
	char              m_ip[16];
};

#endif
