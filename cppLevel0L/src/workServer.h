#ifndef _workServer_h__
#define _workServer_h__
#include <memory>
#include <thread>
#include "deListMsgQue.h"
#include "mainLoop.h"
#include <set>
#include <map>
#include <unordered_map>
#include "typeDef.h"
#include "cTimer.h"
#include "fpsCount.h"
#include "impPackSave_map.h"

class workServer
{
public:
	typedef std::map<uword, procRpcPacketFunType> MsgMap;
    workServer ();
    ~workServer ();
	int  initWorkServer ();
	void run();
	void detach ();
	int onLoopBegin();
	int onLoopEnd();
	int onLoopFrame();

	void showFps ();

	udword  sleepSetp ();
	void  setSleepSetp (udword v);
	NetTokenType nextToken ();
	cTimerMgr&   getTimerMgr ();
	bool regMsg(uword uwMsgId, procRpcPacketFunType pFun);
	ubyte  serverId ();
	void  setServerId (ubyte v);
	bool pushPack (packetHead* pack); // Thread safety
	procRpcPacketFunType findMsg(uword uwMsgId);
	bool start();
	bool onFrame();
private:
	ubyte  m_serverId{0xff};
	MsgMap  m_MsgMap;
	uqword    m_frameNum{0};
	fpsCount  m_fpsC;
	udword  m_sleepSetp;
	// ServerIDType m_id;
	cTimerMgr          m_timerMgr;
    NetTokenType	   m_nextToken{0};
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;

	static void ThreadFun(workServer* pS);

	virtual int processOncePack(packetHead* pPack);
};
#endif
