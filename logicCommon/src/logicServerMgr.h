#ifndef _logicServerMgr_h__
#define _logicServerMgr_h__
#include <memory>
#include "mainLoop.h"
#include <set>
#include <map>
#include "ISession.h"
#include "staticArrayMap.h"

class logicServer;
class logicServerMgr
{
public:
	using  msgDefProcMap = staticArrayMap<msgIdType, loopHandleType>;
	using  serverOpenMap = staticArrayMap<loopHandleType, loopHandleType>;
	using  logicServerPair = std::pair<std::unique_ptr< logicServer* []>, loopHandleType>;
	using  logicMuServerPairS = std::pair<std::unique_ptr<logicServerPair []>, loopHandleType>;
    logicServerMgr ();
    ~logicServerMgr ();
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
	virtual dword afterLoadLogic(int nArgC, char** argS, ForLogicFun* pForLogic);
	int   initMsgDefProc (loopHandleType* pBuf, int num);
	int   procArgS (int nArgC, char** argS);
	ubyte  serverNum ();
	void   setServerNum (ubyte ubyNum);
	logicServer*  findServer(serverIdType	serverId);
	int    initModelS (const char* szModelS);
	ForLogicFun&     forLogicFunSt ();
	logicMuServerPairS*  logicMuServerPairSPtr ();
	serverIdType  netServerTmp ();
	void          setNetServerTmp (serverIdType serverId);
	logicServer**       getNetServerS (uword& num);
	loopHandleType  getMsgDefProcTmp (msgIdType msgId);
	logicServerPair*  getServerArray (serverIdType	serverId);
	static logicServerMgr&  getMgr();
	const char*  homeDir ();
	void  setHomeDir (const char* v);
protected:
	std::unique_ptr<char[]>  m_homeDir;
	static logicServerMgr* s_Mgr;
	std::unique_ptr<serverOpenMap>	m_serverOpenS;
	std::unique_ptr<msgDefProcMap>	m_msgDefProcS;
	serverIdType  m_netServerTmp;
	logicMuServerPairS m_muServerPairS[c_serverLevelNum];
	std::unique_ptr<char[]>    m_modelName;
	ForLogicFun*     m_pForLogicFun;
	ubyte  m_serverNum;
};
#endif
