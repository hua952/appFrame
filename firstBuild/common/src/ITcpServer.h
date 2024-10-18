#ifndef ITcpServer_h__
#define ITcpServer_h__
#include "typeDef.h"
#include "packet.h"
#include "ISession.h"

struct endPoint
{
	// uqword userLogicData[4];	
	char ip[16];
	void* userData;	
	ISession**   ppIConnector; // when connect rec the connector if no neet set it null
	udword   userDataLen;
	uword    port;
	uword    unUse;
};

typedef void (*onAcceptSessionT)(ISession* session, void* userData);
typedef void (*onConnectT)(ISession* session, void* userData);
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

// typedef void (*event_callback_FN)(intptr_t, short, void *);
typedef void (*event_callback_FN)(/*intptr_t, */short, void *);
struct sigInfo
{
	int first;
	event_callback_FN second;
	void* pUserData;
};


typedef ISession* (*onRecHeadIsNeetForwardFT) (netPacketHead* pHead);
class ITcpServer
{
public:
	virtual int onLoopFrame () = 0;
	virtual ISession* getSession (SessionIDType id) = 0;
	virtual int       getAllConnector (ISession** ppRec, int recBuffNum) = 0;
	virtual void*     userData() = 0;
	virtual void      setUserData (void* pData, udword dataSize) = 0;
	virtual void      setOnRecHeadIsNeetForwardFun (onRecHeadIsNeetForwardFT fun) = 0;
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
