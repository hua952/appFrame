#ifndef _impLoog_h_
#define _impLoog_h_
#include "loop.h"
#include "arrayMap.h"
#include "mainLoop.h"
#include <memory>
#include "cTimer.h"
#include <map>

class impLoop
{
public:
	impLoop();
	~impLoop();
	int init(const char* szName, ServerIDType id, serverNode* pNode, frameFunType funOnFrame = nullptr, void* argS = nullptr);
	void clean();

	bool regMsg(uword uwMsgId, procPacketFunType pFun);
	bool	removeMsg(uword uwMsgId);
	procPacketFunType findMsg(uword uwMsgId);
	const char* name();
	int OnLoopFrame();
	int processOncePack(packetHead* pPack);
	int onWriteOncePack(packetHead* pPack);
	ServerIDType id();
	cTimerMgr&    getTimerMgr();
	serverNode*   getServerNode ();
	//void			setId(ModelIDType id);
	//typedef arrayMap<uword, procPacketFunType> MsgMap;
	typedef std::map<uword, procPacketFunType> MsgMap;
private:
	cTimerMgr          m_timerMgr;
	serverNode		   m_serverNode;
	MsgMap	m_MsgMap;
	frameFunType	m_funOnFrame;
	void*			m_pArg;
	ServerIDType m_id;
	std::unique_ptr<char[]>	 m_name;
};

#endif
