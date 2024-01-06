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
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "modelLoder.h"
#include "comMsgMsgId.h"
#include <vector>

int  InitMidFrame(int nArgC, char** argS, PhyCallback* pCallbackS)
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
// static sendPackToLoopFT g_sendPackToLoopFun = nullptr;

static int sSendChMsg (packetHead* pack)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}

static bool sDelNetPack (void* pUP)
{
	auto pArg = (std::pair<NetTokenType, impLoop*>*) pUP; 
	auto pS = pArg->second;
	auto pISavePack = pS->getIPackSave ();
	auto pack = pISavePack->netTokenPackFind (pArg->first);
	if (pack) {
		mWarn ("pack delete by timer"<<*pack);
		auto fnFree = tSingleton<loopMgr>::single ().getPhyCallback().fnFreePack;
		pISavePack->netTokenPackErase (pArg->first);
		fnFree (pack);
	}
	return false;
}
static int sSendPackToSomeSession(loopHandleType myServerId, netPacketHead* pN, uqword* pSessS, udword sessionNum)
{
	int nRet = 0;
	auto& rMgr = tSingleton<loopMgr>::single ();
	do {
		auto pLoop = rMgr.getLoop(myServerId);
		if (!pLoop) {
			nRet = 1;
			break;
		}
		nRet = pLoop->sendPackToSomeSession (pN, pSessS, sessionNum);
	} while (0);
	return nRet;
}

int midSendPackToLoopForChannelFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	auto& rMgr = tSingleton<loopMgr>::single ();
	auto fnPushPackToLoop = tSingleton<loopMgr>::single().getPhyCallback().fnPushPackToLoop;
	pack->pAsk = 0;
	do {
		bool bRand = true;
		if (pN->ubyDesServId != c_emptyLoopHandle) {
			bRand = !packInOnceProc(pack);
		}
		auto objSer = pN->ubyDesServId;
		if (bRand) {
			objSer = rMgr.getOnceUpOrDownServer ();
			myAssert (c_emptyLoopHandle != objSer);
		}
		fnPushPackToLoop (objSer, pack);
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

	auto curHandleFun = rMgr.getPhyCallback().fnGetCurServerHandle;
	auto freeFun = rMgr.getForLogicFun().fnFreePack;
	auto curHandle = curHandleFun ();
	if (curHandle !=  pN->ubySrcServId) {
		mTrace ("curHandle = "<<curHandle<<"pN->ubySrcServId = "<<pN->ubySrcServId);
	}
	myAssert (pN->ubySrcServId == curHandle);
	auto pS = rMgr.getLoop(pN->ubySrcServId);
	myAssert (pS);
	bool bIsRet = NIsRet(pN);
	bool  bNeetRet = false;
	auto bInOnceProc =  packInOnceProc(pack);

	do {
		myAssert(pN->ubyDesServId != c_emptyLoopHandle);
		if (pN->ubyDesServId == c_emptyLoopHandle) {
			mError ("ubyDesServId == c_emptyLoopHandle pack = "<<*pack);
			freeFun (pack);
			break;
		}
		auto fnPushPackToLoop = rMgr.getPhyCallback().fnPushPackToLoop;
		if (bInOnceProc) {
			fnPushPackToLoop (pN->ubyDesServId, pack);
			break;
		}
		if (bIsRet) {
			if (EmptySessionID == pack->sessionID) {
				auto objSe = rMgr.getOnceUpServer ();
				myAssert (c_emptyLoopHandle != objSe);
				if (c_emptyLoopHandle == objSe) {       /*     由于本函数是处理首站发出, 不可能会出现这种情况        */
					mError(" can not find net server whith pack pack is : "<<*pack);
					freeFun (pack);
				} else {
					fnPushPackToLoop (objSe, pack);
				}
			} else {
				auto pSess = pS->getSession (pack->sessionID);
				myAssert (pSess);
				if (pSess) {
					pSess->send (pack);
				} else {
					mError(" can not find sesssion whith pack pack is : "<<*pack);
					freeFun (pack);
				}
			}
			break;
		}
		
		/*  以下处理ask类型消息的发送    */
		myAssert(EmptySessionID == pack->sessionID);

		auto& rMsgInfoMgr = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		auto retMsgId = rMsgInfoMgr.getRetMsg (pN->uwMsgID);
		if (c_null_msgID != retMsgId) {
			auto pFun = pS->findMsg(retMsgId);
			if (pFun ) {
				bNeetRet = true;
			}
		}
		if (bNeetRet) {
			NSetNeetRet(pN);
		} else {
			NSetUnRet(pN);
		}
		if (bNeetRet) {
			pN->dwToKen = pS->nextToken ();
		}
		pack->pAsk = 0;
		
		auto objSer = rMgr.getOnceUpServer ();
		myAssert (c_emptyLoopHandle != objSer);
		if (bNeetRet) {
			// pack->pAsk = 1;  /* 告知发送线程需要保存原包 */
			iPackSave* pISave = pS->getIPackSave ();

			packetHead* sclonePack(packetHead* p);
			auto pNew =  sclonePack (pack);  /*  由于要保存原包,克隆一份 */
			pISave->netTokenPackInsert (pNew);  /* 保存pack 因为该函数是通过网络发送的第一站,故在此保存   */
			std::pair<NetTokenType, impLoop*> pa;
			pa.first = pN->dwToKen;
			pa.second = pS;
			auto delTime = 6180;
			auto& rTimeMgr = pS->getTimerMgr ();
			rTimeMgr.addComTimer (delTime, sDelNetPack, &pa, sizeof (pa));
		}
		fnPushPackToLoop (objSer, pack);
	} while (0);
	return nRet;
}

/*
static iRpcInfoMgr* sGetIRpcInfoMgr()
{
	return &tSingleton<loopMgr>::single().defMsgInfoMgr();
}
*/

static int    sRegRpc(msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
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

static int sRegRouteFun(loopHandleType myServerId, loopHandleType objServerId, SessionIDType sessionId,  udword onlyId)
{
	int nRet = 0;
	do {
		auto pS = tSingleton<loopMgr>::single().getLoop(myServerId);
		myAssert (pS);
		if (!pS) {
			nRet = 1;
			mError ("can not find server id = "<<myServerId);
			break;
		}
		auto nR = pS->regRoute (objServerId, sessionId, onlyId);
		if (nR) {
			nRet = 2;
			mError ("RegRouteFun ret error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}

typedef void		(*freePackFT)(packetHead* pack);

serializePackFunType  loopMgr:: fromNetPack ()
{
    return m_fromNetPack;
}

int loopMgr::init(int nArgC, char** argS, PhyCallback& info)
{
	m_fromNetPack = nullptr;
	m_toNetPack = nullptr;
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	g_allocPackFun = info.fnAllocPack;
	g_freePackFun = info.fnFreePack;
	// g_sendPackToLoopFun = info.fnSendPackToLoop;
	forLogic.fnCreateLoop = sCreateServer;
	forLogic.fnAllocPack = sAllocPack; // info.fnAllocPack;
	forLogic.fnFreePack = sFreePack; //  info.fnFreePack;
	forLogic.fnRegMsg = sRegMsg;
	forLogic.fnSendPackToLoop =  midSendPackToLoopFun;
	forLogic.fnSendPackToLoopForChannel = midSendPackToLoopForChannelFun;
	forLogic.fnSendPackToSomeSession = sSendPackToSomeSession;
	forLogic.fnLogMsg = info.fnLogMsg;
	forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
	forLogic.fnNextToken = info.fnNextToken;
	// forLogic.fnGetIRpcInfoMgr = sGetIRpcInfoMgr;
	forLogic.fnPushToCallStack = info.fnPushToCallStack;
	forLogic.fnPopFromCallStack = info.fnPopFromCallStack;
	forLogic.fnLogCallStack = info.fnLogCallStack;
	forLogic.fnRegRpc = sRegRpc;
	forLogic.fnGetDefProcServerId = sGetDefProcServerId;
	forLogic.fnRegRoute = sRegRouteFun;

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

		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto midNetLibName = rArgS.midNetLibName ();
		if (midNetLibName ) {
			nR = initNetServer ();
			if (nR) {
				mError("initNetServer  return error nR = "<<nR);
				nRet = 2;
				break;
			}
		}
		auto& rModuleS = m_ModuleS;
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			rM.load(nArgC, argS, &forLogic );
			if (m_fromNetPack) {
				myAssert (m_fromNetPack == forLogic.fromNetPack);
			} else {
				m_fromNetPack = forLogic.fromNetPack;
			}
			if (m_toNetPack) {
				myAssert (m_toNetPack == forLogic.toNetPack);
			} else {
				m_toNetPack = forLogic.toNetPack;
			}

		}
	}while(0);
	return nRet;
}

int loopMgr::procArgS(int nArgC, char** argS)
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
	// m_upNum = 1;
	m_canUpRouteServerNum = 0;
	m_canDownRouteServerNum = 0;
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
	std::vector<loopHandleType>  upVec;
	std::vector<loopHandleType>  downVec;
	for (auto i = 0; i < LoopNum && nRet < nBuffNum; i++)
	{
		auto &p = m_loopS[i];
		if (!p) {
			continue;
		}
		auto sid = p->id ();
		if (p->canUpRoute ()) {
			upVec.push_back(sid);
		}
		if (p->canDownRoute ()) {
			downVec.push_back(sid);
		}
		auto &node = pBuff[nRet++];
		auto pNode = p->getServerNode ();
		if (pNode) {
			node = *pNode;
		} else {
			node.listenerNum = 0;
			node.connectorNum = 0;
		}
		node.handle = sid;//toHandle (pid, id);
		m_canUpRouteServerNum = upVec.size ();
		m_canDownRouteServerNum = downVec.size ();
		m_canRouteServerIdS = std::make_unique<loopHandleType[]> (m_canUpRouteServerNum + m_canDownRouteServerNum);
		auto curI = 0;
		for (auto it = upVec.begin (); it != upVec.end (); it++) {
			m_canRouteServerIdS [curI++] = *it;
		}
		for (auto it = downVec.begin (); it != downVec.end (); it++) {
			m_canRouteServerIdS [curI++] = *it;
		}
	}

	mTrace ("at the end nRet = "<<nRet);
	return nRet;
}

uword   loopMgr:: getAllCanRouteServerS (loopHandleType* pBuff, uword buffNum) // Thread safety
{
	auto nAll = m_canUpRouteServerNum + m_canDownRouteServerNum;
	myAssert (buffNum > nAll);
    uword  nRet = nAll;
	if (nRet > buffNum	) {
		nRet = buffNum;
	}
    do {
		for (decltype (nRet) i = 0; i < nRet; i++) {
			pBuff[i] = m_canRouteServerIdS[i];
		}
    } while (0);
    return nRet;
}

uword    loopMgr::getAllCanUpServerS (loopHandleType* pBuff, uword buffNum)
{
	myAssert (buffNum > m_canUpRouteServerNum);
	uword nRet = m_canUpRouteServerNum;
	if (nRet > buffNum	) {
		nRet = buffNum;
	}
	for (decltype (nRet) i = 0; i < nRet; i++) {
		pBuff[i] = m_canRouteServerIdS[i];
	}
	return nRet;
}

uword   loopMgr::getAllCanDownServerS (loopHandleType* pBuff, uword buffNum)
{
	myAssert (buffNum > m_canDownRouteServerNum);
	uword nRet = m_canDownRouteServerNum;
	if (nRet > buffNum	) {
		nRet = buffNum;
	}
	for (decltype (nRet) i = 0; i < nRet; i++) {
		pBuff[i] = m_canRouteServerIdS[i + m_canUpRouteServerNum];
	}
	return nRet;
}

loopHandleType loopMgr:: getOnceUpServer ()
{
    loopHandleType    nRet = c_emptyLoopHandle;
    do {
		if (m_canUpRouteServerNum) {
			auto i = rand () % m_canUpRouteServerNum;
			nRet = m_canRouteServerIdS [i];
		}
    } while (0);
    return nRet;
}

loopHandleType   loopMgr:: getOnceDownServer ()
{
    loopHandleType    nRet = c_emptyLoopHandle;
    do {
		if (m_canDownRouteServerNum) {
			auto i = rand () % m_canDownRouteServerNum;
			nRet = m_canRouteServerIdS [i +  m_canUpRouteServerNum];
		}
    } while (0);
    return nRet;
}

loopHandleType   loopMgr::getOnceUpOrDownServer ()
{
    loopHandleType    nRet = 0;
    do {
		nRet = getOnceUpServer ();
		if (c_emptyLoopHandle != nRet) {
			break;
		}
		nRet = getOnceDownServer ();
    } while (0);
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

int getMidDllPath (std::unique_ptr<char[]>& pathBuf);

int    loopMgr:: initNetServer ()
{
    int    nRet = 0;
    do {
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto midNetLibName = rArgS.midNetLibName ();
		std::unique_ptr<char[]> binH;
		getMidDllPath (binH);
		std::string strPath = binH.get (); 
		strPath += midNetLibName;
		// strPath += "/";
		strPath += dllExtName ();
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

serializePackFunType loopMgr :: toNetPack ()
{
    return m_toNetPack;
}

uword loopMgr :: canRouteNum ()
{
    return m_canUpRouteServerNum  + m_canDownRouteServerNum; // m_upNum;
}
/*
void  loopMgr :: setUpNum (uword v)
{
    m_upNum = v;
}
*/

bool  loopMgr:: isRootApp ()
{
    bool   nRet = 0;
    do {
		nRet =  m_canDownRouteServerNum && !m_canUpRouteServerNum;
    } while (0);
    return nRet;
}

uword loopMgr:: canUpRouteServerNum ()
{
    return m_canUpRouteServerNum;
}

void loopMgr:: setCanUpRouteServerNum (uword v)
{
    m_canUpRouteServerNum = v;
}

uword  loopMgr:: canDownRouteServerNum ()
{
    return m_canDownRouteServerNum;
}

void  loopMgr:: setCanDownRouteServerNum (uword v)
{
    m_canDownRouteServerNum = v;
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

