#include<dlfcn.h>
#include <arpa/inet.h>
#include"strFun.h"
#include"libeventSession.h"
#include<iostream>

static	void s_delSessionVisit(void* pU, PSession& pS)
{
	libeventSession* pLS = (libeventSession*)(pS);
	rTrace("s_delSessionVisit"<<pS->id());
	delLibeventSession(pLS);
}


static void timeout_cb(int fd, short event, void *params)
{
LOG_FUN_CALL
	//rInfo("timeout_cb");
	iLogic* pL = libeventNet::single().getLogic();
	pL->onFram();

	//frame()->getTimerMgr()->onFram();
}

static void s_sig_handler(int signo, short events, void* arg) 
{

	//rInfo("s_sig_handler");
	iLogic* pL = libeventNet::single().getLogic();
	pL->onSignal(signo);
	//rInfo("s_sig_handler2");
	if(SIGINT == signo || SIGUSR1 == signo)
	{
		libeventNet::single().stop();
	}
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

static void socket_read_cb_client(bufferevent *bev, void *arg)  
{  
	LOG_FUN_CALL
	libeventConnector* pC = (libeventConnector*)arg;
    libeventSession* pS = (libeventSession*)(pC->session());
	if(pS)
	{
		pS->onReadBuff();
	}
}  
  
static void socket_write_cb_client(bufferevent *bev, void *arg)  
{  
LOG_FUN_CALL
	libeventConnector* pC = (libeventConnector*)arg;
    libeventSession* pS = (libeventSession*)(pC->session());
	if(pS)
	{
		pS->processSend();
	}
} 

static void socket_event_cb(bufferevent *bev, short events, void *arg)  
{  
LOG_FUN_CALL
	libeventSession* pS = (libeventSession*)arg;  
	on_errOrObjClosSession(pS);
}

static void socket_event_cb_client(bufferevent *bev, short events, void *arg)  
{  
LOG_FUN_CALL

	libeventConnector* pC = (libeventConnector*)arg;
	ubyte ubyS = pC->state();
	if(events & BEV_EVENT_CONNECTED)
    {
		libeventSession* pS = newLibeventSession();
		pS->setBufEvent(bev);
		SessionIDType id = SessionMgr::single().nextID();
		pS->setId(id);
		bool bOK = SessionMgr::single().insert(pS);
		assert(bOK);
		MsgPmp* pPmp = pC->msgPmp();
		if(pPmp)
		{
			pS->setMsgPmp(pPmp);
		}
		pC->setSession(pS);
		if(0 == ubyS || 3 == ubyS)
		{
			if(0 == ubyS)
			{
				libeventNet::single().onConnect(pC);
			}
			else
			{
				libeventNet::single().onReConnect(pC);
			}
			iLogic* pLogic =  libeventNet::single().getLogic();
			uqword uqwT = pC->token();
			pLogic->onConnect(pC, (void*)uqwT);
			rDebug("BEV_EVENT_CONNECTED");
			pC->setState(1);
		}
		else if(2 == ubyS)
		{
			pC->setState(1);
			libeventNet::single().onReConnect(pC);
		}
	}
	else
	{
		if(0 == ubyS)
		{
			libeventNet::single().onConnect(pC);
		}
		else if(1 == ubyS)
		{

			libeventSession* pS = (libeventSession*)(pC->session());
			libeventNet::single().onCloseSession(pS);
			iLogic* pLogic =  libeventNet::single().getLogic();
			pLogic->onClose(pS);
			delLibeventSession(pS);
			pC->setSession(NULL);
			pC->setState(2);
		}
		else if(2 == ubyS)
		{
			rTrace("ReConnect Fail");
		}
	}
}

static void s_TestSessionMgr(void* pU, PSession& pS)
{
	libeventSession* pLS = (libeventSession*)pS;
	rTrace("s_TestSessionMgr"<<pLS->id());
}
 
libeventSession* newLibeventSession()
{
}

void delLibeventSession(libeventSession* pS)
{
}

void on_errOrObjClosSession(libeventSession* pS)
{
}

libeventNet::libeventNet()
{
	m_pBase = NULL;
	m_frameSetp = 5000;
}

libeventNet::~libeventNet()
{
	if(NULL != m_pBase)
	{
		event_base_free(m_pBase);
		m_pBase = NULL;
	}
}


void libeventNet::run()
{
	LOG_FUN_CALL
	struct timeval tv = {0, m_frameSetp/* 5000*/};
	m_pTimeout = event_new(m_pBase, -1, EV_PERSIST, timeout_cb, NULL);
	evtimer_add(m_pTimeout, &tv);
    m_pSigInt = evsignal_new(m_pBase, SIGINT,  s_sig_handler, NULL);
	event_add(m_pSigInt, NULL);
    m_pSigUser1 = evsignal_new(m_pBase, SIGUSR1,  s_sig_handler, NULL);
	event_add(m_pSigUser1, NULL);
	event_base_dispatch(m_pBase);
	
	rTrace("~libevent Exit loop");

	if(NULL != m_pTimeout)
	{
		event_del(m_pTimeout);
		m_pTimeout = NULL;
	}

	if(NULL != m_pSigInt)
	{
		event_del(m_pSigInt);
		m_pSigInt= NULL;
	}

	if(NULL != m_pSigUser1)
	{
		event_del(m_pSigUser1);
		m_pSigUser1 = NULL;
	}
	delSingleton();
}

void libeventNet::stop()
{
LOG_FUN_CALL
	 struct timeval ten_sec;
	 ten_sec.tv_sec = 1;
	 ten_sec.tv_usec = 0;
	 event_base_loopexit(m_pBase, &ten_sec);
//	event_base_loopbreak(m_pBase);
	
}
 


int libeventNet::listen(uqword& handle, udword udwToken, const char* szIP, udword udwPort, udword udwQuLen, udword ForwebShowck, const char* szName)
{
LOG_FUN_CALL
	int nCurLN = SessionMgr::single().curListenerNum();
	if(nCurLN >= SessionMgr::c_MaxListenNum)
	{
		return 1;
	}
	IMemMgr* pIMe = GetMemMgr();
	libevListener* pListener = NULL;
	if(0 == ForwebShowck)
	{
		pListener = (libevListener*)(pIMe->Malloc(sizeof(libevListener)));	
		pListener = new (pListener) libevListener;
	}
	else
	{
		pListener = (libevWebShockListener*)(pIMe->Malloc(sizeof(libevWebShockListener)));	
		pListener = new (pListener) libevWebShockListener;
	}
	if(szName)
	{
		pListener->setName(szName);
	}
	pListener->setToken(udwToken);
	SessionMgr::single().addListener(pListener, handle);
	int 	nR = pListener->listen(szIP, udwPort, udwQuLen);
	if(0 != nR)
	{
		return 2;
	}
	return 0;
}

int libeventNet:: procListen(char* szEndPoint)
{
	LOG_FUN_CALL
	const int c_BuffSize = 64;
	char szBuff[c_BuffSize];
	const int c_RetBuffSize = 6;
	char* retA[c_RetBuffSize];
	int nR = strR(szEndPoint, ':', retA, c_RetBuffSize);
	int nWeb = 0;
	udword udwToken = 0;
	assert(2 <= nR && nR <= 6);
	if(2 > nR)
	{
		return 1;
	}
	if(nR >= 3)
	{
		nWeb = atoi(retA[2]);
	}
	if(nR >= 4)
	{
		udwToken = atoi(retA[3]);
	}
	const char* szName = NULL;
	if(nR >= 5)
	{
		szName = retA[5];
	}
	uqword handle = 0;
	int nT = listen(handle, udwToken ,retA[0], atoi(retA[1]), 128, nWeb, szName); 
	if(0 != nT)
	{
		return 5;
	}
	iLogic* pLogic =  getLogic();
	if(nR >= 5)
	{
		if(NULL != pLogic)
		{
			//MsgPmp* pPmp = pLogic->getPmp(retA[4]);
			
			MsgPmp* pPmp = msgPmpMgr::single().getPmp(retA[4]);
			if(NULL != pPmp)
			{
				int nCurLN = SessionMgr::single().curListenerNum();
				listener** ppL = SessionMgr::single().listenerS();
				ppL[nCurLN - 1]->setMsgPmp(pPmp);
			}
		}
	}
	
	if(pLogic)
	{
		pLogic->onListen(handle, udwToken);
	}
	return 0;
}

struct event_base* libeventNet::base()
{
	return m_pBase;
}

int libeventNet:: procConnect(char* szEndPoint)
{
	LOG_FUN_CALL
	const int c_BuffSize = 64;
	char szBuff[c_BuffSize];
	const int c_RetBuffSize = 6;
	char* retA[c_RetBuffSize];
	int nR = strR(szEndPoint, ':', retA, c_RetBuffSize);
	assert(2 <= nR && nR <= 6);
	if(nR < 2 || nR > 6)
	{
		return 1;
	}
	udword pToken = 0; 
	if(nR > 2)
	{
		pToken = atoi(retA[2]);
	}
	int nTime = 1;
	if(nR > 3)
	{
		nTime = atoi(retA[3]);
		assert(nTime > 0);
	}
	const char* szPmpName = NULL;
	if(nR > 4)
	{
		szPmpName = retA[4];
	}
	const char* szName = NULL;
	if(nR > 5)
	{
		szName = retA[5];
	}
	uword uwPort = (uword)(atoi(retA[1]));
	for(int i = 0; i < nTime; i++)
	{
		int nR = connect(retA[0], uwPort, pToken, szPmpName, retA[5]);
		if(0 != nR)
		{
			return 2;
		}
	}
	return 0;
}

int libeventNet::connect(const char* szIP, udword udwPort, udword udwToken, const char* szPmpName, const char* szName)
{

LOG_FUN_CALL
	IMemMgr* pI = GetMemMgr();
/*	
	int nR = connectGetSession(szIP, udwPort);	
	if(0 != nR)
	{
		return 1;
	}
	*/
/*
	using namespace temp;
	stConData* pD = (stConData*)(pI->Malloc(sizeof(stConData)));
	strNCpy(pD->m_szIP, szIP, sizeof(pD->m_szIP));
	strNCpy(pD->m_szPmpName, szPmpName, sizeof(pD->m_szPmpName));
	pD->m_uwPort = udwPort;
	pD->m_OpertType = 1;
	pD->m_udwToken = udwToken;
*/
	libeventConnector* pC = (libeventConnector*)(pI->Malloc(sizeof(libeventConnector)));
	pC = new (pC) libeventConnector;
	pC->setIp(szIP);
	pC->setPort(udwPort);
	pC->setToken(udwToken);
	if(szPmpName)
	{

		//iLogic* pLogic =  getLogic();
		//if(pLogic )
		{
			//MsgPmp* pPmp = pLogic->getPmp(szPmpName);
			MsgPmp* pPmp = msgPmpMgr::single().getPmp(szPmpName);
			if(NULL != pPmp)
			{
				pC->setMsgPmp(pPmp);
			}
		}
	}	
	if(szName)
	{
		pC->setName(szName);
	}
	return connectGetSession(szIP, udwPort, pC);
}

int libeventNet::connectGetSession(const char* szIP, udword udwPort, libeventConnector* pC)
{
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(udwPort);
	inet_aton(szIP, &server_addr.sin_addr);

	struct bufferevent* bev = bufferevent_socket_new(base(), -1,
			BEV_OPT_CLOSE_ON_FREE);
	if(bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr,
			sizeof(server_addr)) < 0)
	{
		bufferevent_free(bev);
		return 1;
	}
	bufferevent_setcb(bev, socket_read_cb_client,  socket_write_cb_client, socket_event_cb_client, pC);  
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
		return 0;
}

int libeventNet::procCmd1(char* szText)
{
LOG_FUN_CALL
	const int c_BuffSize = 128;
	char szBuff[c_BuffSize];
	int nL = strlen(szText);
	assert(nL < c_BuffSize);
	strNCpy(szBuff, szText, c_BuffSize);
	const int c_RetBuffSize = 3;
	char* retA[c_RetBuffSize];
	int nR = strR(szBuff, '=', retA, c_RetBuffSize);
	assert(2==nR);
	if(2 != nR)
	{
		return 1;
	}
	if(0 == strcmp(retA[0], "frameSetp"))
	{
		m_frameSetp = atoi(retA[1]);	
	}
	return 0;
}


int libeventNet::procCmd2(char* szText)
{
LOG_FUN_CALL
	const int c_BuffSize = 128;
	char szBuff[c_BuffSize];
	int nL = strlen(szText);
	assert(nL < c_BuffSize);
	strNCpy(szBuff, szText, c_BuffSize);
	const int c_RetBuffSize = 3;
	char* retA[c_RetBuffSize];
	int nR = strR(szBuff, '=', retA, c_RetBuffSize);
	assert(2==nR);
	if(2 != nR)
	{
		return 1;
	}
	if(0 == strcmp(retA[0], "listen"))
	{
		nR =  procListen(retA[1]);
		if(0 != nR)
		{
			return 2;
		}
	}
	if(0 == strcmp(retA[0], "connect"))
	{
		nR =  procConnect(retA[1]);
		if(0 != nR)
		{
			return 3;
		}
	}	
	
	return 0;
}



int libeventNet::init(const createSessionServerData& initData)
{
	if (initData.m_listenerNum > c_MaxListenNum || initData.m_connectorNum > c_MaxConnectorNum) {
		return 1;
	}
	m_listenSpace.init (c_MaxListenNum);
	m_pBase = event_base_new();
	for (udword i = 0; i < initData.m_listenerNum; i++) {
		auto pL = m_listenSpace.newT();
		auto& data = initData.m_listenEndPointS[i];
		pL->init (data.ip, data.port, data.token);
	}
	m_conSpace.init (c_MaxConnectorNum);
	for (udword i = 0; i < initData.m_connectorNum; i++) {
		auto pC = m_conSpace.newT();
		auto& data = initData.m_connectorEndPointS[i];
		pC->init (data.ip, data.port, data.token);
	}
	return 0;
}

static const udword  c_MaxNetNum = 4;
static uword	     g_curNetNum = 0;
libeventNet g_net[c_MaxNetNum];
int createSessionServer(const createSessionServerData& initData, udword& serverId)
{
	
}
