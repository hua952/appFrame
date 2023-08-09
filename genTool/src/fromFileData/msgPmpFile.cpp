#include "msgPmpFile.h"
#include "strFun.h"

msgPmpFile:: msgPmpFile ()
{
}

msgPmpFile:: ~msgPmpFile ()
{
}

msgGroupFileMgr&  msgPmpFile:: msgGroupFileS ()
{
    return m_msgGroupFileS;
}

rpcFileMgr&  msgPmpFile:: rpcFileS ()
{
    return m_rpcFileS;
}

msgFileMgr&  msgPmpFile:: msgFileS ()
{
    return m_msgFileS;
}

structFileMgr& msgPmpFile :: structFileS ()
{
    return m_structFileS;
}
/*
const char*  msgPmpFile:: defFile ()
{
    return m_defFile.get ();
}

void  msgPmpFile:: setDefFile (const char* v)
{
    strCpy (v, m_defFile);
}
*/
const char*  msgPmpFile:: pmpName ()
{
    return m_pmpName.get ();
}

void  msgPmpFile:: setPmpName (const char* v)
{
    strCpy (v, m_pmpName);
}

msgPmpFile::msgFileV&  msgPmpFile:: msgDefFileS ()
{
    return m_msgDefFileS;
}


