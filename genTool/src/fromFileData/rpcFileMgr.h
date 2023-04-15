#ifndef _rpcFileMgr_h__
#define _rpcFileMgr_h__
#include <memory>
#include <map>
#include <string>

class rpcFile;
class rpcFileMgr
{
public:
	using rpcMap = std::map <std::string, std::shared_ptr <rpcFile>>;
    rpcFileMgr ();
    ~rpcFileMgr ();
	rpcMap&  rpcS ();
private:
	rpcMap  m_rpcS;
};
#endif
