#include "structFile.h"
#include "strFun.h"
#include <string>

structFile:: structFile ()
{
	// m_canProto = false;
	m_powerCom = true;
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
	std::string strFrom = v;
	strFrom += "FromPb";
	setFromPbFuName (strFrom.c_str());
	strFrom = "void ";
	strFrom += fromPbFuName ();
	strFrom += "(";
	strFrom += v;
	strFrom += "& rData, const ";
	strFrom += v;
	strFrom += "Proto& rPb)";
	setFromPbFuFullName (strFrom.c_str ());

	strFrom = v;
	strFrom += "ToPb";
	setToPbFuName (strFrom.c_str());
	strFrom = "void ";
	strFrom += toPbFuName ();
	strFrom += "(const ";
	strFrom += v;
	strFrom += "& rData, ";
	strFrom += v;
	strFrom += "Proto& rPb)";
	setToPbFuFullName (strFrom.c_str ());
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
/*
bool  structFile:: canProto ()
{
    return m_canProto;
}

void  structFile:: setCanProto (bool v)
{
    m_canProto = v;
}
*/
const char*  structFile:: fromPbFuFullName ()
{
    return m_fromPbFuFullName.get ();
}

void  structFile:: setFromPbFuFullName (const char* v)
{
    strCpy (v, m_fromPbFuFullName);
}

const char*  structFile:: fromPbFuName ()
{
    return m_fromPbFuName.get ();
}

void  structFile:: setFromPbFuName (const char* v)
{
    strCpy (v, m_fromPbFuName);
}

const char*  structFile:: toPbFuFullName ()
{
    return m_toPbFuFullName.get ();
}

void  structFile:: setToPbFuFullName (const char* v)
{
    strCpy (v, m_toPbFuFullName);
}

const char*  structFile:: toPbFuName ()
{
    return m_toPbFuName.get ();
}

void  structFile:: setToPbFuName (const char* v)
{
    strCpy (v, m_toPbFuName);
}

bool  structFile:: powerCom ()
{
    return m_powerCom;
}

void  structFile:: setPowerCom (bool v)
{
    m_powerCom = v;
}

