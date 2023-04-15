#ifndef _msgGroupFile_h__
#define _msgGroupFile_h__
#include <memory>
#include <string>
#include <set>

class msgGroupFile
{
public:
	using rpcNameSet = std::set<std::string>;
    msgGroupFile ();
    ~msgGroupFile ();
	const char*  msgGroupName ();
	void  setMsgGroupName (const char* v);
	rpcNameSet&  rpcNameS ();
private:
	rpcNameSet  m_rpcNameS;
	std::unique_ptr <char[]>  m_msgGroupName;
};
#endif
