#include "myLoggerMgr.h"
#include "strFun.h"

myLoggerMgr:: myLoggerMgr ()
{
}

myLoggerMgr:: ~myLoggerMgr ()
{
}
/*
myLoggerMgr::loggerMap&  myLoggerMgr:: loggerS ()
{
    return m_loggerS;
}
*/
myLogger*    myLoggerMgr:: findLogger (const char* szName)
{
	/*
    myLogger*    nRet = nullptr;
    do {
		auto& rMap = loggerS ();
		auto it = rMap.find (szName);
		if (rMap.end() != it) {
			nRet = it->second.get();
		}
    } while (0);
    return nRet;
	*/
	return m_myLogger.get();
}

int myLoggerMgr:: addLogger  (const char* szName, std::shared_ptr<myLogger> pLog)
{
    int nRet = 0;
    do {
		/*
		auto& rMap = loggerS ();
		auto inRet = rMap.insert (std::make_pair(szName, pLog));
		if (!inRet.second) {
			nRet = 1;
		}
		*/
    } while (0);
    return nRet;
}

