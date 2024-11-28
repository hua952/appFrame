#ifndef _configMgr_h__
#define _configMgr_h__
#include <memory>
#include "comFun.h"
#include "argMgr.h"

enum structBadyType
{
	structBadyTime_com = 0,
	structBadyTime_proto = 1
};
class configMgr:public argMgr 
{
public:
    configMgr ();
    ~configMgr ();

	void onCmpKey (char* argv[])override;

	const char*  defFile ();
	void  setDefFile (const char* v);
	bool  reProc ();
	void  setReProc (bool v);
	dword  structBadyType ();
	void  setStructBadyType (dword v);
	bool  installThreadDll ();
	void  setInstallThreadDll (bool v);
	const char*  projectDir ();
	void  setProjectDir (const char* v);
	const char*  projectName ();
	void  setProjectName (const char* v);
    const char*  frameInstallPath ();
    void  setFrameInstallPath (const char* v);
	const char*  projectInstallDir ();
	void  setProjectInstallDir (const char* v);
private:
	std::unique_ptr <char[]>  m_projectInstallDir;
    std::unique_ptr <char[]>  m_frameInstallPath;
	std::unique_ptr <char[]>  m_projectName;
	std::unique_ptr <char[]>  m_projectDir;
	std::unique_ptr<char[]>  m_defFile;
	dword  m_structBadyType;
	bool  m_installThreadDll;
	bool  m_reProc;
};
#endif
