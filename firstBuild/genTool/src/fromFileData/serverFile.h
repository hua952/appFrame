#ifndef _serverFile_h__
#define _serverFile_h__
#include <memory>
#include <set>
#include <string>
#include "mainLoop.h"

#include <vector>

struct procRpcNode
{
	std::string  rpcName;
	std::string  retValue;
	bool         bAsk;
};

class cmpProcRpcNode
{
public:
    cmpProcRpcNode ();
    ~cmpProcRpcNode ();
	bool operator () (const procRpcNode& a, const procRpcNode& b) const;
private:
};

struct toolServerEndPointInfo
{
	char			  endPointName[24];
	char			  targetEndPoint [24];
	char              ip[16];
	uword             port;
	ServerIDType	  targetHandle; // use to reg route use on onConnect
	uword             userData;  // 
	bool              bDef;  // def route
};

struct toolServerNode
{
	udword                  udwUnuse;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
    toolServerEndPointInfo  listenEndpoint [c_onceServerMaxListenNum];
	toolServerEndPointInfo  connectEndpoint [c_onceServerMaxConnectNum];
};
class appFile;
class serverFile
{
public:
	using rpcMap = std::set<procRpcNode, cmpProcRpcNode>;
    serverFile ();
    ~serverFile ();
	const char*  serverName ();
	void  setServerName (const char* v);
	const char*  commit ();
	void  setCommit (const char* v);
	rpcMap&  procMsgS ();
	const char*  moduleName ();
	void  setModuleName (const char* v);
	// appFile* getApp ();
	const char*  strHandle ();
	void  setStrHandle (const char* v);
	toolServerNode&  serverInfo ();
	const char*  regPackFunName ();
	void  setRegPackFunName (const char* v);
	const char*  regPackFunDec ();
	void  setRegPackFunDec (const char* v);
	const char*  initFunDec ();
	void  setInitFunDec (const char* v);
	const char*  initFunName ();
	void  setInitFunName (const char* v);
	const char*  frameFunName ();
	void  setFrameFunName (const char* v);
	const char*  frameFunDec ();
	void  setFrameFunDec (const char* v);
	
	udword  sleepSetp ();
	void  setSleepSetp (udword v);
	udword  fpsSetp ();
	void  setFpsSetp (udword v);
	bool  isRoot ();
	bool  autoRun ();
	void  setAutoRun (bool v);
	bool  rearEnd ();
	void  setRearEnd (bool v);
	bool  regRoute ();
	void  setRegRoute (bool v);
	const char*  strTmpHandle ();
	void  setStrTmpHandle (const char* v);
	uword  openNum ();
	void  setOpenNum (uword v);
	uword  tmpNum ();
	void  setTmpNum (uword v);
	bool  route ();
	void  setRoute (bool v);
	const char*  strServerGroupId ();
	void  setStrServerGroupId (const char* v);
	uword serverGroupId ();
	void  setServerGroupId (uword v);
	std::vector<std::string>&     attrs ();
private:
	std::vector<std::string>  m_attrs;
	std::unique_ptr <char[]>  m_strServerGroupId;
	std::unique_ptr <char[]>  m_frameFunDec;
	std::unique_ptr <char[]>  m_strTmpHandle;
	std::unique_ptr <char[]>  m_frameFunName;
	std::unique_ptr <char[]>  m_initFunName;
	std::unique_ptr <char[]>  m_initFunDec;
	std::unique_ptr <char[]>  m_regPackFunDec;
	std::unique_ptr <char[]>  m_regPackFunName;
	std::unique_ptr <char[]>  m_strHandle;
	std::unique_ptr <char[]>  m_moduleName;
	std::unique_ptr <char[]>  m_serverName;
	std::unique_ptr <char[]>  m_commit;
	udword			m_fpsSetp;
	udword			m_sleepSetp;
	uword  m_tmpNum;
	uword  m_serverGroupId{0};
	uword  m_openNum;
	bool  m_regRoute;
	bool  m_route;
	bool  m_rearEnd;
	bool  m_autoRun;
	
	toolServerNode  m_serverInfo;
	rpcMap			m_procMsgS;
};
#endif
