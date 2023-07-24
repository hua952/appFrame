#ifndef _msgFile_h__
#define _msgFile_h__
#include <memory>
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
	const char*  msgFunDec ();
	void  setMsgFunDec (const char* v);
	const char*  msgName ();
	void  setMsgName (const char* v);
private:
	std::unique_ptr <char[]>  m_msgName;
	std::unique_ptr <char[]>  m_msgFunDec;
	std::unique_ptr <char[]>  m_msgFunName;
	std::unique_ptr <char[]>  m_packFunName;
	std::unique_ptr <char[]>  m_strMsgId;
	std::unique_ptr <char[]>  m_defProServerId;
};
#endif
