#include "strFun.h"
#include "libeventConnector.h"
#include "libeventServerCom.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "loop.h"
#include "nLog.h"

libeventConnector::libeventConnector ()
{
	//m_server = nullptr;
}
libeventConnector::~libeventConnector ()
{
}

uword  libeventConnector::  port ()
{
    return m_port;
}

void libeventConnector:: setPort (uword  va)
{
    m_port = va;
}

const char* libeventConnector::ip()
{
	return m_ip;
}

void  libeventConnector::setIp (const char* szIp)
{
	strNCpy (m_ip, sizeof (m_ip), szIp);
}

int tcp_connect_server(const char* server_ip, int port)
{
	nTrace (__FUNCTION__);
	typedef struct sockaddr SA;
    int sockfd = -1;
	int status = 0;
	int save_errno = 0;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr) );

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
	status = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if( status != 1 ) //the server_ip is not valid value
    {
        errno = EINVAL;
		nError ("inet_pton error");
        return -1;
    }

    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 ) {
		nError ("socket error");
        return sockfd;
	}

    status = ::connect(sockfd, (SA*)&server_addr, sizeof(server_addr) );

    if( status == -1 ) {
        save_errno = errno;
		closesocket(sockfd);
        errno = save_errno; //the close may be error  
        return -1;
    }
    evutil_make_socket_nonblocking(sockfd);
    return sockfd;
}

static void server_msg_cb(struct bufferevent* bev, void* arg)
{
	auto pConnector = (libeventConnector*)arg;
	auto pServer = pConnector->serverCom();
	auto fun = pServer->procPackfun ();
	auto pNode = (pConnector);
	size_t len = 0;
	do {
		auto pBuff = pNode->getBuff ();
		auto neet  = pNode->getNeet ();
		len = bufferevent_read(bev, pBuff, neet);
		//nTrace (__FUNCTION__<<" 1111 neet = "<<neet<<" len = "<<len);
		auto pack = pNode->afterReadBuff (len);
		if (pack) {
			auto pN = P2NHead(pack);
			if (fun) {
				auto id = pNode->id ();
				/*
				nTrace (__FUNCTION__<<" rec Pack for app pack = "<<pack<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength);
					*/
				auto nRet = fun (pNode, pack);
				if (procPacketFunRetType_doNotDel != nRet ) {
					auto freeF = freePackFun ();
					freeF (pack);
				}
			}
			//pNode->updateExitTimePoint ();
		}
	} while (len > 0);
}

static void event_cb(struct bufferevent *bev, short event, void *arg)
{
	auto pConnector = (libeventConnector*)arg;
    if (event & BEV_EVENT_EOF) {
        nInfo("connection closed");
	} else if (event & BEV_EVENT_ERROR) {
        nError("some other error");
	}
	//pConnector->close();
	pConnector->setState (SessionState_Offline);
}

int    libeventConnector::connect()
{
	int nRet = 0;
	auto pServer = serverCom ();// m_server;
	nTrace (__FUNCTION__<<" pServer = "<<pServer);
	auto baseL = pServer->getBase ();
	int sockfd = tcp_connect_server(m_ip, m_port);
	do {
		if( sockfd == -1) {
			nError("tcp_connect error ip = " <<(char*)m_ip<<" port = "<<port());
			nRet = 1;
			break;
		}
		auto pQ = (uqword*)(userData());
		nInfo("connect to server successful userData = "<<*pQ);
		auto bev = bufferevent_socket_new(baseL, sockfd,
				BEV_OPT_CLOSE_ON_FREE);
		setBev (bev);
		bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, this);
		bufferevent_enable(bev, EV_READ | EV_PERSIST);
	} while (0);
	return nRet;
}
/*
uqword  libeventConnector::  userData ()
{
    return m_userData;
}

void  libeventConnector::setUserData (uqword  va)
{
    m_userData = va;
}
*/
int   libeventConnector:: init(libeventServerCom* pServer, const char* ip, uword port)
{
	nTrace (__FUNCTION__<< "ip = "<<ip<<" port = "<<port);
	int nRet = 0;
	//m_server = pServer;
	setServerCom (pServer);
	auto baseL = pServer->getBase ();
	strcpy_s (m_ip, sizeof (m_ip) / sizeof (m_ip[0]), ip);
	m_port = port;
	return nRet;
}

int libeventConnector::close()
{
	auto nId = id ();
	auto pServer = serverCom ();
	return pServer->closeSession (nId);
}
/*
libeventServerCom*   libeventConnector:: server ()
{
    return m_server;
}
*/

