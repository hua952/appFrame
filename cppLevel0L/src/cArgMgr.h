#ifndef _cArgMgr_h__
#define _cArgMgr_h__
#include <memory>
#include "argMgr.h"

class cArgMgr:public argMgr
{
public:
    cArgMgr ();
    ~cArgMgr ();
	void onCmpKey (char* argv[])override;
	uword  logLevel ();
	void  setLogLevel (uword v);
	bool  detachServerS ();
	void  setDetachServerS (bool v);
	const char*  logFile ();
	void  setLogFile (const char* v);
	const char*  workDir ();
	void  setWorkDir (const char* v);
private:
	std::unique_ptr<char[]>  m_workDir;
	std::unique_ptr<char[]>  m_logFile;
	bool  m_detachServerS;
	uword  m_logLevel;
};
#endif
