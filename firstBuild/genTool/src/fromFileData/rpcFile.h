#ifndef _rpcFile_h__
#define _rpcFile_h__
#include <memory>

class rpcFile
{
public:
    rpcFile ();
    ~rpcFile ();
	// bool         isChannel ();
	const char*  commit ();
	void  setCommit (const char* v);
	const char*  rpcName ();
	void  setRpcName (const char* v);
	const char*  groupName ();
	void  setGroupName (const char* v);
	const char*  askMsgName ();
	void  setAskMsgName (const char* v);
	const char*  retMsgName ();
	void  setRetMsgName (const char* v);
private:
	std::unique_ptr <char[]>  m_retMsgName;
	std::unique_ptr <char[]>  m_askMsgName;
	std::unique_ptr <char[]>  m_groupName;
	std::unique_ptr <char[]>  m_rpcName;
	std::unique_ptr <char[]>  m_commit;
};
#endif
