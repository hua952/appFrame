#include "serverFile.h"
#include "strFun.h"

cmpProcRpcNode::cmpProcRpcNode ()
{
}
cmpProcRpcNode::~cmpProcRpcNode ()
{
}

bool  cmpProcRpcNode::operator () (const procRpcNode& a, const procRpcNode& b) const
{
	bool bRet = true;
	do {
		if (a.rpcName < b.rpcName) {
			break;
		}
		if (b.rpcName < a.rpcName) {
			bRet = false;
			break;
		}
		bRet = a.bAsk;	
	} while (0);
	return bRet;
}

serverFile:: serverFile ()
{
}

serverFile:: ~serverFile ()
{
}

const char*  serverFile::serverName ()
{
    return m_serverName.get ();
}

void  serverFile::setServerName (const char* v)
{
    strCpy (v, m_serverName);
}

const char*  serverFile:: commit ()
{
    return m_commit.get ();
}

void  serverFile:: setCommit (const char* v)
{
    strCpy (v, m_commit);
}

serverFile::rpcMap&  serverFile:: procMsgS ()
{
    return m_procMsgS;
}

const char*  serverFile:: moduleName ()
{
    return m_moduleName.get ();
}

void  serverFile:: setModuleName (const char* v)
{
    strCpy (v, m_moduleName);
}

