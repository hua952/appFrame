#include<string.h>
#include <arpa/inet.h>
#include"libevListener.h"
#include"iLogic.h"
#include"rlog.h"
#include"libeventSession.h"
#include"libeventNet.h"
#include"funLog.h"
#include"sessionMgr.h"

int libevListener ::init(const char* ip, uword port, udword token)
{
	strNCpy (m_ip, sizeof(m_ip), ip);
	m_port = port;
	m_token = token;
}


void on_errOrObjClosSession(libeventSession* pS);
libeventSession* newLibeventSession();

libevListener::libevListener()
{
	m_evListener = NULL;	
	m_udwToken = 0;
	//m_pMsgPmp = NULL;
}

libevListener::~libevListener()
{
	release();	
	rTrace("~libevListener()");
}
libeventSession* libevListener::_newLibeventSession()
{
	return newLibeventSession();
}
/*
void libevListener::_delLibeventSession(libeventSession* pS)
{
	void delLibeventSession(libeventSession* pS);
	delLibeventSession(pS);
}
*/
void libevListener::release()
{
	if(NULL != m_evListener )
	{
		evconnlistener_free(m_evListener);
		m_evListener = NULL;
	}
}

udword  libevListener::token()
{
	return m_udwToken;
}

void    libevListener::setToken(udword udwT)
{
	m_udwToken = udwT;
}

evconnlistener* libevListener:: evListener()
{
	return m_evListener;
}

static void socket_read_cb(bufferevent *bev, void *arg)  
{  
	LOG_FUN_CALL
    libeventSession* pS = (libeventSession*)arg;
	pS->onReadBuff();
}  
  
static void socket_write_cb(bufferevent *bev, void *arg)  
{  
LOG_FUN_CALL
	libeventSession* pS = (libeventSession*)arg;  
	pS->processSend();
}  
  
static void socket_event_cb(bufferevent *bev, short events, void *arg)  
{  
LOG_FUN_CALL
	libeventSession* pS = (libeventSession*)arg;  
	on_errOrObjClosSession(pS);
} 

static void listener_cb(evconnlistener *listener, evutil_socket_t fd,  
                 struct sockaddr *sock, int socklen, void *arg)  
{  
LOG_FUN_CALL
	rInfo("listener_cb"); 
    libeventNet* pApp = &libeventNet::single();//(libeventNet*)arg;  
    libevListener* pThis = (libevListener*)arg;
	libeventSession* pS = pThis->_newLibeventSession();
    //为这个客户端分配一个bufferevent  
    bufferevent *bev =  bufferevent_socket_new(pApp->base(), fd,  
                                               BEV_OPT_CLOSE_ON_FREE);  
  
    bufferevent_setcb(bev, socket_read_cb,  socket_write_cb, socket_event_cb, pS);  
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  
	pS->setBufEvent(bev);
	MsgPmp* pPmp = pThis->msgPmp();
	if(NULL != pPmp)
	{
		pS->setMsgPmp(pPmp);
	}
	SessionIDType id = SessionMgr::single().nextID();
	pS->setId(id);
	bool bOK = SessionMgr::single().insert(pS);
	assert(bOK);
	bOK = pThis->onAccept(pS);
	iLogic* pLogic =  libeventNet::single().getLogic();
	uqword uqwToken = pThis->token();
	pLogic->onAccept(pS, (void*)uqwToken);
	//SessionMgr::PSessionMap &psm = SessionMgr::single().sessionMap();
	//rTrace("TestSessionMgrMapSize: "<<psm.size());
	//psm.visit(NULL, s_TestSessionMgr);
}

int  libevListener::listen(const char* szIP, udword udwPort, udword udwQuNum)
{
LOG_FUN_CALL
	struct sockaddr_in sin;  
    memset(&sin, 0, sizeof(struct sockaddr_in));  
    sin.sin_family = AF_INET;  
	//sin.sin_addr.s_addr = inet_addr(retA[0]);
    inet_aton(szIP, &sin.sin_addr);
    sin.sin_port = htons(udwPort);  

    m_evListener = evconnlistener_new_bind(libeventNet::single().base(), listener_cb,this,  
                                      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,  
                                      udwQuNum, (struct sockaddr*)&sin,  
                                      sizeof(struct sockaddr_in)); 
	return 0;
}

