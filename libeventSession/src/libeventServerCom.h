#ifndef  libeventServerCom_h_
#include "packet.h"
#include "comFun.h"
#include "ITcpServer.h"
#include "libeventSessionCom.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <signal.h>
#include "mainLoop.h"
#include "cTimer.h"

int initGlobal (allocPackFT  allocPackFun, freePackFT	freePackFun, logMsgFT logMsgFun);
allocPackFT    allocPackFun ();
freePackFT    freePackFun ();
logMsgFT logMsgFun ();

struct event_base;
class libeventListener;
class libeventConnector;
class libeventServerSession;
class libeventSessionCom;
class libeventServerCom: public ITcpServer
{
public:
    libeventServerCom ();
    ~libeventServerCom ();
	void  clean();
	typedef std::unordered_map<SessionIDType, libeventSessionCom*> sessonMap;
	typedef std::unordered_map<SessionIDType, std::shared_ptr<libeventServerSession>> serverSessonMap;
	typedef std::map<SessionIDType, std::shared_ptr<libeventConnector>>  connectMap;
	int onLoopFrame () override;
	void*     userData() override;
	void      setUserData (void* pData) override;
	sessonMap&  getSessonMap();
	serverSessonMap&  getServerSessonMap ();
	struct event_base* getBase();
	libeventSessionCom* getLibeventSessionCom (SessionIDType id);
	ISession* getSession (SessionIDType id) override;
	int    init (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
			endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum);
    SessionIDType nextSessionId ();
	int closeSession (SessionIDType id);
    onAcceptSessionT    acceptFun ();
    void  setAcceptFun (onAcceptSessionT va);
    onCloseT    closeFun ();
    void  setCloseFun (onCloseT va);
    onConnectT    connectFun ();
    void  setConnectFun (onConnectT va);
    onProcPackT procPackfun ();
    connectMap&    getConnectMap ();
	cTimerMgr&     getTimerMgr ();
	static const  int c_waitConTime = 1000; // ms
    onWritePackT    onWritePackFun ();
    void  setOnWritePackFun (onWritePackT va);
private:
    onWritePackT  m_onWritePackFun;
	cTimerMgr   m_timerMgr;
    connectMap  m_connectMap;
    onCloseT  m_closeFun;
    onAcceptSessionT  m_acceptFun;
    onConnectT  m_connectFun;
    onProcPackT m_procPackfun;
    SessionIDType  m_nextSession;
	sessonMap m_sessonMap;
	serverSessonMap     m_serverSessonMap;
	struct event_base* m_base;
	void*				m_userData;
	//netMsgFunMap   m_netMsgFunMap;
	std::unique_ptr<libeventListener[]> m_listerS;
	//std::unique_ptr<libeventConnector[]> m_connectorS;
	std::unique_ptr<sigInfo[]>  m_sigInfo;
	udword    m_listerNum;
	//udword    m_connectorNum;
	udword    m_sigInfoNum;
};
#endif
