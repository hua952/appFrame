#ifndef _impMainLoop_h_
#define _impMainLoop_h_
#include "mainLoop.h"
#include <memory>
#include <map>
#include "tJumpListNS.h"
#include <string>
#include "CModule.h"
#include "msgMgr.h"
#include "ITcpServer.h"

class impLoop;
class loopMgr
{
public:
	loopMgr();
	~loopMgr();
	impLoop*   getLoop(loopHandleType id);
	impLoop*   getLoopByIndex(uword index);
	int createServer(const char* szName, loopHandleType serId,
			serverNode* pNode, frameFunType funFrame, void* arg);
	int		init(int nArgC, const char* argS[], PhyCallback& info);
	int   initNetServer ();
	int procArgS(int nArgC, const char* argS[]);
	loopHandleType	procId();
	loopHandleType	gropId();
	void			setProcId(loopHandleType proc);
	void			setGropId(loopHandleType grop);
	int		createServerS();
	int getAllLoopAndStart(serverNode* pBuff, int nBuffNum);
	msgMgr&	defMsgInfoMgr (); // Thread safety
	udword  delSendPackTime ();
	PhyCallback& getPhyCallback();
	ForLogicFun&  getForLogicFun();
	uword  upNum ();
	void  setUpNum (uword v);
private:	
	uword  m_upNum;
    udword  m_delSendPackTime;
	msgMgr						m_defMsgInfoMgr;
	int							m_CurLoopNum;
	loopHandleType	m_gropId;
	std::unique_ptr<impLoop>	 m_loopS [LoopNum];
	typedef std::map<loopHandleType, std::string> tempLoopIdMap;
	tempLoopIdMap	m_tempLoopIdMap;
	PhyCallback  m_callbackS;
	ForLogicFun m_forLogic;
	std::unique_ptr<CModule[]>	 m_ModuleS;
	uword			m_ModuleNum;
};
#endif
