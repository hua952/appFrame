#include "rpcFileMgr.h"
#include "strFun.h"
#include "rpcFile.h"

rpcFileMgr:: rpcFileMgr ()
{
}

rpcFileMgr:: ~rpcFileMgr ()
{
}

rpcFileMgr::rpcMap&  rpcFileMgr:: rpcS ()
{
    return m_rpcS;
}

rpcFile*   rpcFileMgr:: findRpc (const char* szName)
{
    rpcFile*   nRet = nullptr;
    do {
		auto& rMap = rpcS ();
		auto it = rMap.find (szName);
		if (rMap.end () != it) {
			nRet = it->second.get ();
		}
    } while (0);
    return nRet;
}
/*
rpcFileMgr::rpcVec&  rpcFileMgr:: rpcOrder ()
{
    return m_rpcOrder;
}
*/
