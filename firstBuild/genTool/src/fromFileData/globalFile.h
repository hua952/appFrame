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
	using  argV = std::vector<std::string>;
	// const char*  depLibHome ();
	// const char*  depIncludeHome ();
    const char*  frameLibPath ();
    const char*  frameIncPath ();
	msgFileV&  msgFileS ();
	const char*  projectName ();
	void  setProjectName (const char* v);
    const char*  frameInstallPath ();
    void  setFrameInstallPath (const char* v);
	msgPmpFile* findMsgPmp (const char* szPmpName);
	std::vector<std::string>&  rootServerS ();
	bool haveServer (); 
	bool haveMsg ();
    // const char*  thirdPartyDir ();
    // void  setThirdPartyDir (const char* v);
	void  initProtoDataTypeS ();
	protoDataTypeMap&  protoDataTypeS ();
	const char*  projectInstallDir ();
	void  setProjectInstallDir (const char* v);
	endPointGlobalMap&  endPointGlobalS ();
	const char*  configDef ();
	void  setConfigDef (const char* v);
	const char*  configFile ();
	void  setConfigFile (const char* v);
	const char*  projectDir ();
	void  setProjectDir (const char* v);
	argV&  argS ();
	const char*  configClassName ();
	void  setConfigClassName (const char* v);
	const char*  xmlDir ();
	void  setXmlDir (const char* v);
	uword  netNum ();
	void  setNetNum (uword v);
	uword  gateAppGroupId ();
	void  setGateAppGroupId (uword v);
	uword  gateRouteServerGroupId ();
	void  setGateRouteServerGroupId (uword v);
	const char*  projectHome ();
	void  setProjectHome (const char* v);
private:
	std::unique_ptr <char[]>  m_projectHome;
	std::unique_ptr <char[]>  m_projectDir;
	std::unique_ptr <char[]>  m_projectName;
    std::unique_ptr <char[]>  m_frameIncPath;
	std::unique_ptr <char[]>  m_xmlDir;
	std::unique_ptr <char[]>  m_configClassName;
	std::unique_ptr <char[]>  m_configFile;
	std::unique_ptr <char[]>  m_configDef;
	std::unique_ptr <char[]>  m_projectInstallDir;
    std::unique_ptr <char[]>  m_thirdPartyDir;
    std::unique_ptr <char[]>  m_installPath;
    std::unique_ptr <char[]>  m_frameInstallPath;
    std::unique_ptr <char[]>  m_frameLibPath;
	std::unique_ptr <char[]>  m_frameHome;
	std::unique_ptr <char[]>  m_depIncludeHome;
	std::unique_ptr <char[]>  m_depLibHome;
	uword  m_gateRouteServerGroupId;
	uword  m_netNum;
	uword  m_gateAppGroupId;
	protoDataTypeMap  m_protoDataTypeS;
	endPointGlobalMap  m_endPointGlobalS;
	msgFileV  m_msgFileS;
	std::vector<std::string>  m_rootServerS;
	// void reSetProjectInstallDir ();
	argV  m_argS;
};
#endif
