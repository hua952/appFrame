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

