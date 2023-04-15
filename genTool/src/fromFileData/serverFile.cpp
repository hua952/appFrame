#include "serverFile.h"
#include "strFun.h"

serverFile:: serverFile ()
{
}

serverFile:: ~serverFile ()
{
}

const char*  serverFile::serverName ()
{
    return m_serverName.get ();
}

void  serverFile::setServerName (const char* v)
{
    strCpy (v, m_serverName);
}

