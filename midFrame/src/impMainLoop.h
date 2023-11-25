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
#include "mainLoop.h"

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
	int		init(int nArgC, char** argS, PhyCallback& info);
	int   initNetServer ();
	int procArgS(int nArgC, char** argS);
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
	uword  canRouteNum ();
	// void  setUpNum (uword v);
	void onLoopBegin(ServerIDType fId);
	void onLoopEnd(ServerIDType fId);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	void logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData);
	serializePackFunType  fromNetPack ();
	serializePackFunType  toNetPack ();
	uword  canUpRouteServerNum ();
	void  setCanUpRouteServerNum (uword v);
	uword  canDownRouteServerNum ();
	void  setCanDownRouteServerNum (uword v);
	loopHandleType   getOnceUpServer ();
	loopHandleType   getOnceDownServer ();
	uword  getAllCanRouteServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanUpServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanDownServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	bool   isRootApp ();
private:	
	uword  m_canDownRouteServerNum;
	uword  m_canUpRouteServerNum;
	serializePackFunType  m_toNetPack;
	serializePackFunType  m_fromNetPack;
	// uword  m_upNum;
    udword  m_delSendPackTime;
	msgMgr		m_defMsgInfoMgr;
	int			m_CurLoopNum;
	loopHandleType	m_gropId;
	std::unique_ptr<impLoop>	 m_loopS [LoopNum];
	std::unique_ptr<loopHandleType[]>	 m_canRouteServerIdS;
	typedef std::map<loopHandleType, std::string> tempLoopIdMap;
	tempLoopIdMap	m_tempLoopIdMap;
	PhyCallback  m_callbackS;
	ForLogicFun m_forLogic;
	std::unique_ptr<CModule[]>	 m_ModuleS;
	uword			m_ModuleNum;
};
#endif
