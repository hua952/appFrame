/*
#ifndef _moduleFile_h__
#define _moduleFile_h__
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "typeDef.h"

class serverFile;
class moduleFile
{
public:
	using  serverMap = std::map<std::string, std::shared_ptr<serverFile>>;
	using  serverOrder = std::vector <std::shared_ptr<serverFile>>;
    moduleFile ();
    ~moduleFile ();
	const char*  moduleName ();
	void  setModuleName (const char* v);
	serverFile*  findServer (const char* szName);
	serverFile*  findServerByTmpNum (uword num);
	serverMap&  serverS ();
	const char*  appName ();
	void  setAppName (const char* v);
	serverOrder&  orderS ();
private:
	serverOrder  m_orderS;
	std::unique_ptr <char[]>  m_appName;
	std::unique_ptr <char[]>  m_moduleName;
	serverMap  m_serverS;
};
#endif
*/
