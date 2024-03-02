#include "msgPmpFile.h"
#include "strFun.h"
#include <string>

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
	std::string strS = v;
	strS += "SerializePackFunS";
	setSerializePackFunStName (strS.c_str());
}

msgPmpFile::msgFileV&  msgPmpFile:: msgDefFileS ()
{
    return m_msgDefFileS;
}


const char*  msgPmpFile:: serializePackFunStName ()
{
    return m_serializePackFunStName.get ();
}

void  msgPmpFile:: setSerializePackFunStName (const char* v)
{
    strCpy (v, m_serializePackFunStName);
}

