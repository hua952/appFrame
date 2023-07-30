#ifndef _msgMgr_h__
#define _msgMgr_h__
#include "packet.h"
#include <memory>
#include "arrayMap.h"
#include "iRpcInfoMgr.h"

class msgMgr:public iRpcInfoMgr // Thread safety
{
public:
	struct msgInfo
	{
		loopHandleType  defProcServer;
	};
	typedef msgInfo* pmsgInfo;
	typedef arrayMap<msgIdType, msgInfo>    msgInfoMap;
	typedef arrayMap<msgIdType, msgIdType>  msgIdMap;
    msgMgr ();
    ~msgMgr ();
	int    regRpc (msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer) override;
	int    regAskOnly (msgIdType askId, serverIdType	askDefProcSer) override;
	bool   isRetMsg (msgIdType msgId) override;
	msgIdType     getRetMsg(msgIdType    askMsg) override;
	msgIdType     getAskMsg(msgIdType    retMsg) override;
	serverIdType  getDefProcServerId (msgIdType msgId);
private:
	msgInfoMap				  m_msgInfoS;
	msgIdMap                  m_askMap;
	msgIdMap                  m_retMap;
};
#endif
