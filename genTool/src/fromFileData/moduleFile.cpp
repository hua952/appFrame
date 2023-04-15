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

