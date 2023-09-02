#ifndef _serverFile_h__
#define _serverFile_h__
#include <memory>
#include <set>
#include <string>
#include "mainLoop.h"

struct procRpcNode
{
	std::string  rpcName;
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
	char			  szTarget [32];
	char              ip[16];
	uword             port;
	ServerIDType	  targetHandle; // use to reg route use on onConnect
	uword             userData;  // 
	bool              bDef;  // def route
	bool              bRegHandle; // 几乎没用 
};
struct toolServerNode
{
	udword                  udwUnuse;
	ServerIDType			handle;
	ubyte					connectorNum;
	ubyte                   listenerNum;
	ubyte	                ubyUnuse;
    toolServerEndPointInfo  listenEndpoint [c_onceServerMaxListenNum];
	toolServerEndPointInfo  connectEndpoint [c_onceServerMaxConnectNum];
};

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
private:
	std::unique_ptr <char[]>  m_frameFunDec;
	std::unique_ptr <char[]>  m_frameFunName;
	std::unique_ptr <char[]>  m_initFunName;
	std::unique_ptr <char[]>  m_initFunDec;
	std::unique_ptr <char[]>  m_regPackFunDec;
	std::unique_ptr <char[]>  m_regPackFunName;
	std::unique_ptr <char[]>  m_strHandle;
	std::unique_ptr <char[]>  m_moduleName;
	std::unique_ptr <char[]>  m_serverName;
	std::unique_ptr <char[]>  m_commit;
	toolServerNode  m_serverInfo;
	rpcMap			m_procMsgS;
	udword			m_fpsSetp;
	udword			m_sleepSetp;
};
#endif
