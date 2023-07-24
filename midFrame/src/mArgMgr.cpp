#include "mArgMgr.h"
#include "strFun.h"

mArgMgr:: mArgMgr ()
{
	m_savePackTag = 0;
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
    } while (0);
}

