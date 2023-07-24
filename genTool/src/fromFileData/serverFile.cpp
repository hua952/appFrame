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
	m_serverInfo.listenerNum = 0;
	m_serverInfo.connectorNum = 0;
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
	if (v) {
		std::unique_ptr <char[]>  wName;
		strCpy (v, wName);
		toWord (wName.get());
		std::string strInit = "on";
		strInit += wName.get ();
		strInit += "Init";
		setInitFunName (strInit.c_str ());
		std::string strDec = "int ";
		strDec += strInit;
		strDec += "(ForLogicFun* pForLogic)";
		setInitFunDec (strDec.c_str ());

		std::string strFrame = "app";
		strFrame += wName.get ();
		strFrame += "FrameFun";
		setFrameFunName (strFrame.c_str ());
		std::string strFrameDec = "int ";
		strFrameDec += strFrame;
		strFrameDec += R"( (void* pArgS))";
		setFrameFunDec (strFrameDec.c_str ());
	}
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

const char*  serverFile:: strHandle ()
{
    return m_strHandle.get ();
}

void  serverFile:: setStrHandle (const char* v)
{
    strCpy (v, m_strHandle);
}

toolServerNode&  serverFile:: serverInfo ()
{
    return m_serverInfo;
}

const char*  serverFile:: regPackFunName ()
{
    return m_regPackFunName.get ();
}

void  serverFile:: setRegPackFunName (const char* v)
{
    strCpy (v, m_regPackFunName);
}

const char*  serverFile:: regPackFunDec ()
{
    return m_regPackFunDec.get ();
}

void  serverFile:: setRegPackFunDec (const char* v)
{
    strCpy (v, m_regPackFunDec);
}

const char*  serverFile:: initFunDec ()
{
    return m_initFunDec.get ();
}

void  serverFile:: setInitFunDec (const char* v)
{
    strCpy (v, m_initFunDec);
}

const char*  serverFile:: initFunName ()
{
    return m_initFunName.get ();
}

void  serverFile:: setInitFunName (const char* v)
{
    strCpy (v, m_initFunName);
}

const char*  serverFile:: frameFunName ()
{
    return m_frameFunName.get ();
}

void  serverFile:: setFrameFunName (const char* v)
{
    strCpy (v, m_frameFunName);
}

const char*  serverFile:: frameFunDec ()
{
    return m_frameFunDec.get ();
}

void  serverFile:: setFrameFunDec (const char* v)
{
    strCpy (v, m_frameFunDec);
}

