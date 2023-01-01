#include "libeventListener.h"
#include "libeventServerSession.h"
#include "libeventServerCom.h"
#include <memory>
#include "myAssert.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "loop.h"
#include "nLog.h"

libeventListener::libeventListener ()
{
	m_pServer = nullptr;
}

libeventListener::~libeventListener ()
{
}

typedef struct sockaddr SA;
static int tcp_server_init(int port, int listen_num)
{
	int errno_save;
	evutil_socket_t listener;
	int nRet = -1;
	listener = ::socket(AF_INET, SOCK_STREAM, 0);
	do {
		if( listener == -1 ) {
			break;
		}
		evutil_make_listen_socket_reuseable(listener);

		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = 0;
		sin.sin_port = htons(port);

		if( ::bind(listener, (SA*)&sin, sizeof(sin)) < 0 ) {
			nError ("bind Err port = "<<port);
			break;
		}

		if( ::listen(listener, listen_num) < 0) {
			nError ("listen Err port = "<<port);
			break;
		}
		evutil_make_socket_nonblocking(listener);
		nRet = listener;
	} while(0);
	if (-1 == nRet && -1 != listener) {
		errno_save = errno;
		evutil_closesocket(listener);
		errno = errno_save;
	}
	return nRet;
}

static void socket_read_cb(bufferevent* bev, void* arg)
{
	auto pNode = (libeventServerSession*)arg;
	//auto pL = pNode->listener ();
	size_t len = 0;
	do {
		auto pBuff = pNode->getBuff();
		//nTrace (__FUNCTION__<<" 0000 pBuff = "<<pBuff);
		auto neet = pNode->getNeet();
		len = bufferevent_read(bev, pBuff, neet);
		//nTrace (__FUNCTION__<<" 1111 neet = "<<neet<<" len = "<<len);
		auto pack = pNode->afterReadBuff(len);
		if (pack) {
			auto id = pNode->id ();
			//auto pL = pNode->listener ();
			//auto pServer = pL->getServer ();
			auto pServer = pNode->serverCom ();
			auto fun = pServer->procPackfun ();
			auto pN = P2NHead (pack);
			/*
			nTrace (__FUNCTION__<<" rec Pack for app pack = "<<pack<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength);
					*/
			auto nRet = fun (pNode, pack);
			if (procPacketFunRetType_doNotDel != nRet) {
				auto freeF = freePackFun ();
				freeF (pack);
			}
		}
	} while (len > 0);
}

static void event_cb(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF) {
        nInfo("connection closed");
	}
    else if (event & BEV_EVENT_ERROR) {
        nError("some other error re = {}", EVUTIL_SOCKET_ERROR());
	}
	auto pSS = (libeventServerSession*) arg;
	pSS->setState (SessionState_Offline);
	/*
	auto pL = pSS->listener ();
	auto pServer = pL->getServer ();
	auto id = pSS->id();
	pServer->closeSession (id);
	*/
}

static void accept_cb(intptr_t fd, short events, void* arg)
{
	auto pL = (libeventListener*) arg;
    evutil_socket_t sockfd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
    evutil_make_socket_nonblocking(sockfd);
    nInfo ("accept a client {} ", sockfd);
    struct event_base* base = pL->getBase ();
    bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
	auto pSSS = std::make_shared<libeventServerSession>();
	auto pSS = pSSS.get();
	pSS->setBev (bev);
	//pSS->setListener (pL);
	auto pServer = pL->getServer ();
	pSS->setServerCom (pServer);
	auto id = pServer->nextSessionId();
	pSS->setId (id);
	auto& rSSM = pServer->getServerSessonMap ();
	rSSM[id] = pSSS;
	auto pRS = pSS;
	auto& rSM =  pServer->getSessonMap();
	rSM[id] = pRS;
    bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, pRS);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
	auto fun = pServer->acceptFun ();
	auto token = pL->userData ();
	pSS->setState (SessionState_Online);
	fun (pSS, token);
}

uqword  libeventListener::  userData ()
{
    return m_userData;
}
void  libeventListener::setUserData (uqword  va)
{
    m_userData = va;
}

libeventServerCom*  libeventListener::getServer ()
{
	return m_pServer;
}

int  libeventListener::init(libeventServerCom* pServer, const char* ip, uword port)
{
	int nRet = 0;
	m_pServer = pServer;
	int listener = tcp_server_init(port, 10);
	do {
		if( listener == -1 )
		{
			nError(" tcp_server_init error ");
			nRet = 1;
			break;
		}
		auto Lbase = pServer->getBase();
		struct event* ev_listen = event_new(Lbase, listener, EV_READ | EV_PERSIST,
				accept_cb, this);
		event_add(ev_listen, NULL);
		//myAssert (0 == nR);
	} while (0);
	return nRet;
}

struct event_base*  libeventListener::  getBase ()
{
    return m_pServer->getBase();
}

