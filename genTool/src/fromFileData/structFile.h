#ifndef _structFile_h__
#define _structFile_h__
#include <memory>
#include <vector>
#include <map>
#include <string>

class dataFile;
class structFile
{
public:
	using dataV = std::vector <std::shared_ptr <dataFile>>;
	using dataMap = std::map <std::string, std::shared_ptr <dataFile>>;
    structFile ();
    ~structFile ();
	const char*  structName ();
	void  setStructName (const char* v);
	const char*  commit ();
	void  setCommit (const char* v);
	dataV&  dataOrder ();
	dataMap&  dataS ();
	bool   hasData ();
	/*
	bool  canProto ();
	void  setCanProto (bool v);
	*/
	const char*  fromPbFuName ();
	void  setFromPbFuName (const char* v);
	const char*  fromPbFuFullName ();
	void  setFromPbFuFullName (const char* v);
	const char*  toPbFuFullName ();
	void  setToPbFuFullName (const char* v);
	const char*  toPbFuName ();
	void  setToPbFuName (const char* v);
	bool  powerCom ();
	void  setPowerCom (bool v);
private:
	bool  m_powerCom;
	std::unique_ptr <char[]>  m_toPbFuName;
	std::unique_ptr <char[]>  m_toPbFuFullName;
	std::unique_ptr <char[]>  m_fromPbFuFullName;
	std::unique_ptr <char[]>  m_fromPbFuName;
	// bool  m_canProto;
	dataMap  m_dataS;
	dataV  m_dataOrder;
	std::unique_ptr <char[]>  m_commit;
	std::unique_ptr <char[]>  m_structName;
};
#endif
