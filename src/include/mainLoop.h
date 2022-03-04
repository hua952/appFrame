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
	allocPackFT		 fnAllocPack;
	freePackFT		 fnFreePack;
} PhyCallback;

typedef loopHandleType (*createLoopFT)(char* szName, frameFunType funFrame, void* arg);
typedef struct _ForLogicFun
{
	allocPackFT		 fnAllocPack;
	freePackFT		 fnFreePack;
	createLoopFT	 fnCreateLoop;
} ForLogicFun;

extern "C"
{
	int onInit(int nArgC, char* argS[], PhyCallback* pCallbackS); // call by level 0
	int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum); // call by level 0
	//void loopStartResult(loopHandleType pLoop, int res, ModelIDType id); // call by level 0
}

#endif
