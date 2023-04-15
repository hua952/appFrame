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

