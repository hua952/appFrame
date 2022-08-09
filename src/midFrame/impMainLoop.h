#ifndef _impMainLoop_h_
#define _impMainLoop_h_
#include "mainLoop.h"
#include <memory>
#include <map>
#include "tJumpListNS.h"
#include <string>

class CModule;
class PhyInfo
{
public:
	PhyInfo();
	int init(int nArgC, const char* argS[], PhyCallback& info);
	int procArgS(int nArgC, const char* argS[]);
	CModule* getModuleS (int& ModuleNum);
	PhyCallback& getPhyCallback();  
	ForLogicFun&  getForLogicFun();
	static loopHandleType createLoop(const char* szName,/* uword uwId,*/ frameFunType funFrame, void* arg);
	static int regMsg(loopHandleType handle, uword uwMsgId, procPacketFunType pFun);
	static int removeMsg(loopHandleType handle, uword uwMsgId);
private:		
	PhyCallback  m_callbackS;
	ForLogicFun m_forLogic;
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
	loopHandleType createLoop(const char* szName,/* uword uwId,*/ frameFunType funFrame, void* arg);
	int		init(loopHandleType	procId, loopHandleType	gropId);
	loopHandleType	procId();
	loopHandleType	gropId();
	int		createServerS();
	int getAllLoopAndStart(loopHandleType* pBuff, int nBuffNum);
	static const auto c_MaxLoopNum = LoopNum;
private:	
	loopHandleType	m_procId;
	loopHandleType	m_gropId;
	std::unique_ptr<impLoop[]>	 m_loopS;
	int							 m_CurLoopNum;
	//typedef std::map<std::string, loopHandleType> tempLoopNameMap;
	//tempLoopNameMap& getTempLoopNameMap();
	//tempLoopNameMap m_tempLoopMap;
	typedef std::map<loopHandleType, std::string> tempLoopIdMap;
	tempLoopIdMap	m_tempLoopIdMap;
};
#endif
