#include "configMgr.h"
#include "strFun.h"

configMgr:: configMgr ()
{
	m_reProc = false;
	m_installThreadDll = false;
	m_structBadyType = structBadyTime_com;
}

configMgr:: ~configMgr ()
{
}

const char*  configMgr:: defFile ()
{
    return m_defFile.get ();
}

void  configMgr:: setDefFile (const char* v)
{
    strCpy (v, m_defFile);
}

bool  configMgr:: reProc ()
{
    return m_reProc;
}

void  configMgr:: setReProc (bool v)
{
    m_reProc = v;
}

dword  configMgr:: structBadyType ()
{
    return m_structBadyType;
}

void  configMgr:: setStructBadyType (dword v)
{
    m_structBadyType = v;
}


void  configMgr:: onCmpKey (char* argv[])
{
    do {
		procDwordA ("structBadyType", argv, m_structBadyType);
		procStrA ("defFile", argv, m_defFile);
		procStrA ("projectDir", argv, m_projectDir);
		procStrA ("projectName", argv, m_projectName);
		procStrA ("projectInstallDir", argv, m_projectInstallDir);
		procStrA ("frameInstallPath", argv, m_frameInstallPath);
		procBoolA ("reProc", argv,  m_reProc);
		procBoolA ("installThreadDll", argv,  m_installThreadDll);
    } while (0);
}

bool  configMgr:: installThreadDll ()
{
    return m_installThreadDll;
}

void  configMgr:: setInstallThreadDll (bool v)
{
    m_installThreadDll = v;
}

const char*  configMgr:: projectDir ()
{
    return m_projectDir.get ();
}

void  configMgr:: setProjectDir (const char* v)
{
    strCpy (v, m_projectDir);
}

const char*  configMgr:: projectName ()
{
    return m_projectName.get ();
}

void  configMgr:: setProjectName (const char* v)
{
    strCpy (v, m_projectName);
}

const char*  configMgr:: frameInstallPath ()
{
    return m_frameInstallPath.get ();
}

void  configMgr:: setFrameInstallPath (const char* v)
{
    strCpy (v, m_frameInstallPath);
}

const char*  configMgr:: projectInstallDir ()
{
    return m_projectInstallDir.get ();
}

void  configMgr:: setProjectInstallDir (const char* v)
{
    strCpy (v, m_projectInstallDir);
}

