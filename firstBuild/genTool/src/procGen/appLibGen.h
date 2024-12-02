#ifndef _appLibGen_h__
#define _appLibGen_h__
#include <memory>

class appFile;
class serverFile;
class appLibGen
{
public:
    appLibGen (appFile&  rAppData);
    ~appLibGen ();
	const char*  homeDir ();
	void  setHomeDir (const char* v);
	const char*  srcDir ();
	void  setSrcDir (const char* v);

	int genWorkerMgrH ();
	int genWorkerMgrCpp ();

	int  cmakeListsGen ();

	int  genWorkerH (serverFile& rServer);
	int  genWorkerCpp (serverFile& rServer);

	int  genPackFun (serverFile& rServer);
	int  genMain ();
	int  startGen ();
private:
	int batFileGen ();
	int writeUnixDep ();
	int writeWinDep ();
	int writeMain ();

	std::unique_ptr <char[]>  m_srcDir;
	std::unique_ptr <char[]>  m_homeDir;
	appFile&  m_appData;
};
#endif
