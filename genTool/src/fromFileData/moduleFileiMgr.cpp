#include "moduleFileiMgr.h"
#include "strFun.h"
#include "moduleFile.h"

moduleFileiMgr:: moduleFileiMgr ()
{
}

moduleFileiMgr:: ~moduleFileiMgr ()
{
}

moduleFileiMgr::moduleMap&  moduleFileiMgr:: moduleS ()
{
    return m_moduleS;
}

moduleFile*   moduleFileiMgr:: findModule (const char* szName)
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

