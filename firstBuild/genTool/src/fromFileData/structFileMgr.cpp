#include "structFileMgr.h"
#include "strFun.h"
#include "structFile.h"

structFileMgr:: structFileMgr ()
{
}

structFileMgr:: ~structFileMgr ()
{
}

structFileMgr::structMap&  structFileMgr:: structS ()
{
    return m_structS;
}

structFileMgr::structVec&  structFileMgr:: structOrder ()
{
    return m_structOrder;
}

