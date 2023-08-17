#ifndef _comTcpNet_h__
#define _comTcpNet_h__
#include <memory>
#include "packet.h"
#include "ISession.h"
#include "ITcpServer.h"
#include "mainLoop.h"
#include <map>

int initComTcpNet (const char* szDllFile, allocPackFT  allocPackFun,
		freePackFT	freePackFun, logMsgFT logMsgFun);
void cleanComTcpNet ();
class comTcpNet
{
public:
    comTcpNet ();
    ~comTcpNet ();
	int  initNet (endPoint* pLister, udword listerNum,
			endPoint* pConnector, udword conNum,
			sigInfo* pInfo, udword sigNum);
	void cleanNet ();
	virtual	int processNetPackFun(ISession* session, packetHead* pack);
	virtual	void onAcceptSession(ISession* session, uqword userData);
	virtual	void onConnect(ISession* session, uqword userData);
	virtual	void onClose(ISession* session);
	virtual	void onWritePack(ISession* session, packetHead* pack);
/*
	virtual packetHead* allocPackFun (udword udwSize) = 0;
	virtual void		freePackFun (packetHead* pack) = 0;
	virtual int logMsgFun (const char* logName, const char* szMsg, uword wLevel);
*/
	ITcpServer*  tcpServer ();
	void  setTcpServer (ITcpServer* v);
	ISession* getSession (SessionIDType id);
	// delTcpServerFT  delTcpServerFun ();
	using usrDataType = std::pair<comTcpNet*, uqword>;
private:
	std::unique_ptr<usrDataType[]>   m_usrDataS;
	ITcpServer*  m_tcpServer;
};
#endif
