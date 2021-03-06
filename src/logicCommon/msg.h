#ifndef _Msg_h___
#define _Msg_h___
#include"packet.h"
#include"mainLoop.h"

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

extern "C"
{
	ForMsgModuleFunS& getForMsgModuleFunS();
	packetHead* allocPacket(udword udwS);
	void releasePack(packetHead* p);
}

#endif
