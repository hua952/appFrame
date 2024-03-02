#include "itemMgr.h"
#include "strFun.h"

itemMgr:: itemMgr ()
{
}

itemMgr:: ~itemMgr ()
{
}

itemMgr:: itemMap&  itemMgr:: itemS ()
{
    return m_itemS;
}

itemMgr& itemMgr:: mgr ()
{
	static itemMgr s_Mgr;
	return s_Mgr;
}

const char*  itemMgr:: className ()
{
    return m_className.get ();
}

void  itemMgr:: setClassName (const char* v)
{
    strCpy (v, m_className);
}

const char*  itemMgr:: projectName ()
{
    return m_projectName.get ();
}

void  itemMgr:: setProjectName (const char* v)
{
    strCpy (v, m_projectName);
}

const char*  itemMgr:: projectDir ()
{
    return m_projectDir.get ();
}

void  itemMgr:: setProjectDir (const char* v)
{
    strCpy (v, m_projectDir);
}

const char*  itemMgr:: includeDir ()
{
    return m_includeDir.get ();
}

void  itemMgr:: setIncludeDir (const char* v)
{
    strCpy (v, m_includeDir);
}

const char*  itemMgr:: libDir ()
{
    return m_libDir.get ();
}

void  itemMgr:: setLibDir (const char* v)
{
    strCpy (v, m_libDir);
}

