#ifndef _loop_h_
#define _loop_h_
#include "packet.h"
#include "comFun.h"

enum procPacketFunRetType
{
	procPacketFunRetType_del = 0,
	procPacketFunRetType_doNotDel = 1,
	procPacketFunRetType_stopBroadcast = 2,
	procPacketFunRetType_exitNow = 4,
	procPacketFunRetType_exitAfterLoop = 8,
	// procPacketFunRetType_alderHandle = 16
};

typedef struct _procPacketArg
{
	loopHandleType handle;
	bool           broadcast{false};
} procPacketArg;

typedef int (*frameFunType)(void* arg);
//typedef void* loopHandleType;
typedef int (*procPacketFunType)(packetHead*, procPacketArg*);
typedef int (*serializePackFunType) (netPacketHead* pN, pPacketHead& pNew);
typedef serializePackFunType* pSerializePackFunType;
typedef int (*procRpcPacketFunType)(pPacketHead,  pPacketHead&, procPacketArg*);
typedef loopHandleType  ServerIDType;
typedef uword MsgIdT;
// #define c_emptyModelId 255

#define c_emptyLoopHandle	((1<<(sizeof(serverIdType)*8))-1)
#define c_emptyLocalServerId	(c_emptyLoopHandle&LoopMark)
#define c_sinServerIdBegin  240
#define c_onceMutServerNum  16
#define c_maxMutServerNum  (c_sinServerIdBegin/c_onceMutServerNum)
#define c_maxSinServerNum  15

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
