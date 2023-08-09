#ifndef _msgGen_h__
#define _msgGen_h__
#include <memory>
#include <string>

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
private:
	int  genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool, std::string& strOut);
	int  genOnceMsgClassH (msgFile& rMsg, std::string& strOut);
	int  genOnceRpcH (msgGroupFile& rG);
	int  genOnceRpcCpp (msgGroupFile& rG);
	int  genOnceStruct (structFile& rS, std::string& strOut);
	int  genOnceData (dataFile& rData, std::string& strOut);
	std::unique_ptr <char[]>  m_srcDir;
	msgPmpFile& m_rPmp;
};
#endif
