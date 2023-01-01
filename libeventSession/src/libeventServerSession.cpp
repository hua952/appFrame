#include "libeventServerSession.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "libeventListener.h"
#include "libeventServerCom.h"

libeventServerSession::libeventServerSession ()
{
}
libeventServerSession::~libeventServerSession ()
{
}
/*
libeventListener*   libeventServerSession:: listener ()
{
    return m_listener;
}

void  libeventServerSession::setListener (libeventListener*  va)
{
    m_listener = va;
}

ITcpServer* libeventServerSession::getServer ()
{
	return m_listener->getServer ();
}
*/
int libeventServerSession::close()
{
	auto pServer = serverCom();// m_listener->getServer ();
	auto nId = id();
	return pServer->closeSession (nId);
}

