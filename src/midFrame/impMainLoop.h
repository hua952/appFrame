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
	PhyCallback& getPhyCallback();  
	ForLogicFun&  getForLogicFun();
	static loopHandleType createLoop(const char* szName, frameFunType funFrame, void* arg);
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
	loopHandleType createLoop(const char* szName, frameFunType funFrame, void* arg);
	typedef std::map<std::string, impLoop* > tempLoopMap;

	tempLoopMap& getTempLoopMap();
private:	
	tempLoopMap m_tempLoopMap;
};
#endif
