#ifndef _appFile_h__
#define _appFile_h__
#include <memory>
#include <set>
#include <string>
#include <vector>

class moduleFile;
class appFile
{
public:
    appFile ();
    ~appFile ();
	using  moduleFileNameSet = std::set<std::string>;
	using  argV = std::vector<std::string>;
	const char*  appName ();
	void  setAppName (const char* v);
	moduleFileNameSet&  moduleFileNameS ();
	int  procId ();
	void  setProcId (int v);
	argV&  argS ();
private:
	argV  m_argS;
	int  m_procId;
	moduleFileNameSet  m_moduleFileNameS;
	std::unique_ptr <char[]>  m_appName;
};
#endif
