#include "sysinc.h"
#include "comFun.h" // E: comFun.h: No such file or directory

#include "../libeventListener.h"
#include "../nLog.h"
#include "../libeventServerSession.h"
#include "../libeventServerCom.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"

/*
void socket_read_cb(bufferevent* bev, void* arg);
void event_cb(struct bufferevent *bev, short event, void *arg);
void accept_cb(intptr_t fd, short events, void* arg)
{
	auto pL = (libeventListener*) arg;
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
	evutil_make_socket_nonblocking(sockfd);
	nInfo ("accept a client {} "<<sockfd);
	struct event_base* base = pL->getBase ();
	bufferevent* bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
	auto pSSS = std::make_shared<libeventServerSession>();
	auto pSS = pSSS.get();
	pSS->setBev (bev);
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
*/
void event_callback_com(intptr_t, short sig, void * pArg)
{
	auto pSig = (sigInfo*)pArg;
	pSig->second(sig, pSig->pUserData);
}
