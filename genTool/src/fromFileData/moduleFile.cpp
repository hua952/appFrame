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

serverFile*   moduleFile:: findServer (const char* szName)
{
    serverFile*   nRet = nullptr;
    do {
		auto& rSS = serverS ();
		auto it = rSS.find (szName);
		if (rSS.end () != it) {
			nRet = it->second.get ();
		}
    } while (0);
    return nRet;
}

moduleFile::serverOrder&  moduleFile:: orderS ()
{
    return m_orderS;
}


