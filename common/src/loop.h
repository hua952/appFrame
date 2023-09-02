#ifndef _loop_h_
#define _loop_h_
#include "packet.h"
#include "comFun.h"

enum procPacketFunRetType
{
	procPacketFunRetType_del,
	procPacketFunRetType_doNotDel,
	procPacketFunRetType_exitNow,
	procPacketFunRetType_exitAfterLoop
};

typedef struct _procPacketArg
{
	loopHandleType handle;
} procPacketArg;

typedef int (*frameFunType)(void* arg);
//typedef void* loopHandleType;
typedef int (*procPacketFunType)(packetHead*, procPacketArg*);
typedef int (*procRpcPacketFunType)(packetHead*,  pPacketHead&, procPacketArg*);
typedef loopHandleType  ServerIDType;
typedef uword MsgIdT;
#define c_emptyModelId 255
#define c_emptyLoopHandle	0xff

extern "C"
{
	//int regMsg(loopHandleType handle, uword uwMsgId, procPacketFunType pFun); // call by level 2
	//int removeMsg(loopHandleType handle, uword uwMsgId); // call by level 2
	//procPacketFunType findMsg(loopHandleType handle, uword uwMsgId);

	int onMidLoopBegin(loopHandleType pThis); // call by level 0
	int onMidLoopEnd(loopHandleType pThis); // call by level 0
	int onLoopFrame(loopHandleType pThis); // call by level 0
	int processOncePack(loopHandleType pThis, packetHead* pPack);// call by level 0
	// int onWriteOncePack(loopHandleType pThis, packetHead* pPack);// call by level 0
	// void onFreePack(loopHandleType pThis, packetHead* pPack);// call by level 0
	//int loadLogicModle(char* szPath);
	//int unLoadLogicModle(char* szPath);
}

#endif
