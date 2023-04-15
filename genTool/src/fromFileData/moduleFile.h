#ifndef _moduleFile_h__
#define _moduleFile_h__
#include <memory>
#include <map>
#include <string>

class serverFile;
class moduleFile
{
public:
	using  serverMap = std::map<std::string, std::shared_ptr<serverFile>>;
    moduleFile ();
    ~moduleFile ();
	const char*  moduleName ();
	void  setModuleName (const char* v);
	serverMap&  serverS ();
private:
	serverMap  m_serverS;
	std::unique_ptr <char[]>  m_moduleName;
};
#endif
