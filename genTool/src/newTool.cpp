#include "msgTool.h"
#include "strFun.h"

newTool::newTool ()
{
}
newTool::~newTool ()
{
}

char*  frameHome ()
{
    return m_frameHome.get ();
}

void  setFrameHome (const char* v)
{
    strCpy (v, m_frameHome);
}

char*  projectHome ()
{
    return m_projectHome.get ();
}

void  setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

char*  depIncludeHome ()
{
    return m_depIncludeHome.get ();
}

void  setDepIncludeHome (const char* v)
{
    strCpy (v, m_depIncludeHome);
}


