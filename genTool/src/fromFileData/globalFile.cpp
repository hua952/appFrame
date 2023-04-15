#include "globalFile.h"
#include "strFun.h"

globalFile:: globalFile ()
{
}

globalFile:: ~globalFile ()
{
}

const char*  globalFile::frameBinPath ()
{
    return m_frameBinPath.get ();
}

void  globalFile::setFrameBinPath (const char* v)
{
    strCpy (v, m_frameBinPath);
}

const char* globalFile:: depLibHome ()
{
    return m_depLibHome.get ();
}

void  globalFile::setDepLibHome (const char* v)
{
    strCpy (v, m_depLibHome);
}

const char*  globalFile::depIncludeHome ()
{
    return m_depIncludeHome.get ();
}

void  globalFile::setDepIncludeHome (const char* v)
{
    strCpy (v, m_depIncludeHome);
}

const char*  globalFile::frameHome ()
{
    return m_frameHome.get ();
}

void  globalFile::setFrameHome (const char* v)
{
    strCpy (v, m_frameHome);
}

const char* globalFile:: projectHome ()
{
    return m_projectHome.get ();
}

void  globalFile::setProjectHome (const char* v)
{
    strCpy (v, m_projectHome);
}

const char*  globalFile::frameLibPath ()
{
    return m_frameLibPath.get ();
}

void  globalFile::setFrameLibPath (const char* v)
{
    strCpy (v, m_frameLibPath);
}

const char* globalFile:: outPutPath ()
{
    return m_outPutPath.get ();
}

void  globalFile::setOutPutPath (const char* v)
{
    strCpy (v, m_outPutPath);
}

globalFile::msgFileV& globalFile:: msgFileS ()
{
    return m_msgFileS;
}
