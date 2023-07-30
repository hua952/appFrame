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

class impLoop
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
	int onWriteOncePack(packetHead* pPack);
	void onFreePack(packetHead* pPack);
    NetTokenType	nextToken ();
	ServerIDType id();
	cTimerMgr&    getTimerMgr();
	serverNode*   getServerNode ();
	typedef std::map<uword, procRpcPacketFunType> MsgMap;
	typedef std::map<NetTokenType, packetHead*>  tokenMsgMap;
	iPackSave*    getIPackSave ();
	void  showFps ();
	fpsCount&  fpsC ();
private:
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
