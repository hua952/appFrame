#ifndef ITcpServer_h__
#define ITcpServer_h__
#include "typeDef.h"
#include "packet.h"
#include "ISession.h"

typedef struct _endPoint
{
	// uqword userLogicData[4];	
	char ip[16];
	void* userData;	
	ISession**   ppIConnector; // when connect rec the connector if no neet set it null
	udword   userDataLen;
	uword    port;
	uword    unUse;
}endPoint;

typedef void (*onAcceptSessionT)(ISession* session, void* userData);
typedef void (*onConnectT)(ISession* session, void* userData);
typedef void (*onCloseT)(ISession* session);
typedef int (*onProcPackT)(ISession* session, packetHead* packet);
typedef void (*onWritePackT)(ISession* session, packetHead* packet);
typedef ISession* (*onRecHeadIsNeetForwardT)(ISession* session, netPacketHead* packet);

typedef struct _callbackS
{
	onProcPackT			 procPackfun;
	onAcceptSessionT     acceptFun;
	onConnectT			connectFun;
	onCloseT			closeFun;
	onWritePackT        onWritePackFun;
	onRecHeadIsNeetForwardT  onRecHeadIsNeetForwardFun;
}callbackS;

// typedef void (*event_callback_FN)(intptr_t, short, void *);
typedef void (*event_callback_FN)(/*intptr_t, */short, void *);
typedef struct _sigInfo
{
	int first;
	event_callback_FN second;
	void* pUserData;
}sigInfo;


class ITcpServer
{
public:
	virtual int onLoopFrame () = 0;
	virtual ISession* getSession (SessionIDType id) = 0;
	virtual int       getAllConnector (ISession** ppRec, int recBuffNum) = 0;
	virtual void*     userData() = 0;
	virtual void      setUserData (void* pData, udword dataSize) = 0;
	virtual const char*  getAttr(const char* key) = 0;
	virtual void      setAttr(const char* key, const char* value) = 0;
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
