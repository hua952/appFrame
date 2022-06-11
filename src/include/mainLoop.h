#ifndef _mainLoop_h__
#define _mainLoop_h__
#include "loop.h"

#define LoopNumBitLen 3
#define ProcNumBitLen 3
#define GroupNumBitLen 2 
#define LoopNum			(1<<LoopNumBitLen)
#define ProcNum			(1<<ProcNumBitLen)
#define GroupNum		(1<<GroupNumBitLen)

typedef int (*sendPackToLoopFT)(netPacketHead*, uword);
typedef void (*stopLoopSFT)();
typedef packetHead* (*allocPackFT)(udword udwSize);
typedef void		(*freePackFT)(packetHead* pack);
typedef struct _PhyCallback
{
	sendPackToLoopFT fnSendPackToLoop;
	stopLoopSFT      fnStopLoopS;
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
} PhyCallback;

typedef loopHandleType (*createLoopFT)(const char* szName, frameFunType funFrame, void* arg);

typedef  int (*regMsgFT)(loopHandleType handle, uword uwMsgId, procPacketFunType pFun); // call by level 2
typedef  int (*removeMsgFT)(loopHandleType handle, uword uwMsgId); // call by level 2

typedef struct _ForLogicFun
{
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	createLoopFT	 fnCreateLoop;
	regMsgFT		 fnRegMsg;
	removeMsgFT		 fnRemoveMsg;
} ForLogicFun;

typedef struct _ForMsgModuleFunS
{
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
} ForMsgModuleFunS;

extern "C"
{
	int InitMidFrame(int nArgC, const char* argS[], PhyCallback* pCallbackS); // call by level 0
	int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum); // call by level 0
	//void loopStartResult(loopHandleType pLoop, int res, ModelIDType id); // call by level 0
}

#endif
