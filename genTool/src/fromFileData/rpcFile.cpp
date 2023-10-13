#include "rpcFile.h"
#include "strFun.h"
#include <cstring>

rpcFile:: rpcFile ()
{
}

rpcFile:: ~rpcFile ()
{
}

const char*  rpcFile:: commit ()
{
    return m_commit.get ();
}

void  rpcFile:: setCommit (const char* v)
{
    strCpy (v, m_commit);
}

const char*  rpcFile:: rpcName ()
{
    return m_rpcName.get ();
}

void  rpcFile:: setRpcName (const char* v)
{
    strCpy (v, m_rpcName);
}

const char*  rpcFile:: groupName ()
{
    return m_groupName.get ();
}

void  rpcFile:: setGroupName (const char* v)
{
    strCpy (v, m_groupName);
}

const char*  rpcFile:: askMsgName ()
{
    return m_askMsgName.get ();
}

void  rpcFile:: setAskMsgName (const char* v)
{
    strCpy (v, m_askMsgName);
}

const char*  rpcFile:: retMsgName ()
{
    return m_retMsgName.get ();
}

void  rpcFile:: setRetMsgName (const char* v)
{
    strCpy (v, m_retMsgName);
}

bool          rpcFile:: isChannel ()
{
    bool          nRet = false;
    do {
		const char** getRpptRpc (int &num);
		int szRootRpcNum = 0;
		auto szRootRpc = getRpptRpc (szRootRpcNum);
		for (decltype (szRootRpcNum) i = 0; i < szRootRpcNum; i++) {
			if (strcmp (rpcName (), szRootRpc[i]) == 0) {
				nRet = true;
				break;
			}
		}
    } while (0);
    return nRet;
}

