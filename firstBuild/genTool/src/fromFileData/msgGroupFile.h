#ifndef _msgGroupFile_h__
#define _msgGroupFile_h__
#include <memory>
#include <string>
#include <set>
#include <vector>
#include "typeDef.h"
#include "packet.h"

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
	uword  groupOrder ();
	void  setGroupOrder (uword v);
	bool  hasOrder ();
	void  setHasOrder (bool v);
	bool  extPH ();
	void  setExtPH (bool v);
	packAddrType addrType ();
	void  setAddrType (packAddrType v);
private:
	bool  m_extPH;
	bool  m_hasOrder;
	packAddrType m_addrType;
	uword  m_groupOrder;
	std::unique_ptr <char[]>  m_rpcSrcFileName;
	std::unique_ptr <char[]>  m_fullChangName;
	rpcNameSet  m_rpcNameS;
	std::unique_ptr <char[]>  m_msgGroupName;
};
#endif
