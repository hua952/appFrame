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
private:
	bool  m_installThreadDll;
	dword  m_structBadyType;
	bool  m_reProc;
	std::unique_ptr<char[]>  m_defFile;
};
#endif
