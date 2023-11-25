#ifndef _impLoog_h_
#define _impLoog_h_
#include "loop.h"
#include "arrayMap.h"
#include "mainLoop.h"
#include <memory>
#include "cTimer.h"
#include <map>
#include "impPackSave_map.h"
#include "fpsCount.h"
#include "ITcpServer.h"
#include "comTcpNet.h"
#include <set>

class impLoop:public comTcpNet
{
public:
	impLoop();
	~impLoop();
	int init(const char* szName, ServerIDType id, serverNode* pNode, frameFunType funOnFrame = nullptr, void* argS = nullptr);
	void clean();

	bool regMsg(uword uwMsgId, procRpcPacketFunType pFun);
	bool	removeMsg(uword uwMsgId);
	procRpcPacketFunType findMsg(uword uwMsgId);
	const char* name();

	int onLoopBegin();
	int onLoopEnd();
	int onLoopFrame();
	int processOncePack(packetHead* pPack);

	int processLocalServerPack(packetHead* pPack);
	int processOtherAppToMePack(ISession* session, packetHead* pPack);
	int processOtherAppPack(packetHead* pPack);
	int processAllGatePack(packetHead* pPack);
	int procProx(packetHead* pPack);
	int forward(packetHead* pPack);
    NetTokenType	nextToken ();
	ServerIDType id();
	cTimerMgr&    getTimerMgr();
	serverNode*   getServerNode ();
	typedef std::map<uword, procRpcPacketFunType> MsgMap;
	typedef std::map<NetTokenType, packetHead*>  tokenMsgMap;
	iPackSave*    getIPackSave ();
	void  showFps ();
	fpsCount&  fpsC ();
	int clonePackToOtherNetThread (packetHead* pack);	
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void onAcceptSession(ISession* session, void* userData)override;
	void onConnect(ISession* session, void* userData)override;
	void onClose(ISession* session)override;
	void onWritePack(ISession* session, packetHead* pack)override;
	using serverSessionMap = std::map<ServerIDType, SessionIDType>;
	using processSessionMap = std::set<uqword>;

	serverSessionMap&  serverSessionS ();
	ISession*  defSession ();
	void  setDefSession (ISession* v);
	ISession*  getSession(SessionIDType sid);
	ISession*  getServerSession(ServerIDType sid);

	static const auto c_userQuewordNum = 2;
	int regRoute (ServerIDType objServer, SessionIDType sessionId, udword onlyId);
	bool  canUpRoute ();
	void  setCanUpRoute (bool v);
	bool  canDownRoute ();
	void  setCanDownRoute (bool v);
	processSessionMap&  processSessionS ();
	ISession*  getProcessSession(ServerIDType pid);
	ISession*  getProcessSessionByFullServerId(ServerIDType fullId);
private:
	processSessionMap  m_processSessionS;
	bool  m_canDownRoute;
	bool  m_canUpRoute;
	ISession*  m_defSession;
	serverSessionMap  m_serverSessionS;
	uqword    m_frameNum;
	fpsCount  m_fpsC;
	std::unique_ptr<impPackSave_map>	 m_pImpPackSave_map;
	iPackSave*							 m_packSave;
    tokenMsgMap  m_tokenMsgS;
	cTimerMgr          m_timerMgr;
	serverNode		   m_serverNode;
	MsgMap	m_MsgMap;
	frameFunType	m_funOnFrame;
	void*			m_pArg;
	ServerIDType m_id;
	std::unique_ptr<char[]>	 m_name;
};

#endif
