#ifndef _mainLoop_h__
#define _mainLoop_h__
#include "loop.h"
#include "comFun.h"
#include "iRpcInfoMgr.h"

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

typedef int (*sendPackToLoopFT)(packetHead*);
typedef void (*stopLoopSFT)();
typedef packetHead* (*allocPackFT)(udword udwSize);
typedef void		(*freePackFT)(packetHead* pack);
typedef int (*logMsgFT) (const char* logName, const char* szMsg, uword wLevel);

typedef int (*addComTimerFT)(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen);
typedef NetTokenType   (*nextTokenFT)(loopHandleType pThis);
typedef loopHandleType      (*getCurServerHandleFT) ();
typedef struct _PhyCallback
{
	sendPackToLoopFT fnSendPackToLoop;// Thread safety
	stopLoopSFT      fnStopLoopS;
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	logMsgFT		 fnLogMsg;// Thread safety
    //addComTimerFT    fnAddComTimer;// Thread safety
	nextTokenFT      fnNextToken;
	getCurServerHandleFT   fnGetCurServerHandle; // Thread safety
} PhyCallback;

struct  serverNode;
typedef int (*createLoopFT)(const char* szName, loopHandleType serId, serverNode* pNode, frameFunType funFrame, void* arg);
typedef  int (*regMsgFT)(loopHandleType serverId, uword uwMsgId, procRpcPacketFunType pFun); // call by level 2
typedef  int (*removeMsgFT)(loopHandleType handle, uword uwMsgId); // call by level 2
typedef  iRpcInfoMgr* (*getIRpcInfoMgrFT)();

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
	getIRpcInfoMgrFT fnGetIRpcInfoMgr;
} ForLogicFun;

typedef void (*afterLoadFunT)(ForLogicFun* pForLogic);
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
	uword             port;
	ServerIDType	  targetHandle;
	uword             unUse;
	bool              bDef;
	bool              bRegHandle;
};
struct serverNode
{
	udword                  udwUnuse;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
	ubyte	                ubyUnuse;
	serverEndPointInfo		listenEndpoint [c_onceServerMaxListenNum];
	serverEndPointInfo		connectEndpoint [c_onceServerMaxConnectNum];
};

extern "C"
{
	int InitMidFrame(int nArgC, const char* argS[], PhyCallback* pCallbackS); // call by level 0
	int getAllLoopAndStart(serverNode* pBuff, int nBuffNum); // call by level 0
	//void loopStartResult(loopHandleType pLoop, int res, ServerIDType id); // call by level 0
}
#endif
