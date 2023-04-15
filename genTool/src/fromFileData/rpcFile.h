#ifndef _rpcFile_h__
#define _rpcFile_h__
#include <memory>

class rpcFile
{
public:
    rpcFile ();
    ~rpcFile ();
	const char*  commit ();
	void  setCommit (const char* v);
	const char*  rpcName ();
	void  setRpcName (const char* v);
private:
	std::unique_ptr <char[]>  m_rpcName;
	std::unique_ptr <char[]>  m_commit;
};
#endif
