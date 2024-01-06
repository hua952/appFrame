#ifndef _globalFile_h__
#define _globalFile_h__
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "comFun.h"

class msgPmpFile;
class globalFile
{
public:
    globalFile ();
    ~globalFile ();
	using endPointGlobalMap = std::map<std::string, uword>;
	using msgFileV = std::map<std::string, std::shared_ptr<msgPmpFile>>;
	using protoDataTypeMap = std::map<std::string, std::string>;
	const char*  depLibHome ();
	const char*  depIncludeHome ();
	const char*  projectHome ();
	void  setProjectHome (const char* v);
    const char*  frameLibPath ();
    const char*  frameIncPath ();
	msgFileV&  msgFileS ();
	const char*  projectName ();
	void  setProjectName (const char* v);
    const char*  frameInstallPath ();
    void  setFrameInstallPath (const char* v);
    // void  getRealInstallPath (std::string& strPath);
	msgPmpFile* findMsgPmp (const char* szPmpName);
	std::vector<std::string>&  rootServerS ();
	bool haveServer (); 
    const char*  thirdPartyDir ();
    void  setThirdPartyDir (const char* v);
	void  initProtoDataTypeS ();
	protoDataTypeMap&  protoDataTypeS ();
	const char*  projectInstallDir ();
	void  setProjectInstallDir (const char* v);
	endPointGlobalMap&  endPointGlobalS ();
private:
	endPointGlobalMap  m_endPointGlobalS;
	std::unique_ptr <char[]>  m_projectInstallDir;
    std::unique_ptr <char[]>  m_thirdPartyDir;
    std::unique_ptr <char[]>  m_installPath;
    std::unique_ptr <char[]>  m_frameInstallPath;
	std::unique_ptr <char[]>  m_projectName;
    std::unique_ptr <char[]>  m_frameLibPath;
    std::unique_ptr <char[]>  m_frameIncPath;
	std::unique_ptr <char[]>  m_projectHome;
	std::unique_ptr <char[]>  m_frameHome;
	std::unique_ptr <char[]>  m_depIncludeHome;
	std::unique_ptr <char[]>  m_depLibHome;
	protoDataTypeMap  m_protoDataTypeS;
	msgFileV  m_msgFileS;
	std::vector<std::string>  m_rootServerS;
	void reSetProjectInstallDir ();
};
#endif
