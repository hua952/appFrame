#include "msgGroupFile.h"
#include "strFun.h"

msgGroupFile:: msgGroupFile ()
{
}

msgGroupFile:: ~msgGroupFile ()
{
}

const char*  msgGroupFile:: msgGroupName ()
{
    return m_msgGroupName.get ();
}

void  msgGroupFile:: setMsgGroupName (const char* v)
{
    strCpy (v, m_msgGroupName);
}

msgGroupFile::rpcNameSet&  msgGroupFile:: rpcNameS ()
{
    return m_rpcNameS;
}

const char*  msgGroupFile:: fullChangName ()
{
    return m_fullChangName.get ();
}

void  msgGroupFile:: setFullChangName (const char* v)
{
    strCpy (v, m_fullChangName);
}

const char*  msgGroupFile:: rpcSrcFileName ()
{
    return m_rpcSrcFileName.get ();
}

void  msgGroupFile:: setRpcSrcFileName (const char* v)
{
    strCpy (v, m_rpcSrcFileName);
}

