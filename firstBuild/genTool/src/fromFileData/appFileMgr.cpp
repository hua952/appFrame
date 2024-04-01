#include "appFileMgr.h"
#include "appFile.h"
#include "strFun.h"

appFileMgr:: appFileMgr ()
{
}

appFileMgr:: ~appFileMgr ()
{
}

appFileMgr::appMap&  appFileMgr:: appS ()
{
    return m_appS;
}

appFile*  appFileMgr:: findApp (const char* szAppName)
{
    appFile*  nRet = nullptr;
    do {
		auto& rMgr = appS ();
		auto it = rMgr.find (szAppName);
		if (rMgr.end () != it) {
			nRet = it->second.get();
		}
    } while (0);
    return nRet;
}

