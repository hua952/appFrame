#ifndef _msgGen_h__
#define _msgGen_h__
#include <memory>
#include <string>
#include <sstream>
#include <fstream>

class msgPmpFile;
class msgGroupFile;
class msgFile;
class structFile;
class dataFile;

class msgGen
{
public:
    msgGen (msgPmpFile& rPmp);
    ~msgGen ();
	int startGen ();
	int CMakeListGen ();
	int msgGroupIdGen ();
	int mkDir ();
	int msgIdGen (msgGroupFile& rGroup);
	int msgStructGen ();
	const char*  srcDir ();
	void  setSrcDir (const char* v);
	int rpcHGen ();
	int rpcCppGen ();
	int rpcInfoHGen ();
	int rpcInfoCppGen ();
	std::stringstream&  loadFileOs ();
	std::stringstream&  dumpOs ();
	std::stringstream&  checkOs ();
	std::stringstream&  protoOs ();
	const char*  protoDir ();
	void  setProtoDir (const char* v);
	std::stringstream&  pbcOs ();
	std::string&  strSerializeOut ();
	const char*  comSerFileName ();
	void  setComSerFileName (const char* v);
	const char*  protoSerFileName ();
	void  setProtoSerFileName (const char* v);
private:
	std::unique_ptr <char[]>  m_protoSerFileName;
	std::unique_ptr <char[]>  m_comSerFileName;
	std::string  m_strSerializeOut;
	std::stringstream  m_pbcOs;
	std::unique_ptr <char[]>  m_protoDir;
	std::stringstream  m_protoOs;
	std::stringstream  m_checkOs;
	std::stringstream  m_dumpOs;
	void procDataCheck (const char* structName, const char* valueName, bool bArry);
	int  genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool, std::string& strOut, std::string& strToPack);
	int  genOnceMsgClassH (msgFile& rMsg, std::string& strOut);
	int  genOnceRpcH (msgGroupFile& rG);
	int  genOnceRpcCpp (msgGroupFile& rG);
	int  genOnceStruct (structFile& rS, std::string& strOut, std::stringstream& protoStr, std::string& strSerializeOut);
	int  genOnceData (structFile& rS, dataFile& rData, std::string& strOut, std::stringstream& protoStr, int nIndex, std::stringstream& fromOs, std::stringstream& toOs);
	std::unique_ptr <char[]>  m_srcDir;
	msgPmpFile& m_rPmp;
};
#endif
