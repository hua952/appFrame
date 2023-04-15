#include "newTool.h"
#include "strFun.h"

newTool::newTool ()
{
}
newTool::~newTool ()
{
}

const char*  newTool::frameBinPath ()
{
    return m_frameBinPath.get ();
}

void  newTool::setFrameBinPath (const char* v)
{
    strCpy (v, m_frameBinPath);
}

const char* newTool:: depLibHome ()
{
    return m_depLibHome.get ();
}

void  newTool::setDepLibHome (const char* v)
{
    strCpy (v, m_depLibHome);
}

const char*  newTool::depIncludeHome ()
{
    return m_depIncludeHome.get ();
}

void  newTool::setDepIncludeHome (const char* v)
{
    strCpy (v, m_depIncludeHome);
}

const char*  newTool::frameHome ()
{
    return m_frameHome.get ();
}

void  newTool::setFrameHome (const char* v)
{
    strCpy (v, m_frameHome);
}

const char* newTool:: projectHome ()
{
    return m_projectHome.get ();
}

void  newTool::setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

const char*  newTool::frameLibPath ()
{
    return m_frameLibPath.get ();
}

void  newTool::setFrameLibPath (const char* v)
{
    strCpy (v, m_frameLibPath);
}

const char* newTool:: outPutPath ()
{
    return m_outPutPath.get ();
}

void  newTool::setOutPutPath (const char* v)
{
    strCpy (v, m_outPutPath);
}

newTool::msgFileV& newTool:: msgFileS ()
{
    return m_msgFileS;
}

