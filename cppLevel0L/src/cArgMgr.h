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
private:
	uword  m_logLevel;
};
#endif
