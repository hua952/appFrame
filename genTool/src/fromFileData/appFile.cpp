#include "appFile.h"
#include "strFun.h"

appFile:: appFile ()
{
}

appFile:: ~appFile ()
{
}

const char*  appFile::appName ()
{
    return m_appName.get ();
}

void  appFile::setAppName (const char* v)
{
    strCpy (v, m_appName);
}

appFile::moduleFileNameSet&  appFile:: moduleFileNameS ()
{
    return m_moduleFileNameS;
}

