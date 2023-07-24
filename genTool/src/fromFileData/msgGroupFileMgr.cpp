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

msgGroupFile*  msgGroupFileMgr:: findGroup (const char* pGName)
{
    msgGroupFile*  nRet = nullptr;
    do {
		auto& rMap =  msgGroupS ();
		auto it = rMap.find (pGName);
		if (rMap.end () != it) {
			nRet = it->second.get ();
		}
    } while (0);
    return nRet;
}

