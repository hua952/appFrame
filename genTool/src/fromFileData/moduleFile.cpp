#include "moduleFile.h"
#include "strFun.h"

moduleFile:: moduleFile ()
{
}

moduleFile:: ~moduleFile ()
{
}

const char*  moduleFile::moduleName ()
{
    return m_moduleName.get ();
}

void  moduleFile::setModuleName (const char* v)
{
    strCpy (v, m_moduleName);
}

moduleFile::serverMap&  moduleFile:: serverS ()
{
    return m_serverS;
}

const char*  moduleFile:: appName ()
{
    return m_appName.get ();
}

void  moduleFile:: setAppName (const char* v)
{
    strCpy (v, m_appName);
}

