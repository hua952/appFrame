#include "serverFile.h"
#include "strFun.h"
#include "moduleFileMgr.h"
#include "moduleFile.h"
#include "appFileMgr.h"
#include "tSingleton.h"


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
	m_openNum = 1;
	m_sleepSetp=10;
	m_fpsSetp=0;
	m_autoRun = true;
	m_serverInfo.listenerNum = 0;
	m_serverInfo.connectorNum = 0;
	m_rearEnd = true;
	m_regRoute = false;
	m_route = false;
}

serverFile:: ~serverFile ()
{
}

std::vector<std::string>& serverFile:: attrs ()
{
    return m_attrs;
}

bool  serverFile:: route ()
{
    return m_route;
}

void  serverFile:: setRoute (bool v)
{
    m_route = v;
}

const char*  serverFile::serverName ()
{
    return m_serverName.get ();
}
/*
appFile*  serverFile:: getApp ()
{
    appFile*  nRet = nullptr;
    do {
		auto mN = moduleName ();
		auto pM = tSingleton<moduleFileMgr>::single().findModule (mN);
		if (pM) {
			auto aN = pM->appName ();
			auto& rAppMgr = tSingleton<appFileMgr>::single ();
			auto& rMap = rAppMgr.appS ();
			auto it = rMap.find (aN);
			if (rMap.end() != it) {
				nRet = it->second.get();
			}
		}
    } while (0);
    return nRet;
}
*/
bool   serverFile:: isRoot ()
{
    bool   nRet = 0;
    do {
		auto& rN = m_serverInfo;
		
		bool bFindDefLis = false;
		bool bFindDefCon = false;
		for (decltype (rN.listenerNum) i = 0; i < rN.listenerNum; i++) {
			if (rN.listenEndpoint[i].bDef) {
				bFindDefLis = true;
				break;
			}
		}
		for (decltype (rN.connectorNum) i = 0; i < rN.connectorNum; i++) {
			if (rN.connectEndpoint[i].bDef) {
				bFindDefCon = true;
				break;
			}
		}
		nRet =bFindDefLis && !bFindDefCon;
    } while (0);
    return nRet;
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
		strDec += "(logicServer& rServer, ForLogicFun* pForLogic)";
		setInitFunDec (strDec.c_str ());

		std::string strFrame = "app";
		strFrame += wName.get ();
		strFrame += "FrameFun";
		setFrameFunName (strFrame.c_str ());
		std::string strFrameDec = "int ";
		strFrameDec += strFrame;
		strFrameDec += R"( (logicServer& rServer))";
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

udword  serverFile:: sleepSetp ()
{
    return m_sleepSetp;
}

void  serverFile:: setSleepSetp (udword v)
{
    m_sleepSetp = v;
}

udword  serverFile:: fpsSetp ()
{
    return m_fpsSetp;
}

void  serverFile:: setFpsSetp (udword v)
{
    m_fpsSetp = v;
}

bool  serverFile:: autoRun ()
{
    return m_autoRun;
}

void  serverFile:: setAutoRun (bool v)
{
    m_autoRun = v;
}

bool  serverFile:: rearEnd ()
{
    return m_rearEnd;
}

void  serverFile:: setRearEnd (bool v)
{
    m_rearEnd = v;
}

bool  serverFile:: regRoute ()
{
    return m_regRoute;
}

void  serverFile:: setRegRoute (bool v)
{
    m_regRoute = v;
}

const char*  serverFile:: strTmpHandle ()
{
    return m_strTmpHandle.get ();
}

void  serverFile:: setStrTmpHandle (const char* v)
{
    strCpy (v, m_strTmpHandle);
}

uword  serverFile:: openNum ()
{
    return m_openNum;
}

void  serverFile:: setOpenNum (uword v)
{
    m_openNum = v;
}

uword  serverFile:: tmpNum ()
{
    return m_tmpNum;
}

void  serverFile:: setTmpNum (uword v)
{
    m_tmpNum = v;
}

uword  serverFile:: serverGroupId ()
{
    return m_serverGroupId;
}

void  serverFile:: setServerGroupId (uword v)
{
    m_serverGroupId = v;
}

const char*  serverFile:: strServerGroupId ()
{
    return m_strServerGroupId.get ();
}

void  serverFile:: setStrServerGroupId (const char* v)
{
    strCpy (v, m_strServerGroupId);
}

