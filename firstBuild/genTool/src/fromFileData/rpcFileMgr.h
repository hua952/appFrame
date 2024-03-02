#ifndef _rpcFileMgr_h__
#define _rpcFileMgr_h__
#include <memory>
#include <map>
#include <string>
#include <vector>

class rpcFile;
class rpcFileMgr
{
public:
	using rpcMap = std::map <std::string, std::shared_ptr <rpcFile>>;
	// using rpcVec = std::vector<std::shared_ptr <rpcFile>>;
    rpcFileMgr ();
    ~rpcFileMgr ();
	rpcMap&  rpcS ();
	rpcFile*  findRpc (const char* szName);
	// rpcVec&  rpcOrder ();
private:
	// rpcVec  m_rpcOrder;
	rpcMap  m_rpcS;
};
#endif
