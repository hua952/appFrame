#ifndef _libevent_app_h_
#define _libevent_app_h_
#include "typeDef.h"
#include "Session.h"
#include "libeventConnector.h"
#include "libevListener.h"
#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "tJumpListNS.h"
#include <unordered_map.h>
#include "tSpace.h"

class libeventNet
{
public:
	libeventNet();
	~libeventNet();

	int onFrame();
	int init(const createSessionServerData& initData);
	virtual int connect(const char* szIP, udword udwPort, udword udwToken, const char* szPmpName, const char* szName);
	virtual int listen(uqword& handle, udword udwToken, const char* szIP, udword udwPort, udword udwQuLen, udword ForwebShowck, const char* szName);
	struct event_base* base();
	int connectGetSession(const char* szIP, udword udwPort, libeventConnector* pC);
	static const int c_MaxListenerNum = 8;
	static const int c_MaxConnectorNum = 1024;
private:
	int procCmd1(char* szText);
	int procCmd2(char* szText);
	int procConnect(char* szEndPoint);
	int procListen(char* szEndPoint);
	struct event_base* m_pBase;
	struct event *			m_pTimeout;
	struct event *			m_pSigInt;
	struct event *			m_pSigUser1;
	typedef tSpace<libeventConnector, uword>  connectorSpace;
	connectorSpace		       m_conSpace;
	typedef tSpace<libevListener, ubyte>  listenSpace;
	listenSpace		m_listenSpace;

};


#endif

