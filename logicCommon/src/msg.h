#ifndef _Msg_h___
#define _Msg_h___
#include"packet.h"
#include"mainLoop.h"
#include "comFun.h"
/*
typedef struct _ForMsgModuleFunS
{
	sendPackToLoopFT fnSendPackToLoop;// Thread safety
	allocPackFT		 fnAllocPack; // Thread safety
	freePackFT		 fnFreePack; // Thread safety
	logMsgFT		 fnLogMsg;
	getIRpcInfoMgrFT fnGetIRpcInfoMgr;
} ForMsgModuleFunS;
*/

typedef ForLogicFun ForMsgModuleFunS;

enum MsgRpcType
{
    eMsgRpc_Ask,
    eMsgRpc_Ret,
	eMsgRpc_Unknow
};

class CMsgBase
{
public:
    CMsgBase(packetHead* p);
    CMsgBase();
    ~CMsgBase();
	virtual bool fromPack(packetHead* p);
    virtual packetHead*   toPack();
    packetHead*  pop();
	packetHead*  getPack();
protected:
    packetHead*  m_pPacket;
};

class CAskMsgBase:public CMsgBase
{
public:
    CAskMsgBase();
	virtual MsgRpcType RpcType();
};

class CRetMsgBase:public CMsgBase
{
public:
    CRetMsgBase();
	virtual MsgRpcType RpcType();
};

void              setForMsgModuleFunS (ForMsgModuleFunS* pFunS);
ForMsgModuleFunS& getForMsgModuleFunS();
extern "C"
{
	packetHead* allocPacket(udword udwS);
	packetHead* allocPacketExt(udword udwS, udword ExtNum);
	void releasePack(packetHead* p);
}

#endif
