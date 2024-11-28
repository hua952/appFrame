#ifndef _appModuleGen_h__
#define _appModuleGen_h__
#include <memory>

class appFile;
class serverFile;
struct procRpcNode;
class appModuleGen
{
public:
    appModuleGen (appFile&  rAppData);
    ~appModuleGen ();
	const char*  srcDir ();
	void  setSrcDir (const char* v);
	const char*  homeDir ();
	void  setHomeDir (const char* v);
	int  startGen ();
private:
	int  genPackFun (serverFile& rServer);
	int  cmakeListsGen ();
	int  genOnLoopBegin (serverFile& rServer);
	int  genOnLoopEnd(serverFile& rServer);
	int  genOnLoopFrame(serverFile& rServer);
	int  onCreateChannelRetGen (serverFile& rServer);
	int  onWorkerInitGen (serverFile& rServer);
	int genServer (serverFile& rServer);
	int genExportFunH ();
	int genBashFile ();
	int genExportFunCpp ();
	int genWinDef ();
	int  versFileGen ();
	int procMsgReg (serverFile* pServer, const procRpcNode& rProcRpc, const char* szMsgDir);
	std::unique_ptr <char[]>  m_homeDir;
	std::unique_ptr <char[]>  m_srcDir;
	appFile&  m_appData;
};
#endif
