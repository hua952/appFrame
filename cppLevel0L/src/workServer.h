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
    workServer ();
    ~workServer ();
	int  initWorkServer (ServerIDType id);
	ServerIDType id();
	void run();

	virtual int onLoopBegin();
	virtual int onLoopEnd();
	virtual int onLoopFrame();

	void showFps ();
private:
	uqword    m_frameNum;
	fpsCount  m_fpsC;
	udword  m_sleepSetp;
	ServerIDType m_id;
	cTimerMgr          m_timerMgr;
    NetTokenType	   m_nextToken;
	std::unique_ptr<std::thread> m_pTh;
	deListMsgQue	m_slistMsgQue;

	static void ThreadFun(server* pS);

	bool start();
	bool onFrame();
	virtual int processOncePack(packetHead* pPack);
	int processLocalServerPack(packetHead* pPack);
};
#endif
