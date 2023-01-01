#ifndef ITcpServer_h__
#define ITcpServer_h__
#include "typeDef.h"
#include "packet.h"
#include "ISession.h"

struct endPoint
{
	char ip[16];
	uqword   userData;	
	uword    port;
};

typedef void (*onAcceptSessionT)(ISession* session, uqword userData);
typedef void (*onConnectT)(ISession* session, uqword userData);
typedef void (*onCloseT)(ISession* session);
typedef int (*onProcPackT)(ISession* session, packetHead* packet);
typedef void (*onWritePackT)(ISession* session, packetHead* packet);

struct callbackS
{
	onProcPackT			 procPackfun;
	onAcceptSessionT     acceptFun;
	onConnectT			connectFun;
	onCloseT			closeFun;
	onWritePackT        onWritePackFun;
};

typedef void (*event_callback_fn)(intptr_t, short, void *);
struct sigInfo
{
	int first;
	event_callback_fn second;
};

class ITcpServer
{
public:
	/*
	virtual int init (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
			endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum) = 0;*/
	virtual int onLoopFrame () = 0;
	virtual ISession* getSession (SessionIDType id) = 0;
	virtual void*     userData() = 0;
	virtual void      setUserData (void* pData) = 0;
};

typedef ITcpServer* (*createTcpServerFT) (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
	endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum);
typedef void  (*delTcpServerFT) (ITcpServer* pServer);

extern "C"
{
ITcpServer* createTcpServer (callbackS* pCallbackS, endPoint* pLister, udword listerNum,
	endPoint* pConnector, udword conNum, sigInfo* pInfo, udword sigNum);
void  delTcpServer (ITcpServer* pServer);
}
#endif
