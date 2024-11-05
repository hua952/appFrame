#ifndef _myLoggerMgr_h__
#define _myLoggerMgr_h__
#include <memory>
#include <unordered_map>
#include <string>
#include "myLogger.h"

class myLoggerMgr
{
public:
    myLoggerMgr ();
    ~myLoggerMgr ();
	// using loggerMap = std::unordered_map<std::string, std::shared_ptr<myLogger>>;
	// loggerMap&  loggerS ();
	myLogger*   findLogger (const char* szName);
	int         addLogger  (const char* szName, std::shared_ptr<myLogger> pLog);

	std::unique_ptr<myLogger>	m_myLogger;
private:
	// loggerMap  m_loggerS;
};
#endif
