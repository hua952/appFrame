#include "dataFile.h"
#include "strFun.h"
#include <string>
#include <algorithm>

dataFile:: dataFile ()
{
	m_dataLength = 0;
	m_haveSize = false;
	m_wordSize = false;
	m_zeroEnd = false;
}

dataFile:: ~dataFile ()
{
}

const char*  dataFile:: dataName ()
{
    return m_dataName.get ();
}

void  dataFile:: setDataName (const char* v)
{
    strCpy (v, m_dataName);
	std::string strM = "m_";
	strM += v;
	setMemberName (strM.c_str());
	std::string strL = v;
	transform(strL.begin(), strL.end(), strL.begin(), ::tolower);
	setLowerName (strL.c_str());
}

const char*  dataFile:: lowerName ()
{
    return m_lowerName.get ();
}

void  dataFile:: setLowerName (const char* v)
{
    strCpy (v, m_lowerName);
}

const char*  dataFile:: memberName ()
{
    return m_memberName.get ();
}

void  dataFile:: setMemberName (const char* v)
{
    strCpy (v, m_memberName);
}

const char*  dataFile:: dataType ()
{
    return m_dataType.get ();
}

void  dataFile:: setDataType (const char* v)
{
    strCpy (v, m_dataType);
}

int  dataFile:: dataLength ()
{
    return m_dataLength;
}

void  dataFile:: setDataLength (int v)
{
    m_dataLength = v;
}

bool  dataFile:: wordSize ()
{
    return m_wordSize;
}

void  dataFile:: setWordSize (bool v)
{
    m_wordSize = v;
}

bool  dataFile:: haveSize ()
{
    return m_haveSize;
}

void  dataFile:: setHaveSize (bool v)
{
    m_haveSize = v;
}

bool  dataFile:: zeroEnd ()
{
    return m_zeroEnd;
}

void  dataFile:: setZeroEnd (bool v)
{
    m_zeroEnd = v;
}

const char*  dataFile:: commit ()
{
    return m_commit.get ();
}

void  dataFile:: setCommit (const char* v)
{
    strCpy (v, m_commit);
}

