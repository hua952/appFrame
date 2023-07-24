#include "structFile.h"
#include "strFun.h"

structFile:: structFile ()
{
}

structFile:: ~structFile ()
{
}

const char*  structFile:: structName ()
{
    return m_structName.get ();
}

void  structFile:: setStructName (const char* v)
{
    strCpy (v, m_structName);
}

const char*  structFile:: commit ()
{
    return m_commit.get ();
}

void  structFile:: setCommit (const char* v)
{
    strCpy (v, m_commit);
}

structFile::dataV&  structFile:: dataOrder ()
{
    return m_dataOrder;
}

structFile::dataMap&  structFile:: dataS ()
{
    return m_dataS;
}

bool    structFile:: hasData ()
{
    bool    nRet = false;
    do {
		nRet = !m_dataS.empty ();
    } while (0);
    return nRet;
}

