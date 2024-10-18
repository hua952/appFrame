#ifndef _internalMsgId_h
#define _internalMsgId_h

enum comMsgMsgId
{
    internalMsgId_sendPackToRemoteAsk = 0,
    internalMsgId_sendPackToRemoteRet = 1,
    
    internalMsgId_recRemotePackForYouAsk = 2,

    internalMsgId_regAppRouteAsk = 3,
    internalMsgId_regAppRouteRet = 4,
};
#endif
