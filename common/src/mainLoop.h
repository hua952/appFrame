#ifndef _mainLoop_h__
#define _mainLoop_h__
#include "loop.h"
#include "comFun.h"
#include "iRpcInfoMgr.h"
#include "ISession.h"

#define LoopNumBitLen 4
#define ProcNumBitLen 3
#define GroupNumBitLen 1 
#define LoopNum			(1<<LoopNumBitLen)
#define LoopMark		(LoopNum-1)
#define ProcNum			(1<<ProcNumBitLen)
//#define ProcMark		((ProcNum-1)<<LoopNum)
#define ProcMark		((ProcNum-1))
#define GroupNum		(1<<GroupNumBitLen)
#define GroupMark		((GroupNum-1)<<(LoopNumBitLen+ProcNumBitLen))

#define serverNameSize  32
#define getProcFromHandle(h) ((h>>LoopNumBitLen)&ProcMark)
#define netPackInOnceProc(n) (getProcFromHandle(n->ubySrcServId)==getProcFromHandle(n->ubyDesServId))
#define packInOnceProc(p) (netPackInOnceProc(P2NHead(p)))

typedef int (*sendPackToLoopFT)(packetHead*);
typedef int (*pushPackToLoopFT)(loopHandleType pThis, packetHead*);
typedef void (*stopLoopSFT)();
typedef packetHead* (*allocPackFT)(udword udwSize);
typedef void		(*freePackFT)(packetHead* pack);
typedef int (*logMsgFT) (const char* logName, const char* szMsg, uword wLevel);

typedef int (*addComTimerFT)(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen);
typedef NetTokenType   (*nextTokenFT)(loopHandleType pThis);
typedef loopHandleType      (*getCurServerHandleFT) ();
typedef void (*pushToCallStackFT)(loopHandleType pThis, const char* szTxt);
typedef void (*popFromCallStackFT)(loopHandleType pThis);
typedef void (*logCallStackFT) (loopHandleType pThis, int nL);

typedef struct _PhyCallback
{
	// sendPackToLoopFT fnSendPackToLoop; // Thread safety
	pushPackToLoopFT fnPushPackToLoop; // Thread safety
	stopLoopSFT      fnStopLoopS;
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	logMsgFT		 fnLogMsg;// Thread safety
    //addComTimerFT    fnAddComTimer;// Thread safety
	nextTokenFT      fnNextToken;
	getCurServerHandleFT   fnGetCurServerHandle; // Thread safety
	pushToCallStackFT      fnPushToCallStack;
	popFromCallStackFT     fnPopFromCallStack;
	logCallStackFT         fnLogCallStack;
} PhyCallback;

struct  serverNode;
typedef int (*createLoopFT)(const char* szName, loopHandleType serId, serverNode* pNode, frameFunType funFrame, void* arg);
typedef  int (*regMsgFT)(loopHandleType serverId, uword uwMsgId, procRpcPacketFunType pFun); // call by level 2
typedef  int (*removeMsgFT)(loopHandleType handle, uword uwMsgId); // call by level 2
typedef  iRpcInfoMgr* (*getIRpcInfoMgrFT)();
typedef  void (*popFromCallStackFT) (loopHandleType handle);
typedef  int    (*regRpcFT) (msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer);
typedef  serverIdType (*getDefProcServerIdFT) (msgIdType msgId);

typedef struct _ForLogicFun
{
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	createLoopFT	 fnCreateLoop;
	regMsgFT		 fnRegMsg;
	sendPackToLoopFT fnSendPackToLoop;// Thread safety
	logMsgFT		 fnLogMsg;
    addComTimerFT    fnAddComTimer;// Thread safety
	nextTokenFT      fnNextToken;
	// getIRpcInfoMgrFT fnGetIRpcInfoMgr;
	pushToCallStackFT      fnPushToCallStack;
	popFromCallStackFT     fnPopFromCallStack;
	logCallStackFT         fnLogCallStack;
	regRpcFT               fnRegRpc;
    getDefProcServerIdFT   fnGetDefProcServerId;
} ForLogicFun;

typedef dword (*afterLoadFunT)(int nArgC, char** argS, ForLogicFun* pForLogic);
typedef void (*beforeUnloadFT)();
typedef void (*logicOnAcceptFT)(serverIdType	fId, SessionIDType sessionId, uqword userData);
typedef void (*logicOnConnectFT)(serverIdType fId, SessionIDType sessionId, uqword userData);
typedef void (*onLoopBeginFT)(serverIdType	fId);
typedef void (*onLoopEndFT)(serverIdType	fId);
/*
typedef struct _ForRegMsg{
	regMsgFT		 fnRegMsg;
} ForRegMsg;
*/
#define c_onceServerMaxConnectNum    6
#define c_onceServerMaxListenNum    3
struct serverEndPointInfo
{
	char              ip[16];
	// void*             userData;
	// udword            userDataLen;
	uqword            userData;
	uword             port;
	ServerIDType	  targetHandle; // use to reg route use on onConnect
	udword            udwUnUse;
	uword             unUse;  // 
	bool              bDef;  // def route
	bool              bRegHandle; // 几乎没用 
};
/*
typedef void (*logicOnAcceptSessionFT)(serverIdType fId, SessionIDType, uqword);
typedef void (*logicOnConnectFT)(serverIdType fId, SessionIDType, uqword);
*/
struct serverNode
{
	udword                  sleepSetp;
	udword                  fpsSetp;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
	bool                    autoRun;
	ubyte                   unUse[3];
	// logicOnAcceptSessionFT  fnOnAccept;
	// logicOnConnectFT        fnOnConnect;
	serverEndPointInfo		listenEndpoint [c_onceServerMaxListenNum];
	serverEndPointInfo		connectEndpoint [c_onceServerMaxConnectNum];
};

extern "C"
{
	int InitMidFrame(int nArgC, char** argS, PhyCallback* pCallbackS); // call by level 0
	int getAllLoopAndStart(serverNode* pBuff, int nBuffNum); // call by level 0
	//void loopStartResult(loopHandleType pLoop, int res, ServerIDType id); // call by level 0
}
#endif
