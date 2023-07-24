#ifndef _msgGroupFile_h__
#define _msgGroupFile_h__
#include <memory>
#include <string>
#include <set>
#include <vector>

class msgGroupFile
{
public:
	// using rpcNameSet = std::set<std::string>;
	using rpcNameSet = std::vector <std::string>;
    msgGroupFile ();
    ~msgGroupFile ();
	const char*  msgGroupName ();
	void  setMsgGroupName (const char* v);
	rpcNameSet&  rpcNameS ();
	const char*  fullChangName ();
	void  setFullChangName (const char* v);
	const char*  rpcSrcFileName ();
	void  setRpcSrcFileName (const char* v);
private:
	std::unique_ptr <char[]>  m_rpcSrcFileName;
	std::unique_ptr <char[]>  m_fullChangName;
	rpcNameSet  m_rpcNameS;
	std::unique_ptr <char[]>  m_msgGroupName;
};
#endif
