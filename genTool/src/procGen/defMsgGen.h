#ifndef _defMsgGen_h__
#define _defMsgGen_h__
#include <memory>
#include <string>

class dataFile;
class structFile;
class msgGroupFile;
class msgFile;
class defMsgGen
{
public:
    defMsgGen ();
    ~defMsgGen ();
	int CMakeListGen ();
	int startGen ();
	int msgGroupIdGen ();
	int msgIdGen (msgGroupFile& rGroup);
	int msgStructGen ();
	int rpcHGen ();
	int rpcCppGen ();
	int loopHandleSGen ();
	int rpcInfoHGen ();
	int rpcInfoCppGen ();
	int mkDir ();
	const char*  srcDir ();
	void  setSrcDir (const char* v);
private:
	int  genOnceRpcH (msgGroupFile& rG);
	int  genOnceRpcCpp (msgGroupFile& rG);
	int  genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool, std::string& strOut);
	int  genOnceMsgClassH (msgFile& rMsg, std::string& strOut);
	int  genOnceStruct (structFile& rS, std::string& strOut);
	int  genOnceData (dataFile& rData, std::string& strOut);
	std::unique_ptr <char[]>  m_srcDir;
};

#endif
