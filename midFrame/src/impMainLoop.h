#ifndef _impMainLoop_h_
#define _impMainLoop_h_
#include "mainLoop.h"
#include <memory>
#include <map>
#include "tJumpListNS.h"
#include <string>
#include "CModule.h"
#include "msgMgr.h"

//class CModule;
class PhyInfo
{
public:
	PhyInfo();
	int init(int nArgC, const char* argS[], PhyCallback& info);
	int procArgS(int nArgC, const char* argS[]);
	CModule* getModuleS (int& ModuleNum);
	PhyCallback& getPhyCallback();  
	ForLogicFun&  getForLogicFun();
	const char*   binHome ();
	static int createServer (const char* szName, loopHandleType serId, serverNode* pNode, frameFunType funFrame, void* arg);
	static int regMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun);
	static int removeMsg(loopHandleType handle, uword uwMsgId);
private:		
	PhyCallback  m_callbackS;
	ForLogicFun m_forLogic;
	void* m_handle;
	std::unique_ptr<char[]>      m_home;
	std::unique_ptr<CModule[]>	 m_ModuleS;
	uword			m_ModuleNum;
};

class impLoop;
class loopMgr
{
public:
	loopMgr();
	~loopMgr();
	impLoop*   getLoop(loopHandleType id);
	int createServer(const char* szName, loopHandleType serId,
			serverNode* pNode, frameFunType funFrame, void* arg);
	int		init(loopHandleType	procId, loopHandleType	gropId);
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
private:	
    udword  m_delSendPackTime;
	msgMgr						m_defMsgInfoMgr;
	int							m_CurLoopNum;
	loopHandleType	m_procId;
	loopHandleType	m_gropId;

	std::unique_ptr<impLoop>	 m_loopS [LoopNum];

	typedef std::map<loopHandleType, std::string> tempLoopIdMap;
	tempLoopIdMap	m_tempLoopIdMap;
};
#endif
