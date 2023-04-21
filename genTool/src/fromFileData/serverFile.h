#ifndef _serverFile_h__
#define _serverFile_h__
#include <memory>
#include <set>
#include <string>

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
private:
	std::unique_ptr <char[]>  m_moduleName;
	std::unique_ptr <char[]>  m_serverName;
	std::unique_ptr <char[]>  m_commit;
	rpcMap  m_procMsgS;
};
#endif
