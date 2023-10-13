#include "cArgMgr.h"
#include "strFun.h"

cArgMgr:: cArgMgr ()
{
	m_logLevel = 1;
	m_detachServerS = false;
	strCpy("appFrameLog.log", m_logFile);
}

cArgMgr:: ~cArgMgr ()
{
}

void  cArgMgr:: onCmpKey (char* argv[])
{
    do {
		procUwordA("logLevel", argv, m_logLevel);
		procStrA ("logFile", argv, m_logFile);
		procBoolA("detachServerS", argv, m_detachServerS);
		procStrA("workDir", argv, m_workDir);
    } while (0);
}

const char*  cArgMgr:: workDir ()
{
    return m_workDir.get ();
}

void  cArgMgr:: setWorkDir (const char* v)
{
    strCpy (v, m_workDir);
}

const char*  cArgMgr:: logFile ()
{
    return m_logFile.get ();
}

void  cArgMgr:: setLogFile (const char* v)
{
    strCpy (v, m_logFile);
}

uword  cArgMgr:: logLevel ()
{
    return m_logLevel;
}

void  cArgMgr:: setLogLevel (uword v)
{
    m_logLevel = v;
}

bool  cArgMgr:: detachServerS ()
{
    return m_detachServerS;
}

void  cArgMgr:: setDetachServerS (bool v)
{
    m_detachServerS = v;
}

