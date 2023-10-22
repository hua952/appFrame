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
private:
	std::stringstream  m_checkOs;
	std::stringstream  m_dumpOs;
	std::stringstream  m_loadFileOs;
	void procDataCheck (const char* structName, const char* valueName, bool bArry);
	int  genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool, std::string& strOut);
	int  genOnceMsgClassH (msgFile& rMsg, std::string& strOut);
	int  genOnceRpcH (msgGroupFile& rG);
	int  genOnceRpcCpp (msgGroupFile& rG);
	int  genOnceStruct (structFile& rS, std::string& strOut);
	int  genOnceData (structFile& rS, dataFile& rData, std::string& strOut);
	std::unique_ptr <char[]>  m_srcDir;
	msgPmpFile& m_rPmp;
};
#endif
