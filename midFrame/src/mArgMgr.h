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
	const char*  midNetLibName ();
	void  setMidNetLibName (const char* v);
	loopHandleType  procId ();
	void  setProcId (loopHandleType v);
	udword  packTokenTime ();
	void  setPackTokenTime (udword v);
	const char*  workDir ();
	void  setWorkDir (const char* v);
	bool  dumpMsg ();
	void  setDumpMsg (bool v);
private:
	bool  m_dumpMsg;
	std::unique_ptr<char[]>  m_workDir;
	udword  m_packTokenTime;
	loopHandleType  m_procId;
	std::unique_ptr<char[]>  m_midNetLibName;
	uword  m_savePackTag;
};
#endif
