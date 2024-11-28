#ifndef _comTcpNet_h__
#define _comTcpNet_h__
#include <memory>
#include "packet.h"
#include "ISession.h"
#include "ITcpServer.h"
#include "mainLoop.h"
#include <map>
extern "C"
{
int initComTcpNet (const char* szDllFile, allocPackFT  allocPackFun,
		freePackFT	freePackFun, logMsgFT logMsgFun);
void cleanComTcpNet ();
}
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
	virtual	void onAcceptSession(ISession* session, void* userData);
	virtual	void onConnect(ISession* session, void* userData);
	virtual	void onClose(ISession* session);
	virtual	void onWritePack(ISession* session, packetHead* pack);
	virtual	ISession* onRecHeadIsNeetForward(ISession* session, netPacketHead* pN);

	ITcpServer*  tcpServer ();
	void  setTcpServer (ITcpServer* v);
	ISession* getSession (SessionIDType id);
	// delTcpServerFT  delTcpServerFun ();
	// typedef ubyte usrDataValue[32];
	struct usrDataValue
	{
		uqword val[4];
	};
	using usrDataType = std::pair<comTcpNet*, usrDataValue>;
private:
	std::unique_ptr<usrDataType[]>   m_usrDataS;
	ITcpServer*  m_tcpServer;
};
#endif
