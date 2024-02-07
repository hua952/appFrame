#ifndef _appFile_h__
#define _appFile_h__
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "comFun.h"

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
	bool  detachServerS ();
	void  setDetachServerS (bool v);
	argV&  mainArgS ();
	const char*  mainLoopServer ();
	void  setMainLoopServer (const char* v);
	bool  haveServer ();
	ubyte  netType ();
	void  setNetType (ubyte v);
private:
	ubyte  m_netType;
	std::unique_ptr <char[]>  m_mainLoopServer;
	argV  m_mainArgS;
	bool  m_detachServerS;
	argV  m_argS;
	int  m_procId;
	moduleFileNameSet  m_moduleFileNameS;
	std::unique_ptr <char[]>  m_appName;
};
#endif
