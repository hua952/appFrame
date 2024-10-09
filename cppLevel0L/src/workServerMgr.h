#ifndef _workServerMgr_h__
#define _workServerMgr_h__
#include <memory>
#include <set>
#include <mutex>
#include "mainLoop.h"

extern allocPackFT  allocPack;
extern freePackFT  freePack;
typedef  void (*outMemLeakFunT)(std::ostream& os);
extern outMemLeakFunT  outMemLeak;

class workServer;
class workServerMgr
{
public:
	typedef void (*beforeUnloadFunT)();
	using  runThreadIdSet = std::set<loopHandleType>;
    workServerMgr ();
    ~workServerMgr ();
	int initWorkServerMgr (int cArg, char** argS, int cDefArg, char** defArgS);
	ForLogicFun&  getForLogicFun();
	void afterAllLoopEndBeforeExitApp  ();
	workServer*  getServer(loopHandleType id);
	int  runThNum (char* szBuf, int bufSize);
	void  incRunThNum (loopHandleType pThis);
	void  subRunThNum (loopHandleType pThis);
	runThreadIdSet&  runThreadIdS ();
	onRecPacketFT     onRecPacketFun ();

	onFrameLagicFT  onFrameLogicFun ();
	onLoopEndFT  onLoopEnd ();
	onLoopBeginFT  onLoopBegin ();
private:
	std::unique_ptr<workServer[]>	  m_allServers;
	uword  m_allServerNum{0};
	void* m_handle{nullptr};
	onFrameLagicFT  m_fnOnFrameLogic;
	onLoopEndFT  m_fnOnLoopEnd;
	onLoopBeginFT  m_fnOnLoopBegin;
	afterLoadFunT  m_fnAfterLoad;
	beforeUnloadFunT	m_onUnloadFun;
	onRecPacketFT     m_fnRecPacket;
	ForLogicFun m_forLogic;
	runThreadIdSet  m_runThreadIdS;
	std::mutex m_mtxRunThNum;
};
#endif
