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

