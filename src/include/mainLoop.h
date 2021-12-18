#ifndef _mainLoop_h__
#define _mainLoop_h__
#include "loop.h"

typedef int (*sendPackToLoopFT)(packetHead*, uword);
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
/*
typedef struct _packListNode
{
	netPacketHead*	pPack;
	struct _packListNode* pNext;
} packListNode;
*/

extern "C"
{
	int onInit(int nArgC, char* argS[], PhyCallback* pCallbackS); // call by level 0
	int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum); // call by level 0
	void loopStartResult(loopHandleType pLoop, int res,ModelIDType id); // call by level 0
	loopHandleType createLoop(char* szName, frameFunType funFrame, void* arg); // call by level 2
}

#endif
