#include <string>
#include <sstream>
#include "msgFile.h"
#include "strFun.h"
#include "myAssert.h"
#include <string>

msgFile:: msgFile ()
{
	m_extPH = false;
	m_neetSession = false;
	m_addrType = addrType_ser;
	strCpy ("c_emptyLoopHandle", m_defProServerId);
}

msgFile:: ~msgFile ()
{
}

const char*  msgFile:: defProServerId ()
{
    return m_defProServerId.get ();
}

void  msgFile:: setDefProServerId (const char* v)
{
    strCpy (v, m_defProServerId);
}

const char*  msgFile:: strMsgId ()
{
    return m_strMsgId.get ();
}

void  msgFile:: setStrMsgId (const char* v)
{
    strCpy (v, m_strMsgId);
}

const char*  msgFile:: packFunName ()
{
    return m_packFunName.get ();
}

void  msgFile:: setPackFunName (const char* v)
{
    strCpy (v, m_packFunName);
}

const char*  msgFile:: msgFunName ()
{
    return m_msgFunName.get ();
}

void  msgFile:: setMsgFunName (const char* v)
{
    strCpy (v, m_msgFunName);
}

bool  msgFile:: neetSession ()
{
    return m_neetSession;
}

void  msgFile:: setNeetSession (bool v)
{
    m_neetSession = v;
}

void   msgFile:: getClassMsgFunDec (const char* className, std::string& strDec)
{
    do {
		auto pDec = msgFunDec ();
		auto pName = msgFunName ();
		std::string strOldDec = pDec;
		auto isF = strOldDec.find (pName);
		myAssert (isF != strOldDec.npos);
		auto first = strOldDec.substr (0, isF);
		auto second = strOldDec.substr (isF);
		std::stringstream ss;
		ss<<first<<" "<<className<<"::"<<second;
		strDec = ss.str();
    } while (0);
}

const char*  msgFile:: msgFunDec ()
{
    return m_msgFunDec.get ();
}

void  msgFile:: setMsgFunDec (const char* v)
{
    strCpy (v, m_msgFunDec);
}

const char*  msgFile:: msgName ()
{
    return m_msgName.get ();
}

void  msgFile:: setMsgName (const char* v)
{
    strCpy (v, m_msgName);
	std::string str =v;
	str += "FromSerFun";
	setSerFunStuFromName (str.c_str());	

	str =v;
	str += "ToSerFun";
	setSerFunStuToName (str.c_str());	
}

const char*  msgFile:: serFunStuToName ()
{
    return m_serFunStuToName.get ();
}

void  msgFile:: setSerFunStuToName (const char* v)
{
    strCpy (v, m_serFunStuToName);
}

const char*  msgFile:: serFunStuFromName ()
{
    return m_serFunStuFromName.get ();
}

void  msgFile:: setSerFunStuFromName (const char* v)
{
    strCpy (v, m_serFunStuFromName);
}

bool  msgFile:: extPH ()
{
    return m_extPH;
}

void  msgFile:: setExtPH (bool v)
{
    m_extPH = v;
}

packAddrType msgFile:: addrType ()
{
    return m_addrType;
}

void  msgFile:: setAddrType (packAddrType v)
{
    m_addrType = v;
}

const char*  msgFile:: msgFullName ()
{
    return m_msgFullName.get ();
}

void  msgFile:: setMsgFullName (const char* v)
{
    strCpy (v, m_msgFullName);
}

const char*  msgFile:: defProServerTmpId ()
{
    return m_defProServerTmpId.get ();
}

void  msgFile:: setDefProServerTmpId (const char* v)
{
    strCpy (v, m_defProServerTmpId);
}

