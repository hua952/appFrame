#ifndef _msgMgr_h__
#define _msgMgr_h__
#include "packet.h"
#include <memory>
#include "arrayMap.h"

class msgMgr
{
public:
	struct msgInfo
	{
		loopHandleType  defProcServer;
	};
	typedef msgInfo* pmsgInfo;
	/*
	class msgInfoPtrCmp
	{
	public:
		bool operator () (const pmsgInfo& pa, const pmsgInfo& pb) const;
	}
	class msgInfoCmp
	{
	public:
		bool operator () (const msgInfo& ra, const msgInfo& rb) const;
	}
*/
	typedef arrayMap<msgIdType, msgInfo>    msgInfoMap;
	typedef arrayMap<msgIdType, msgIdType>  msgIdMap;
    msgMgr ();
    ~msgMgr ();
	int    regRpc (msgIdType askId, msgIdType retId, loopHandleType askDefProcSer, loopHandleType retDefProcSer);
	int    regAskOnly (msgIdType askId, loopHandleType askDefProcSer);
private:
	msgInfoMap				  m_msgInfoS;
	msgIdMap                  m_askMap;
	msgIdMap                  m_retMap;
};
#endif
