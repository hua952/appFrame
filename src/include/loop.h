#ifndef _loop_h_
#define _loop_h_
#include "packet.h"

typedef int (*frameFunType)(void* arg);
typedef void* loopHandleType;
typedef int (*procPacketFunType)(packetHead*);
typedef ubyte ModelIDType;

#define c_emptyModelId 255

extern "C"
{
	//int regMsg(loopHandleType handle, uword uwMsgId, procPacketFunType pFun); // call by level 2
	//int removeMsg(loopHandleType handle, uword uwMsgId); // call by level 2
	//procPacketFunType findMsg(loopHandleType handle, uword uwMsgId);

	int OnLoopFrame(loopHandleType* pThis); // call by level 0
	int processOncePack(loopHandleType* pThis, packetHead* pPack);// call by level 0

	//int loadLogicModle(char* szPath);
	//int unLoadLogicModle(char* szPath);
}

#endif
