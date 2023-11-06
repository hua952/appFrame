#ifndef _msgPmpFile_h__
#define _msgPmpFile_h__
#include <memory>
#include "structFileMgr.h"
#include "msgGroupFileMgr.h"
#include "rpcFileMgr.h"
#include "msgFileMgr.h"
#include <string>

class msgPmpFile
{
public:
	using msgFileV = std::vector<std::string>;
    msgPmpFile ();
    ~msgPmpFile ();
	structFileMgr&  structFileS ();
	msgGroupFileMgr&  msgGroupFileS ();
	rpcFileMgr&  rpcFileS ();
	msgFileMgr&  msgFileS ();
	/*
	const char*  defFile ();
	void  setDefFile (const char* v);
	*/
	const char*  pmpName ();
	void  setPmpName (const char* v);
	msgFileV&  msgDefFileS ();
	const char*  serializePackFunStName ();
	void  setSerializePackFunStName (const char* v);
private:
	std::unique_ptr <char[]>  m_serializePackFunStName;

	msgFileV  m_msgDefFileS;
	std::unique_ptr <char[]>  m_pmpName;
	// std::unique_ptr <char[]>  m_defFile;
	msgFileMgr  m_msgFileS;
	rpcFileMgr  m_rpcFileS;
	msgGroupFileMgr  m_msgGroupFileS;
	structFileMgr	  m_structFileS;
};
#endif
