#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "impMainLoop.h"
#include "tSingleton.h"
#include "strFun.h"
#include "CModule.h"
#include "impLoop.h"
#include "myAssert.h"
#include "mLog.h"
#include "impLoop.h"
#include "mArgMgr.h"
#include <iostream>
#include <cmath>
#include "modelLoder.h"
#include "comMsgMsgId.h"

int  InitMidFrame(int nArgC, const char* argS[],PhyCallback* pCallbackS)
{
	int nRet = 0;
	do {
		tSingleton <mArgMgr>::createSingleton ();
		auto& rArgS = tSingleton<mArgMgr>::single ();
		nRet = rArgS.procArgS (nArgC, argS);
		myAssert (0 == nRet);
		if (nRet) {
			mError ("rArgS.procArgS error nRet = "<<nRet);
			break;
		}
		tSingleton<loopMgr>::createSingleton();
		auto& rMgr = tSingleton<loopMgr>::single();
		nRet = rMgr.init(nArgC, argS, *pCallbackS);
		if (nRet) {
			mError("loopMgr init error nRet = "<<nRet);
			break;
		}
	} while (0);
	return nRet;
}

int getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	//mInfo (" At then begin of getAllLoopAndStart");
	auto& rMgr = tSingleton<loopMgr>::single();
	return rMgr.getAllLoopAndStart(pBuff, nBuffNum);
}

static int sRegMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun)
{
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->regMsg(uwMsgId, pFun);
	} while(0);
	return nRet;
}

static int sRemoveMsg(loopHandleType handle, uword uwMsgId)
{
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pLoop = rMgr.getLoop(handle);
	do 
	{
		if (NULL == pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->removeMsg(uwMsgId);
	} while(0);
	return nRet;
}

static int sAddComTimer(loopHandleType pThis, udword firstSetp, udword udwSetp,
		ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	int nL = pThis;
	//mTrace ("at then begin of sAddComTimer pThis = "<<nL);
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pTH = rMgr.getLoop(pThis);
	//mTrace ("at then begin of sAddComTimer pTH = "<<pTH);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}
static sendPackToLoopFT g_sendPackToLoopFun = nullptr;

static int sSendChMsg (packetHead* pack)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}

int midSendPackToLoopFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<loopMgr>::single ();
	/*
	auto curHandleFun = tSingleton<loopMgr>::single ().getPhyCallback().fnGetCurServerHandle;
	auto curHandle = curHandleFun ();
	if (curHandle !=  pN->ubySrcServId) {
		mTrace ("curHandle = "<<curHandle<<"pN->ubySrcServId = "<<pN->ubySrcServId);
	}
	myAssert (pN->ubySrcServId == curHandle);
	*/
	auto pS = rMgr.getLoop(pN->ubySrcServId);
	myAssert (pS);
	bool bIsRet = NIsRet(pN);
	if (!bIsRet) {
		pN->dwToKen = pS->nextToken ();
	}
	auto bInOnceProc =  packInOnceProc(pack);
	do {
		auto objSer = pN->ubyDesServId;
		if (!bInOnceProc) {
			auto upN = rMgr.upNum ();
			auto s = rand () % upN;
			objSer = rMgr.getLoopByIndex (s)->id();
		}
		// fnPushPackToLoop
		auto fnPushPackToLoop = tSingleton<loopMgr>::single().getPhyCallback().fnPushPackToLoop;
		fnPushPackToLoop (objSer, pack);
	} while (0);
	return nRet;
}
static iRpcInfoMgr* sGetIRpcInfoMgr()
{
	return &tSingleton<loopMgr>::single().defMsgInfoMgr();
}

int    sRegRpc(msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer)
{
	auto& rMgr = tSingleton<loopMgr>::single().defMsgInfoMgr();
	return rMgr.regRpc (askId, retId, askDefProcSer, retDefProcSer);
}

serverIdType sGetDefProcServerId (msgIdType msgId)
{
	auto& rMgr = tSingleton<loopMgr>::single().defMsgInfoMgr();
	return rMgr.getDefProcServerId (msgId);
}

static allocPackFT g_allocPackFun = nullptr;
packetHead* sAllocPack(udword udwSize)
{
	auto pPack = g_allocPackFun (udwSize);
	pPack->sessionID = EmptySessionID;
	return pPack;
}

static freePackFT g_freePackFun = nullptr;
static void sFreePack (packetHead* pack)
{
	auto pN = P2NHead (pack);
	g_freePackFun (pack);
}

static int sCreateServer (const char* szName, loopHandleType serId, 
		serverNode* pNode, frameFunType funFrame, void* arg)
{
	return tSingleton<loopMgr>::single().createServer(szName, serId,
			pNode, funFrame, arg);
}
typedef void		(*freePackFT)(packetHead* pack);

static void regSysRpcS (const ForLogicFun* pForLogic)
{
	/*
	auto regRpc = pForLogic->fnRegRpc;
	regRpc (comMsg2FullMsg(comMsgMsgId_addChannelAsk), comMsg2FullMsg(comMsgMsgId_addChannelRet), c_emptyLoopHandle, c_emptyLoopHandle);
    regRpc (comMsg2FullMsg(comMsgMsgId_delChannelAsk), comMsg2FullMsg(comMsgMsgId_delChannelRet), c_emptyLoopHandle, c_emptyLoopHandle);
    regRpc (comMsg2FullMsg(comMsgMsgId_listenChannelAsk), comMsg2FullMsg(comMsgMsgId_listenChannelRet), c_emptyLoopHandle, c_emptyLoopHandle);
    regRpc (comMsg2FullMsg(comMsgMsgId_quitChannelAsk), comMsg2FullMsg(comMsgMsgId_quitChannelRet), c_emptyLoopHandle, c_emptyLoopHandle);
    regRpc (comMsg2FullMsg(comMsgMsgId_sendToChannelAsk), comMsg2FullMsg(comMsgMsgId_sendToChannelRet), c_emptyLoopHandle, c_emptyLoopHandle);
	*/
}

int loopMgr::init(int nArgC, const char* argS[], PhyCallback& info)
{
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	g_allocPackFun = info.fnAllocPack;
	g_freePackFun = info.fnFreePack;
	g_sendPackToLoopFun = info.fnSendPackToLoop;
	forLogic.fnCreateLoop = sCreateServer;
	forLogic.fnAllocPack = sAllocPack; // info.fnAllocPack;
	forLogic.fnFreePack = sFreePack; //  info.fnFreePack;
	forLogic.fnRegMsg = sRegMsg;
	forLogic.fnSendPackToLoop =  midSendPackToLoopFun;
	forLogic.fnLogMsg = info.fnLogMsg;
	forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
	forLogic.fnNextToken = info.fnNextToken;
	// forLogic.fnGetIRpcInfoMgr = sGetIRpcInfoMgr;
	forLogic.fnPushToCallStack = info.fnPushToCallStack;
	forLogic.fnPopFromCallStack = info.fnPopFromCallStack;
	forLogic.fnLogCallStack = info.fnLogCallStack;
	forLogic.fnRegRpc = sRegRpc;
	forLogic.fnGetDefProcServerId = sGetDefProcServerId;
	//forLogic.fnRemoveMsg = removeMsg;
	int nR = 0;
	int nRet = 0;
	nR= procArgS(nArgC, argS);
	do
	{
		if(0 != nRet) {
			mError(" procArgS error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = initNetServer ();
		if (nR) {
			mError("initNetServer  return error nR = "<<nR);
			nRet = 2;
			break;
		}
		auto& rModuleS = m_ModuleS;
		regSysRpcS (&forLogic);
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			rM.load(nArgC, argS, &forLogic );
		}
	}while(0);
	return nRet;
}

int loopMgr::procArgS(int nArgC, const char* argS[])
{
	using tempModuleInfo = std::set<std::string>;
	tempModuleInfo moduleS;
	for(int i = 1; i < nArgC; i++)
	{
		auto pC = argS[i];
		auto nL = strlen(pC);
		std::unique_ptr<char[]>	 name = std::make_unique<char[]>(nL + 1);
		strcpy(name.get(), pC);
		const int c_BuffNum = 3;
		char* buff[c_BuffNum];
		auto nR = strR(name.get(), '=', buff, c_BuffNum);
		if(2 == nR)
		{
			if(0 == strcmp(buff[0], "addLogic")) {
				auto bI = moduleS.insert(buff[1]);
				myAssert (bI.second);
			}
		}
	}
	auto nS = moduleS.size();
	m_ModuleNum = nS;
	m_ModuleS = std::make_unique<CModule[]>(nS);
	auto i = 0;
	auto& forLogic = getForLogicFun();
	for(auto it = moduleS.begin(); moduleS.end() != it; ++it)
	{
		auto& rM = m_ModuleS[i++];
		rM.init(it->c_str());
	}
	return 0;
}
 
loopMgr::loopMgr():m_CurLoopNum(0), m_gropId(0)
{
	// m_netLibHandle = nullptr;
    m_delSendPackTime = 5000;
	m_upNum = 1;
}

loopMgr::~loopMgr()
{
}

udword  loopMgr::delSendPackTime ()
{
	return m_delSendPackTime;
}

int loopMgr::createServerS()
{
	int nRet = 0;
	
	return nRet;
}

int loopMgr::createServer(const char* szName, loopHandleType serId,  serverNode* pNode, frameFunType funFrame, void* arg)
{
	loopHandleType pRet = c_emptyLoopHandle;
	loopHandleType pid = 0;
	loopHandleType sid = 0;
	fromHandle (serId, pid, sid);
	auto& p = m_loopS[sid];
	myAssert (!p);
	p = std::make_unique<impLoop> ();
	p->init(szName, serId, pNode, funFrame, arg);
	m_CurLoopNum++;
	mInfo ("createServer szName = "<<szName<<" pid = "<<pid<<" sid = "<<sid<<" m_CurLoopNum = "<<m_CurLoopNum);
	pRet = serId;
	return pRet;
}

msgMgr&	loopMgr::defMsgInfoMgr ()
{
	return m_defMsgInfoMgr;
}

loopHandleType	loopMgr::procId()
{
	return tSingleton<mArgMgr>::single().procId();
}

loopHandleType	loopMgr::gropId()
{
	return m_gropId;
}

void	loopMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

impLoop*  loopMgr::getLoop(loopHandleType id)
{
	loopHandleType pid = 0;
	loopHandleType lid = 0;
	fromHandle (id, pid, lid);
	return m_loopS[lid].get();
}

impLoop*   loopMgr :: getLoopByIndex(uword index)
{
    impLoop*    nRet = nullptr;
    do {
		nRet = m_loopS[index].get();
    } while (0);
    return nRet;
}

int regSysProcPacketFun (regMsgFT fnRegMsg, serverIdType handle);
int loopMgr::getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	auto pid = procId ();
	int nRet = 0;
	for (auto i = 0; i < LoopNum && nRet < nBuffNum; i++)
	{
		auto &p = m_loopS[i];
		if (!p) {
			continue;
		}
		auto &node = pBuff[nRet++];
		auto pNode = p->getServerNode ();
		if (pNode) {
			node = *pNode;
		} else {
			node.listenerNum = 0;
			node.connectorNum = 0;
		}
		auto sid = p->id ();
		node.handle = sid;//toHandle (pid, id);
		regSysProcPacketFun (getForLogicFun().fnRegMsg, sid);
	}

	mTrace ("at the end nRet = "<<nRet);
	return nRet;
}

PhyCallback&   loopMgr:: getPhyCallback()
{
	return m_callbackS;
}

ForLogicFun&  loopMgr::getForLogicFun()
{
	return  m_forLogic;
}

int    loopMgr:: initNetServer ()
{
    int    nRet = 0;
    do {
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto midNetLibName = rArgS.midNetLibName ();
		std::unique_ptr<char[]> binH;
		getCurModelPath(binH);
		std::string strPath = binH.get (); 
		strPath += midNetLibName;
		auto& rC = getPhyCallback();
		nRet = initComTcpNet (strPath.c_str(), rC.fnAllocPack, rC.fnFreePack, rC.fnLogMsg);
		if (nRet) {
			mError ("initComTcpNet error nRet = "<<nRet<<" strPath = "
					<<strPath.c_str());
			break;
		}
    } while (0);
    return nRet;
}

uword loopMgr :: upNum ()
{
    return m_upNum;
}

void  loopMgr :: setUpNum (uword v)
{
    m_upNum = v;
}

void loopMgr :: logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnConnect ();
			fu (fId, sessionId, userData);
		}
    } while (0);
}

void  loopMgr:: logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnAccept ();
			fu (fId, sessionId, userData);
		}
    } while (0);
}

void  loopMgr:: onLoopBegin(ServerIDType fId)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopBegin ();
			fu (fId);
		}
    } while (0);
}

void  loopMgr:: onLoopEnd(ServerIDType fId)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopEnd ();
			fu (fId);
		}
    } while (0);
}

