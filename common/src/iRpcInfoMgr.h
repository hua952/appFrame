#ifndef iRpcInfoMgr_h__
#define iRpcInfoMgr_h__
#include "packet.h"

struct iRpcInfoMgr
{
	virtual int    regRpc (msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer) = 0;
	virtual int    regAskOnly (msgIdType askId, serverIdType	askDefProcSer) = 0;
	virtual bool   isRetMsg (msgIdType msgId) = 0;
	virtual msgIdType     getRetMsg(msgIdType    askMsg) = 0;
	virtual msgIdType     getAskMsg(msgIdType    retMsg) = 0;
};

#endif
