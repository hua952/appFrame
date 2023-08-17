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
	int OnLoopFrame();
	int processOncePack(packetHead* pPack);
	// int onWriteOncePack(packetHead* pPack);
	// void onFreePack(packetHead* pPack);

	int processLocalServerPack(packetHead* pPack);
	int processOtherAppPack(packetHead* pPack);
	int procProx(packetHead* pPack);
    NetTokenType	nextToken ();
	ServerIDType id();
	cTimerMgr&    getTimerMgr();
	serverNode*   getServerNode ();
	typedef std::map<uword, procRpcPacketFunType> MsgMap;
	typedef std::map<NetTokenType, packetHead*>  tokenMsgMap;
	iPackSave*    getIPackSave ();
	void  showFps ();
	fpsCount&  fpsC ();
	/*
	ITcpServer*  midTcpServer ();
	void  setMidTcpServer (ITcpServer* v);
	*/
	int processNetPackFun(ISession* session, packetHead* pack)override;
	void onAcceptSession(ISession* session, uqword userData)override;
	void onConnect(ISession* session, uqword userData)override;
	void onClose(ISession* session)override;
	void onWritePack(ISession* session, packetHead* pack)override;
	using serverSessionMap = std::map<ServerIDType, SessionIDType>;

	serverSessionMap&  serverSessionS ();
	ISession*  defSession ();
	void  setDefSession (ISession* v);
	ISession*  getServerSession(ServerIDType sid);
private:
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
