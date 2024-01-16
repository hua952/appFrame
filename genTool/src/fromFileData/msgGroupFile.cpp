#include "msgGroupFile.h"
#include "strFun.h"

msgGroupFile:: msgGroupFile ()
{
	m_hasOrder = false;
	m_extPH  = false;
	m_addrType = addrType_ser;
}

msgGroupFile:: ~msgGroupFile ()
{
}

const char*  msgGroupFile:: msgGroupName ()
{
    return m_msgGroupName.get ();
}

void  msgGroupFile:: setMsgGroupName (const char* v)
{
    strCpy (v, m_msgGroupName);
}

msgGroupFile::rpcNameSet&  msgGroupFile:: rpcNameS ()
{
    return m_rpcNameS;
}

const char*  msgGroupFile:: fullChangName ()
{
    return m_fullChangName.get ();
}

void  msgGroupFile:: setFullChangName (const char* v)
{
    strCpy (v, m_fullChangName);
}

const char*  msgGroupFile:: rpcSrcFileName ()
{
    return m_rpcSrcFileName.get ();
}

void  msgGroupFile:: setRpcSrcFileName (const char* v)
{
    strCpy (v, m_rpcSrcFileName);
}

uword  msgGroupFile:: groupOrder ()
{
    return m_groupOrder;
}

void  msgGroupFile:: setGroupOrder (uword v)
{
    m_groupOrder = v;
}

bool  msgGroupFile:: hasOrder ()
{
    return m_hasOrder;
}

void  msgGroupFile:: setHasOrder (bool v)
{
    m_hasOrder = v;
}

bool  msgGroupFile:: extPH ()
{
    return m_extPH;
}

void  msgGroupFile:: setExtPH (bool v)
{
    m_extPH = v;
}
packAddrType msgGroupFile:: addrType ()
{
    return m_addrType;
}

void  msgGroupFile:: setAddrType (packAddrType v)
{
    m_addrType = v;
}

