#include "moduleFileMgr.h"
#include "strFun.h"
#include "moduleFile.h"

moduleFileMgr:: moduleFileMgr ()
{
}

moduleFileMgr:: ~moduleFileMgr ()
{
}

moduleFileMgr::moduleMap&  moduleFileMgr:: moduleS ()
{
    return m_moduleS;
}

moduleFile*   moduleFileMgr:: findModule (const char* szName)
{
    moduleFile*   nRet = nullptr;
    do {
		auto& rMap = moduleS ();
		auto it = rMap.find (szName);
		if (rMap.end () == it) {
			break;
		}
		nRet = it->second.get ();
    } while (0);
    return nRet;
}

