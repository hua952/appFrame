#ifndef _internalMsgId_h
#define _internalMsgId_h

enum comMsgMsgId
{
    internalMsgId_sendPackToRemoteAsk = 0,
    internalMsgId_sendPackToRemoteRet = 1,
    
    internalMsgId_recRemotePackForYouAsk = 2,

    internalMsgId_regAppRouteAsk = 3,
    internalMsgId_regAppRouteRet = 4,

    internalMsgId_createChannelAsk = 5,
    internalMsgId_createChannelRet = 6,

    internalMsgId_deleteChannelAsk = 7,
    internalMsgId_deleteChannelRet = 8,

    internalMsgId_subscribeChannelAsk = 9,
    internalMsgId_subscribeChannelRet = 9,

    internalMsgId_sayToChannelAsk = 11,
    internalMsgId_sayToChannelRet = 12,

    internalMsgId_leaveChannelAsk = 13,
    internalMsgId_leaveChannelRet = 14,

    internalMsgId_broadcastPacketNtf = 15,

    internalMsgId_heartbeatAsk = 16,
    internalMsgId_heartbeatRet = 17,
};
#endif
