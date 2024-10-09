#ifndef _logicWorkerMgr_h__
#define _logicWorkerMgr_h__
#include <memory>
#include "mainLoop.h"
#include "arrayMap.h"

class logicWorker;
class logicWorkerMgr
{
public:
	using defProcMap = arrayMap<uword, uword>;
    logicWorkerMgr ();
    ~logicWorkerMgr ();
	int initLogicWorkerMgr (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS);
	int processOncePack(ubyte serverIndex, packetHead* pPack);
	virtual int initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS);
	ForLogicFun*   forLogicFun ();
	uword  allServerNum ();
	void  setAllServerNum (uword v);
	logicWorker** allServers ();
	logicWorker* findServer (ubyte serverIndex);
	bool findDefProc (uword msgId, ubyte& appGroup, ubyte& serverGroup);
	static logicWorkerMgr& getMgr();
	static logicWorkerMgr* s_mgr;
protected:
	uword  m_allServerNum{0};
	ForLogicFun*     m_pForLogicFun;
	std::unique_ptr<logicWorker*[]>	  m_allServers;
	defProcMap  m_defProcMap;
private:
};
#endif
