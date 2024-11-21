#ifndef _xmlGlobalLoad_h__
#define _xmlGlobalLoad_h__
#include <memory>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
// #include <rapidxml/rapidxml_print.hpp>
// #include "rapidxml/rapidxml_ext.hpp"

#include "fromFileData/globalFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/serverFile.h"

class xmlGlobalLoad
{
public:
	using allServerMap = std::map <std::string, std::shared_ptr <serverFile>>;
    xmlGlobalLoad ();
    ~xmlGlobalLoad ();
	int  xmlLoad (const char* szFile);
	int  appSLoad (rapidxml::xml_node<char>* pAppS);
	int  onceAppLoad (rapidxml::xml_node<char>* pApp, std::shared_ptr<appFile>& rApp);
	/*
	int  moduleSLoad (rapidxml::xml_node<char>* pModuleS, appFile& rApp);
	int  onceModuleLoad (rapidxml::xml_node<char>* pM, std::shared_ptr<moduleFile>& rM, appFile& rApp);
	*/
	int  serverSLoad (rapidxml::xml_node<char>*  pServerS, appFile& rApp);
	int  onceServerLoad (rapidxml::xml_node<char>* pS, std::shared_ptr<serverFile> rS, appFile& rApp);
	int  secondProcess ();
	serverFile*    getServerByHandle(const char* szHandle);
	serverFile*    getServerByListenEndPointName(const char* szEnName);
	allServerMap&  allServer ();
	int  perProc (rapidxml::xml_document<>& doc);
private:
	int  procEndPointS (rapidxml::xml_node<char>* pEndPointS);
	allServerMap  m_allServer;
};
#endif
