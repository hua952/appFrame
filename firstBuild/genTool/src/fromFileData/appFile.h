#ifndef _appFile_h__
#define _appFile_h__
#include <memory>
#include <set>
#include <map>
#include <string>
#include <vector>
#include "comFun.h"

class serverFile;
class appFile
{
public:
	using  serverMap = std::map<std::string, std::shared_ptr<serverFile>>;
	using  serverOrder = std::vector <std::shared_ptr<serverFile>>;
    appFile ();
    ~appFile ();
	using  moduleFileNameSet = std::set<std::string>;
	using  argV = std::vector<std::string>;
	const char*  appName ();
	void  setAppName (const char* v);
	// moduleFileNameSet&  moduleFileNameS ();
	int  procId ();
	void  setProcId (int v);
	argV&  argS ();
	bool  detachServerS ();
	void  setDetachServerS (bool v);
	argV&  mainArgS ();
	const char*  mainLoopServer ();
	void  setMainLoopServer (const char* v);
	bool  haveServer ();
	bool  haveNetServer ();
	ubyte  netType ();
	void  setNetType (ubyte v);
	const char*  runWorkNum ();
	void  setRunWorkNum (const char* v);
	const char*  appGroupId ();
	void  setAppGroupId (const char* v);
	const char*  mainLoopGroupId ();
	void  setMainLoopGroupId (const char* v);
	uword  appGroupIdInt ();
	void  setAppGroupIdInt (uword v);

	serverFile*  findServer (const char* szName);
	serverFile*  mainServer ();
	serverMap&  serverS ();
	serverOrder&  orderS ();
    const char*  genPath ();
    void  setGenPath (const char* v);
    const char*  srcPath ();
    void  setSrcPath (const char* v);
private:
    std::unique_ptr <char[]>  m_srcPath;
    std::unique_ptr <char[]>  m_genPath;
	std::unique_ptr <char[]>  m_mainLoopGroupId;
	std::unique_ptr <char[]>  m_appGroupId;
	std::unique_ptr <char[]>  m_runWorkNum;
	ubyte  m_netType;
	std::unique_ptr <char[]>  m_mainLoopServer;
	argV  m_mainArgS;
	uword  m_appGroupIdInt;
	bool  m_detachServerS;
	argV  m_argS;
	int  m_procId;
	moduleFileNameSet  m_moduleFileNameS;
	std::unique_ptr <char[]>  m_appName;
	serverOrder  m_orderS;
	serverMap  m_serverS;
};
#endif
