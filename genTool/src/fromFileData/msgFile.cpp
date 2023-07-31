#include <string>
#include <sstream>
#include "msgFile.h"
#include "strFun.h"
#include "myAssert.h"

msgFile:: msgFile ()
{
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
}

