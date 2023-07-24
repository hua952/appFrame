#include "msgFileMgr.h"
#include "strFun.h"

msgFileMgr:: msgFileMgr ()
{
}

msgFileMgr:: ~msgFileMgr ()
{
}

msgFileMgr::msgMap&  msgFileMgr:: msgS ()
{
    return m_msgS;
}

msgFile*   msgFileMgr:: findMsg (const char* szName)
{
    msgFile*   nRet = nullptr;
    do {
		if (!szName) {
			break;
		}
		auto& rMap = msgS ();
		auto it = rMap.find (szName);
		if (rMap.end () != it) {
			nRet = it->second.get ();
		}
    } while (0);
    return nRet;
}

