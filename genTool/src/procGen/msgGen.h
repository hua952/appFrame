#ifndef _msgGen_h__
#define _msgGen_h__
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

class msgPmpFile;
class msgGroupFile;
class msgFile;
class structFile;
class dataFile;
class msgGen
{
public:
	using strVecV = std::pair<std::string, std::pair<std::string, std::string>>;
	using  strVec = std::vector<strVecV>;
    msgGen (msgPmpFile& rPmp);
    ~msgGen ();
	int startGen ();
	int CMakeListGen ();
	int msgGroupIdGen ();
	int mkDir ();
	int msgIdGen (msgGroupFile& rGroup);
	int msgStructGen (std::string&  strSerializeOut);
	const char*  srcDir ();
	void  setSrcDir (const char* v);
	int rpcHGen (std::string&  strSerializeOut, strVec& serFunStuOs);
	int rpcCppGen (std::string& strToPack, std::string&  strSerializeOut, std::stringstream& serArrayOs);
	int rpcInfoHGen ();
	int rpcInfoCppGen ();
	std::stringstream&  loadFileOs ();
	std::stringstream&  dumpOs ();
	std::stringstream&  checkOs ();
	std::stringstream&  protoOs ();
	const char*  protoDir ();
	void  setProtoDir (const char* v);
	std::stringstream&  pbcOs ();
	// std::string&  strSerializeOut ();
	const char*  comSerFileName ();
	void  setComSerFileName (const char* v);
	const char*  protoSerFileName ();
	void  setProtoSerFileName (const char* v);
	const char*  protobufSerSrcDir ();
	void  setProtobufSerSrcDir (const char* v);
private:
	std::unique_ptr <char[]>  m_protobufSerSrcDir;
	std::unique_ptr <char[]>  m_protoSerFileName;
	std::unique_ptr <char[]>  m_comSerFileName;
	// std::string  m_strSerializeOut;
	std::stringstream  m_pbcOs;
	std::unique_ptr <char[]>  m_protoDir;
	std::stringstream  m_protoOs;
	std::stringstream  m_checkOs;
	std::stringstream  m_dumpOs;
	void procDataCheck (const char* structName, const char* valueName, bool bArry);
	int  genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool, std::string& strOut, std::string& strToPack, std::string&  strSerializeOut, std::stringstream& serArrayOs);
	int  genOnceMsgClassH (msgFile& rMsg, std::string& strOut, std::string&  strSerializeOut,  strVec& serFunStuOs);
	int  genOnceRpcH (msgGroupFile& rG, std::string&  strSerializeOut,  strVec& serFunStuOs);
	int  genOnceRpcCpp (msgGroupFile& rG, std::string&  strToPack, std::string&  strSerializeOut, std::stringstream& serArrayOs);
	int  genOnceStruct (structFile& rS, std::string& strOut, std::stringstream& protoStr, std::string& strSerializeOut);
	int  genOnceData (structFile& rS, dataFile& rData, std::string& strOut, std::stringstream& protoStr, int nIndex, std::stringstream& fromOs, std::stringstream& toOs);
	std::unique_ptr <char[]>  m_srcDir;
	msgPmpFile& m_rPmp;
};
#endif
