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

int initGlobal (allocPackFT  allocPackFun, freePackFT	freePackFun, logMsgFT  argLogMsgFun);
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
	// typedef std::unordered_map<SessionIDType, libeventSessionCom*> sessonMap;
	typedef std::unordered_map<SessionIDType, std::shared_ptr<libeventServerSession>> serverSessonMap;
	typedef std::map<SessionIDType, std::shared_ptr<libeventConnector>>  connectMap;
	int onLoopFrame () override;
	void*     userData() override;
	void      setUserData (void* pData, udword dataSize) override;
	int       getAllConnector (ISession** ppRec, int recBuffNum) override;
	void      setAttr(const char* key, const char* value) override;
	const char*  getAttr(const char* key) override;
	// sessonMap&  getSessonMap();
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
	allocPackFT  allocPackFun ();
	freePackFT  freePackFun ();
	onRecHeadIsNeetForwardT  onRecHeadIsNeetForwardFun ();
private:
	onRecHeadIsNeetForwardT  m_onRecHeadIsNeetForwardFun;
    onWritePackT  m_onWritePackFun;
	cTimerMgr   m_timerMgr;
    connectMap  m_connectMap;
    onCloseT  m_closeFun;
    onAcceptSessionT  m_acceptFun;
    onConnectT  m_connectFun;
    onProcPackT m_procPackfun;
    SessionIDType  m_nextSession;
	// sessonMap m_sessonMap;
	serverSessonMap     m_serverSessonMap;
	struct event_base* m_base;
	std::unique_ptr <char[]> m_userData;
	std::unique_ptr<libeventListener[]> m_listerS;
	std::unique_ptr<sigInfo[]>  m_sigInfo;
	udword    m_listerNum;
	udword    m_sigInfoNum;
};
#endif
