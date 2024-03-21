#ifndef _msgFile_h__
#define _msgFile_h__
#include <memory>
#include "packet.h"
#include "structFile.h"

class msgFile: public structFile
{
public:
    msgFile ();
    ~msgFile ();
	const char*  defProServerId ();
	void  setDefProServerId (const char* v);
	const char*  strMsgId ();
	void  setStrMsgId (const char* v);
	const char*  packFunName ();
	void  setPackFunName (const char* v);
	const char*  msgFunName ();
	void  setMsgFunName (const char* v);
	void  getClassMsgFunDec (const char* className, std::string& strDec);
	const char*  msgFunDec ();
	void  setMsgFunDec (const char* v);
	const char*  msgName ();
	void  setMsgName (const char* v);
	bool  extPH ();
	void  setExtPH (bool v);
	packAddrType addrType ();
	void  setAddrType (packAddrType v);
	bool  neetSession ();
	void  setNeetSession (bool v);
	const char*  serFunStuFromName ();
	void  setSerFunStuFromName (const char* v);
	const char*  serFunStuToName ();
	void  setSerFunStuToName (const char* v);
	const char*  msgFullName ();
	void  setMsgFullName (const char* v);
	const char*  defProServerTmpId ();
	void  setDefProServerTmpId (const char* v);
private:
	std::unique_ptr <char[]>  m_defProServerTmpId;
	std::unique_ptr <char[]>  m_msgFullName;
	std::unique_ptr <char[]>  m_serFunStuToName;
	std::unique_ptr <char[]>  m_serFunStuFromName;
	bool  m_neetSession;
	packAddrType m_addrType;
	bool  m_extPH;
	std::unique_ptr <char[]>  m_msgName;
	std::unique_ptr <char[]>  m_msgFunDec;
	std::unique_ptr <char[]>  m_msgFunName;
	std::unique_ptr <char[]>  m_packFunName;
	std::unique_ptr <char[]>  m_strMsgId;
	std::unique_ptr <char[]>  m_defProServerId;
};
#endif
