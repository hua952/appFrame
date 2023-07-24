#ifndef _mArgMgr_h__
#define _mArgMgr_h__
#include <memory>
#include "packet.h"
#include "argMgr.h"

class mArgMgr: public argMgr
{
public:
    mArgMgr ();
    ~mArgMgr ();
	void onCmpKey (char* argv[])override;
	uword  savePackTag ();
private:
	uword  m_savePackTag;
};
#endif
