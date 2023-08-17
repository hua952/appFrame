#include "mArgMgr.h"
#include "strFun.h"
#include "loop.h"

mArgMgr:: mArgMgr ()
{
	m_savePackTag = 0;
	m_procId = c_emptyLoopHandle;
	m_packTokenTime = 5000;
}

mArgMgr:: ~mArgMgr ()
{
}

uword  mArgMgr:: savePackTag ()
{
    return m_savePackTag;
}

void  mArgMgr:: onCmpKey (char* argv[])
{
    do {
		procUwordA("savePackTag", argv, m_savePackTag);
		procStrA("netLib", argv, m_midNetLibName);
		procUwordA("procId", argv, m_procId);
    } while (0);
}

const char*  mArgMgr:: midNetLibName ()
{
    return m_midNetLibName.get ();
}

void  mArgMgr:: setMidNetLibName (const char* v)
{
    strCpy (v, m_midNetLibName);
}

loopHandleType  mArgMgr:: procId ()
{
    return m_procId;
}

void  mArgMgr:: setProcId (loopHandleType v)
{
    m_procId = v;
}

udword  mArgMgr:: packTokenTime ()
{
    return m_packTokenTime;
}

void  mArgMgr:: setPackTokenTime (udword v)
{
    m_packTokenTime = v;
}

