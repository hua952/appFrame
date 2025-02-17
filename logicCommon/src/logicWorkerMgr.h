#ifndef _logicWorkerMgr_h__
#define _logicWorkerMgr_h__
#include <memory>
#include "mainLoop.h"
#include "arrayMap.h"

class logicWorker;
class logicWorkerMgr
{
public:
	struct pSerializePackFunType3
	{
		serializePackFunType f[3];
	};

	class pSerializePackFunType3Cmp 
	{
	public:
		bool operator () (const pSerializePackFunType3& a, const pSerializePackFunType3& b) const;
	};

	using defProcMap = arrayMap<uword, uword>;
	using  msgSerFunSet = arraySet<pSerializePackFunType3, pSerializePackFunType3Cmp>;


	int fromNetPack (netPacketHead* pN, pPacketHead& pNew);
	int toNetPack (netPacketHead* pN, pPacketHead& pNew);

    logicWorkerMgr ();
    ~logicWorkerMgr ();
	int initLogicWorkerMgr (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS, char* taskBuf, int taskBufSize);
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
	pSerializePackFunType3* sGetNode (udword msgId);
	uword  m_allServerNum{0};
	ForLogicFun*     m_pForLogicFun;
	std::unique_ptr<logicWorker*[]>	  m_allServers;
	defProcMap  m_defProcMap;
private:
	msgSerFunSet s_SerFunSet;
};
#endif
