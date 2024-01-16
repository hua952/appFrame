#include "mArgMgr.h"
#include "strFun.h"
#include "loop.h"

mArgMgr:: mArgMgr ()
{
	m_savePackTag = 0;
	m_dumpMsg = false;
	m_procId = c_emptyLoopHandle;
	m_packTokenTime = 50000;
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
		procBoolA("dumpMsg", argv, m_dumpMsg);
		procUwordA("savePackTag", argv, m_savePackTag);
		procStrA("netLib", argv, m_midNetLibName);
		procStrA("workDir", argv, m_workDir);
		procUwordA("procId", argv, m_procId);
    } while (0);
}

const char*  mArgMgr:: workDir ()
{
    return m_workDir.get ();
}

void  mArgMgr:: setWorkDir (const char* v)
{
    strCpy (v, m_workDir);
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

bool  mArgMgr:: dumpMsg ()
{
    return m_dumpMsg;
}

void  mArgMgr:: setDumpMsg (bool v)
{
    m_dumpMsg = v;
}

