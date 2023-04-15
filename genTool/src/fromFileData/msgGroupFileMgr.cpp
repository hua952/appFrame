#include "msgGroupFileMgr.h"
#include "strFun.h"
#include "msgGroupFile.h"

msgGroupFileMgr:: msgGroupFileMgr ()
{
}

msgGroupFileMgr:: ~msgGroupFileMgr ()
{
}

msgGroupFileMgr::msgGroupMap&  msgGroupFileMgr:: msgGroupS ()
{
    return m_msgGroupS;
}

