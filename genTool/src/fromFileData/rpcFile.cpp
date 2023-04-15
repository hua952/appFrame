#include "rpcFile.h"
#include "strFun.h"

rpcFile:: rpcFile ()
{
}

rpcFile:: ~rpcFile ()
{
}

const char*  rpcFile:: commit ()
{
    return m_commit.get ();
}

void  rpcFile:: setCommit (const char* v)
{
    strCpy (v, m_commit);
}

const char*  rpcFile:: rpcName ()
{
    return m_rpcName.get ();
}

void  rpcFile:: setRpcName (const char* v)
{
    strCpy (v, m_rpcName);
}

