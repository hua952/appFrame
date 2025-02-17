#ifndef _mainLoop_h__
#define _mainLoop_h__
#include "loop.h"
#include "comFun.h"
#include "ISession.h"

#define LoopNumBitLen 8
#define ProcNumBitLen (sizeof(loopHandleType)*8-LoopNumBitLen)
#define LoopNum			(1<<LoopNumBitLen)
#define LoopMark		(LoopNum-1)
#define AllGateLoopId  (LoopMark-1)
#define ProcNum			(1<<ProcNumBitLen)
#define ProcMark		((ProcNum-1))

#define serverNameSize  32
#define getProcFromHandle(h) ((h>>LoopNumBitLen)&ProcMark)
#define netPackInOnceProc(n) (getProcFromHandle(n->ubySrcServId)==getProcFromHandle(n->ubyDesServId))
#define packInOnceProc(p) (netPackInOnceProc(P2NHead(p)))


#define c_serverLevelNum	4
extern ServerIDType		c_levelMaxOpenNum[c_serverLevelNum];
#define c_onceServerLevelNum  (256/c_serverLevelNum)

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

struct  serverNode;
typedef  int (*regMsgFT)(loopHandleType serverId, uword uwMsgId, procRpcPacketFunType pFun); // call by level 2
typedef  procRpcPacketFunType (*findMsgFT)(loopHandleType serverId, uword uwMsgId); // call by level 2
typedef  int (*removeMsgFT)(loopHandleType handle, uword uwMsgId); // call by level 2
typedef  void (*popFromCallStackFT) (loopHandleType handle);

typedef int (*pushPackToServerFT)(loopHandleType desServerId, packetHead* pack);
typedef void (*setAttrFT)(loopHandleType desServerId, const char* szTxt);

typedef struct _ForLogicFun
{
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	regMsgFT		 fnRegMsg;
	findMsgFT        fnFindMsg;
	logMsgFT		 fnLogMsg;
    addComTimerFT    fnAddComTimer;// Thread safety
	nextTokenFT      fnNextToken;
	setAttrFT        fnSetAttr;
	pushPackToServerFT     fnPushPackToServer;
} ForLogicFun;

typedef dword (*afterLoadFunT)(int nArgC, char** argS, ForLogicFun* pForLogic, int nDefArgC, char** defArgS, char* taskBuf, int taskBufSize);
typedef void (*beforeUnloadFT)();
typedef void (*logicOnAcceptFT)(serverIdType	fId, SessionIDType sessionId, uqword userData);
typedef void (*logicOnConnectFT)(serverIdType fId, SessionIDType sessionId, uqword userData);
typedef int (*onLoopBeginFT)(serverIdType	fId);
typedef int (*onFrameLagicFT)(serverIdType	fId);
typedef void (*onLoopEndFT)(serverIdType	fId);
typedef int (*onRecPacketFT)(serverIdType	fId, packetHead* pack);

enum appNetType
{
	appNetType_client,
	appNetType_gate,
	appNetType_server,
};

#define c_onceServerMaxConnectNum    6
#define c_onceServerMaxListenNum    3
struct serverEndPointInfo
{
	char              ip[16];
	uqword            userData;
	uqword            logicData;
	uword             port;
	ServerIDType	  targetHandle; // use to reg route use on onConnect
	udword            udwUnUse;
	ubyte			  unUse;  // 
	bool              bDef;  // def route
	bool			  rearEnd; 
	bool              regRoute;
};

struct serverNode
{
	udword                  sleepSetp;
	udword                  fpsSetp;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
	bool                    autoRun;
	bool                    route;
	ubyte                   unUse [2];
	serverEndPointInfo		listenEndpoint [c_onceServerMaxListenNum];
	serverEndPointInfo		connectEndpoint [c_onceServerMaxConnectNum];
};

extern "C"
{
}
#endif
