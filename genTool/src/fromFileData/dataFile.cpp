#include "dataFile.h"
#include "strFun.h"

dataFile:: dataFile ()
{
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
