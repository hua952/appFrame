#ifndef _appFile_h__
#define _appFile_h__
#include <memory>
#include <set>
#include <string>

class moduleFile;
class appFile
{
public:
    appFile ();
    ~appFile ();
	using  moduleFileNameSet = std::set<std::string>;
	const char*  appName ();
	void  setAppName (const char* v);
	moduleFileNameSet&  moduleFileNameS ();
private:
	moduleFileNameSet  m_moduleFileNameS;
	std::unique_ptr <char[]>  m_appName;
};
#endif
