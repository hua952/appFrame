#ifndef _internalRpc_h
#define _internalRpc_h

#include "msg.h"

#define DEC_MSG(msgName) class msgName##Msg:public CMsgBase {\
public: \
	msgName##Msg();\
	msgName##Msg(packetHead* p);\
};

struct sendPackToRemoteAsk
{
	udword    objSessionId;
};
DEC_MSG(sendPackToRemoteAsk)
struct sendPackToRemoteRet
{
    udword    m_result;
};
DEC_MSG(sendPackToRemoteRet)

DEC_MSG(recRemotePackForYouAsk)

DEC_MSG(heartbeatAsk)
DEC_MSG(heartbeatRet)

struct regAppRouteAsk 
{
    ubyte m_appGrupId;
};
DEC_MSG(regAppRouteAsk)
struct regAppRouteRet
{
    udword    m_result;
};
DEC_MSG(regAppRouteRet)

struct createChannelAsk
{
	char channel[16];
	ubyte  m_sendToMe;
};
DEC_MSG(createChannelAsk)
struct createChannelRet
{
    udword    m_result;
	uword     m_gateIndex;
};
DEC_MSG(createChannelRet)


struct deleteChannelAsk
{
	char channel[16];
};
DEC_MSG(deleteChannelAsk)
struct deleteChannelRet
{
    udword    m_result;
	uword     m_gateIndex;
};
DEC_MSG(deleteChannelRet)

struct subscribeChannelAsk
{
	char channel[16];
};
DEC_MSG(subscribeChannelAsk)
struct subscribeChannelRet
{
    udword    m_result;
};
DEC_MSG(subscribeChannelRet)

struct sayToChannelAsk
{
	char   channel[16];
	udword  packSize;
	char   unUse[4];
	char   pack[1];
};
DEC_MSG(sayToChannelAsk)
struct sayToChannelRet
{
    udword    m_result;
	uword     m_gateIndex;
};
DEC_MSG(sayToChannelRet)

struct leaveChannelAsk
{
	char channel[16];
};
DEC_MSG(leaveChannelAsk)
struct leaveChannelRet
{
    udword    m_result;
};
DEC_MSG(leaveChannelRet)

struct broadcastPacketNtf
{
	uqword retPack;
	udword sessionId;
	ubyte  srcServer;
};
DEC_MSG(broadcastPacketNtf)


DEC_MSG(sendToAllGateAsk)
struct sendToAllGateRet
{
    udword    m_result;
};
DEC_MSG(sendToAllGateRet)

struct exitAppNtfByNet
{
	ubyte exitType;
	uword resultNum;
	char  result[128];
};
DEC_MSG(exitAppNtfByNet)

struct exitAppNtf
{
	ubyte exitType;
};
DEC_MSG(exitAppNtf)
#endif


