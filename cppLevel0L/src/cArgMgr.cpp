#include "cArgMgr.h"
#include "strFun.h"

cArgMgr:: cArgMgr ()
{
}

cArgMgr:: ~cArgMgr ()
{
}

void  cArgMgr:: onCmpKey (char* argv[])
{
    do {
		procUwordA("logLevel", argv, m_logLevel);
    } while (0);
}

uword  cArgMgr:: logLevel ()
{
    return m_logLevel;
}

void  cArgMgr:: setLogLevel (uword v)
{
    m_logLevel = v;
}

