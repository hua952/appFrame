#include "strFun.h"
#include "libeventServerCom.h"
#include "libeventSessionCom.h"
#include "libeventServerSession.h"
#include "libeventListener.h"
#include "libeventConnector.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "myAssert.h"
#include "loop.h"
#include "nLog.h"

static allocPackFT         g_allocPackFun = nullptr;
static freePackFT			g_freePackFun = nullptr;
static logMsgFT				g_logMsgFun = nullptr;
allocPackFT    allocPackFun (){
	return g_allocPackFun;
}

freePackFT    freePackFun () {
	return g_freePackFun;
}

logMsgFT logMsgFun () {
	return g_logMsgFun;
}

int initGlobal (allocPackFT  allocPackFun, freePackFT  freePackFun, logMsgFT logMsgFun)
{
	int nRet = 0;
	g_allocPackFun = allocPackFun;
	g_freePackFun = freePackFun;
	g_logMsgFun = logMsgFun;
	return nRet;
}

allocPackFT   libeventServerCom:: allocPackFun ()
{
	return g_allocPackFun;
}

freePackFT  libeventServerCom:: freePackFun ()
{
    return g_freePackFun;
}

libeventServerCom::libeventServerCom ()
{
	m_base = nullptr;
	m_listerNum = 0;
	//m_connectorNum = 0;
	m_sigInfoNum = 0;
	m_nextSession = 0;
	m_acceptFun = nullptr;
	m_closeFun = nullptr;
	m_connectFun = nullptr;
	m_userData = nullptr;
	/*
	m_allocPackFun = nullptr;
	m_freePackFun = nullptr;
	*/
}

libeventServerCom::~libeventServerCom ()
{
	myAssert (nullptr == m_base);
}

void libeventServerCom::clean ()
{
	if (m_base) {
		event_base_free(m_base);
		m_base = nullptr;
	}
}

int libeventServerCom::onLoopFrame ()
{
	int nRet = procPacketFunRetType_del;

	cTimerMgr&  rTMgr = getTimerMgr ();
	rTMgr.onFrame ();
	auto base = m_base;
	if (base) {
		event_base_loop (base, EVLOOP_NONBLOCK);
	}
	return nRet;
}

onProcPackT libeventServerCom::procPackfun()
{
    return m_procPackfun;
}

libeventServerCom::sessonMap&  libeventServerCom::getSessonMap()
{
	return  m_sessonMap;
}

struct event_base* libeventServerCom::getBase()
{
	return m_base;
}

libeventSessionCom* libeventServerCom::getLibeventSessionCom (SessionIDType id)
{
	libeventSessionCom* pRet = nullptr;
	auto& rM = getSessonMap();
	auto it = rM.find (id);
	if (rM.end () != it) {
		pRet = it->second;
	}
	return pRet;
}

ISession* libeventServerCom::getSession (SessionIDType id)
{
	return getLibeventSessionCom (id);
}

typedef std::pair<libeventServerCom*, SessionIDType> forConInfo;
static bool sConnectComTimerFun(void* p)
{
	bool bRet = false;
	auto& info = *((forConInfo*)(p));
	//nTrace (__FUNCTION__<<" info = "<<p);
	auto& rServer = *(info.first);
	auto& rConMap = rServer.getConnectMap ();
	auto it = rConMap.find (info.second);
	//nTrace (__FUNCTION__<<" 0000 ");
	if (rConMap.end () == it) {
		nTrace ("can'find connector");
	} else {
		auto pL = it->second.get();
		auto sta = pL->state ();
		//nTrace (__FUNCTION__<<" 1111 state = "<<sta);
		if (SessionState_waitCon == sta) {
			auto nRet = pL->connect();
			//nTrace (__FUNCTION__<<" 2222 nRet = "<< nRet);
			if (0 == nRet) {
				auto conFun = rServer.connectFun ();
				auto pU = pL->userData ();
				auto token = (uqword*)pU;
				auto sid = pL->id();
				pL->setState (SessionState_Online);
				//nTrace ("will call onConnect sid = "<<sid<<" token = "<<*token);
				conFun (pL, *token);
			} else {
				nRet = true;
			}
		} else {
			nTrace ("connect can not state error");
		}
	}
	//nTrace (__FUNCTION__<<" 5555");
	return bRet;
}

void* libeventServerCom::    userData()
{
	// return m_userData;
	return m_userData.get();
}

void  libeventServerCom::   setUserData (void* pData, udword dataSize)
{
	// m_userData = pData;
	m_userData = std::make_unique<char[]>(dataSize);
	memcpy(m_userData.get(), pData, dataSize);
}

int libeventServerCom::init (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
		endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum)
{
	nTrace (__FUNCTION__<<" 000");
	int nRet = 0;
	auto base = event_base_new();
	m_base = base;
	setAcceptFun (pCallbackS->acceptFun);
	setCloseFun (pCallbackS->closeFun);
	setConnectFun (pCallbackS->connectFun);
	setOnWritePackFun (pCallbackS->onWritePackFun);

	// m_freePackFun = pCallbackS->freePackFun ;
	// m_allocPackFun = pCallbackS->allocPackFun;
	//setAllocPackFun (pCallbackS->allocPackFun);
	//setFreePackFun (pCallbackS->freePackFun);
	m_procPackfun = pCallbackS->procPackfun;
	if (listerNum > 0) {
		//nTrace (__FUNCTION__<<" 111");
		m_listerS = std::make_unique<libeventListener[]> (listerNum);
		for (udword i = 0; i < listerNum; i++) {
			auto& ep = pLister[i];
			auto& rRL = m_listerS [i];
			rRL.init (this, ep.ip, ep.port);
			rRL.setUserData (ep.userData);
		}
	}
	if (conNum > 0) {
		//nTrace (__FUNCTION__<<" 222");
		auto& rConMap = getConnectMap ();
		for (udword i = 0; i < conNum; i++) {
			auto pCon = std::make_shared<libeventConnector> ();
			auto id = nextSessionId ();
			//auto& rS = pCon.get ();
			//rS.setId (id);
			auto& ep = pConnector[i];
			pCon->init (this, ep.ip, ep.port);
			pCon->setId(id);
			pCon->setState(SessionState_waitCon);
			pCon->setUserData (&ep.userData, sizeof (ep.userData));
			//auto nR = pCon->init (this, ep.ip, ep.port);
			//if (0 == nR) {
			//pCon->setIp (ep.ip);
			//pCon->setPort (ep.port);
			auto& rMap = getSessonMap ();
			auto& rConMap = getConnectMap ();
				//auto& rSC = pCon->sessionCom ();
				rMap[id] = pCon.get();// &rSC;
				rConMap [id] = pCon;
				forConInfo info;
				info.first = this;
				info.second = id;
				auto& rTimeMgr = getTimerMgr ();
				rTimeMgr.addComTimer (c_waitConTime, sConnectComTimerFun, &info, sizeof (info));
				/*
				auto fun = connectFun ();
				fun (pCon.get (), i);
				*/
			//}
		}
	}

	if (sigNum > 0) {
		m_sigInfo = std::make_unique<sigInfo[]> (sigNum);
		for (udword i = 0; i < sigNum; i++) {
			auto& rS = m_sigInfo[i];
			m_sigInfo[i] = rS;

			//创建信号事件
			struct event *signal_event = evsignal_new(base, rS.first, rS.second, NULL);
			event_add(signal_event, NULL);
		}
	}
	m_listerNum = listerNum;
	return nRet;
}

cTimerMgr&  libeventServerCom::   getTimerMgr ()
{
	return m_timerMgr;
}

libeventServerCom::serverSessonMap&   libeventServerCom::getServerSessonMap()
{
	return m_serverSessonMap;
}

int libeventServerCom::closeSession (SessionIDType   id)
{
	int nRet = 0;
	auto& rSM = getSessonMap();
	auto it = rSM.find(id);
	if (rSM.end() != it) {
		auto fun = closeFun();
		fun(it->second);
		rSM.erase(it);
	}
	auto& rServerMap = getServerSessonMap ();
	auto iter = rServerMap.find (id);
	if (rServerMap.end () == iter) {
		auto& rConMap = getConnectMap ();
		auto ite = rConMap.find (id);
		if (rConMap.end () == ite) {
			nWarn ("connector not find id = "<<id);
		} else {
			auto rSC = ite->second.get ();
			auto bev = rSC->getBev ();
			rSC->setBev (nullptr);
			bufferevent_free(bev);
			rConMap.erase (ite);
		}
	} else {
		auto& rSC = *(iter->second.get());
		auto bev = rSC.getBev ();
		rSC.setBev (nullptr);
		bufferevent_free(bev);
		rServerMap.erase (iter);	
	}
	return nRet;
}

SessionIDType  libeventServerCom::  nextSessionId ()
{
    return m_nextSession++;
}

onAcceptSessionT   libeventServerCom:: acceptFun ()
{
    return m_acceptFun;
}

void libeventServerCom:: setAcceptFun (onAcceptSessionT  va)
{
    m_acceptFun = va;
}

onCloseT  libeventServerCom::  closeFun ()
{
    return m_closeFun;
}

void libeventServerCom:: setCloseFun (onCloseT  va)
{
    m_closeFun = va;
}

onConnectT  libeventServerCom::  connectFun ()
{
    return m_connectFun;
}

void libeventServerCom:: setConnectFun (onConnectT  va)
{
    m_connectFun = va;
}
libeventServerCom::connectMap&  libeventServerCom::getConnectMap ()
{
    return m_connectMap;
}

onWritePackT  libeventServerCom::  onWritePackFun ()
{
    return m_onWritePackFun;
}

void  libeventServerCom::setOnWritePackFun (onWritePackT  va)
{
    m_onWritePackFun = va;
}

ITcpServer* createTcpServer (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
	endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum)
{
	auto pServer = new  libeventServerCom ();
	auto nRet = pServer->init (pCallbackS, pLister, listerNum, pConnector, conNum, pInfo, sigNum);
	if (nRet) {
		delete pServer;
		pServer = nullptr;
	}
	return pServer;
}

void  delTcpServer (ITcpServer* pServer)
{
	delete pServer;
}

