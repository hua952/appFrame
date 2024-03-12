#include <iostream>
#include <sstream>
#include <thread>
#include "serverMgr.h"
#include "strFun.h"
#include "loop.h"
#include "tSingleton.h"
#include "comFun.h"
#include "cppLevel0LCom.h"
#include "cArgMgr.h"

#include <set>
#include <string>
#include "CModule.h"
#include "myAssert.h"
#include "mLog.h"
#include "mArgMgr.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "modelLoder.h"
#include "comMsgMsgId.h"
#include <vector>
#include "argConfig.h"

packetHead* allocPack(udword udwSize)
{
	PUSH_FUN_CALL
	auto pRet = (packetHead*)(new char [sizeof(packetHead) + sizeof(netPacketHead) + udwSize]);
	pRet->pAsk = 0;
	auto pN = P2NHead(pRet);
	pN->udwLength = udwSize;
	pN->uwTag = 0;
	POP_FUN_CALL
	//rTrace (" after allocPack pack = "<<pRet);
	return pRet;
}

void	freePack(packetHead* pack)
{
	PUSH_FUN_CALL
	lv0LogCallStack (27);
	if (pack->pAsk) {
		freePack ((packetHead*)(pack->pAsk));
		pack->pAsk = 0;
	}
	auto pN = P2NHead (pack);
	rTrace (" will delete pack "<<*pack);
	delete [] ((char*)(pack));
	POP_FUN_CALL
}

int sPushPackToLoop (loopHandleType pThis, packetHead* pack)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	server* pS = nullptr;
	pS = rMgr.getServer (pThis); // pServerS[ubyDl];
	if (pS) {
		pS->pushPack (pack);
	} else {
		nRet = 1;
		rError ("can not find server handle = "<<pThis);
	}
	return nRet;
}

static void stopLoopS()
{
}

struct pSerializePackFunType3
{
	serializePackFunType f[3];
};

class pSerializePackFunType3Cmp 
{
public:
	bool operator () (const pSerializePackFunType3& a, const pSerializePackFunType3& b) const
	{
		return a.f[0] < b.f[0];
	}
};

using  msgSerFunSet = arraySet<pSerializePackFunType3, pSerializePackFunType3Cmp>;
static msgSerFunSet s_SerFunSet;

static pSerializePackFunType3* sGetNode (udword msgId)
{
	auto & rArr = s_SerFunSet;
	pSerializePackFunType3 temp;
	temp.f[0] = (serializePackFunType)msgId;
	auto pRet = rArr.GetNode (temp);
	return pRet;
}

static int sFromNetPack (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		// myAssert (p);
		// auto pN = P2NHead(p);
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[1];
			if (fun) {
				auto nR = fun (pN, pNew);
				if (nR) {
					nRet = 2;
				}
				if (pNew) {
					pNew->pAsk = 0;
				}
			}
		}
	} while (0);
	return nRet;
}

static int sToNetPack (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	do {
		// myAssert (p);
		// auto pN = P2NHead(p);
		auto pF = sGetNode (pN->uwMsgID);
		if (pF) {
			auto fun = (*pF).f[2];
			auto nR = fun (pN, pNew);
			if (nR) {
				nRet = 2;
			}
			if (pNew) {
				pNew->pAsk = 0;
			}
		}
	} while (0);
	return nRet;
}
serverMgr::serverMgr()
{
	m_outNum = 1;
	m_createTcpServerFn = nullptr;
	m_delTcpServerFn = nullptr;
	m_packSendInfoTime = 5000;

	m_CurLoopNum = 0;
	m_gropId = 0;
    m_delSendPackTime = 5000;
	m_canUpRouteServerNum = 0;
	m_canDownRouteServerNum = 0;
}

int  serverMgr:: pushPackToLoop (loopHandleType pThis, packetHead* pack)
{
	int  nRet = 0;
	do {
		server* pS = nullptr;
		pS = getServer (pThis);
		if (pS) {
			pS->pushPack (pack);
		} else {
			nRet = 1;
			rError ("can not find server handle = "<<pThis);
		}
	} while (0);
	return nRet;
}

udword  serverMgr::  delSendPackTime ()
{
    return m_delSendPackTime;
}
void  serverMgr::setDelSendPackTime (udword  va)
{
    m_delSendPackTime = va;
}
udword  serverMgr::  packSendInfoTime ()
{
    return m_packSendInfoTime;
}

void serverMgr:: setPackSendInfoTime (udword  va)
{
    m_packSendInfoTime = va;
}

server*  serverMgr::getOutServer()
{
	auto n = rand () % m_outNum;
	return m_loopS[n].get(); // g_serverS [n];
}

serverMgr::~serverMgr()
{
}

loopHandleType	serverMgr::procId()
{
	return m_procId;
}

loopHandleType	serverMgr::gropId()
{
	return m_gropId;
}


void serverMgr::setProcId(loopHandleType proc)
{
	m_procId = proc;
}

void			serverMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

serverIdType 	serverMgr::getServerNum()
{
	return g_ServerNum;
}
/*
pserver* serverMgr::getServerS()
{
	return g_serverS.get();
}
PhyCallback&  serverMgr::getPhyCallback()
{
	return m_PhyCallback;
}
*/

int serverMgr::procArgS(int nArgC, char** argS)
{
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
			if(0 == strcmp(buff[0], "gropId")) {
				int nId = atoi (buff[1]);
				setGropId (nId);
			} else if(0 == strcmp(buff[0], "procId")) {
				int nId = atoi (buff[1]);
				setProcId(nId);
			} else if (0 == strcmp(buff[0], "netLib")) {
				auto nL = strlen (buff[1]);
				m_netLibName = std::make_unique <char[]>(nL + 1);
				auto p = m_netLibName.get();
				strNCpy (p, nL + 1, buff[1]);
			}
		}
	}
	return 0;
}

static NetTokenType   sNextToken(loopHandleType pThis)
{
	NetTokenType   nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		nRet = pTH->nextToken ();
	}
	return nRet;
}


static void  sPushToCallStack (loopHandleType pThis, const char* szTxt)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->pushToCallStack (szTxt);
	}
}

static void  sPopFromCallStack (loopHandleType pThis)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->popFromCallStack ();
	}
}

static void  sLogCallStack (loopHandleType pThis, int nL)
{
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getServer(pThis);
	if (pTH) {
		pTH->logCallStack (nL);
	}
}

server*   serverMgr:: getServer(loopHandleType handle)
{
	server* pRet = nullptr;
	
	for (auto i = 0; i < LoopNum; i++) {
		// auto curH = g_serverS[i]->myHandle ();
		if (!m_loopS[i]) {
			continue;
		}
		auto curH = m_loopS[i]->myHandle ();
		if (curH == handle) {
			pRet = m_loopS[i].get(); // g_serverS[i];
			break;
		}
	}
	return pRet;
}
static loopHandleType      getCurServerHandle ()
{
	return server::s_loopHandleLocalTh;
}

int serverMgr::initFun (int cArg, char** argS)
{
	int nRet = 0;
	tSingleton <cArgMgr>::createSingleton ();
	auto& rArgS = tSingleton<cArgMgr>::single ();
	rArgS.procArgS (cArg, argS);
	procArgS (cArg, argS);
	
	tSingleton <argConfig>::createSingleton ();
	auto& rConfig = tSingleton<argConfig>::single ();
	do {
		auto nR = rConfig.procCmdArgS (cArg, argS);
		if (nR) {
			nRet = 1;
			mError("rConfig.procCmdArgS error nR = "<<nR);
			break;
		}
		auto logLevel = rConfig.logLevel ();
		auto pWorkDir = rConfig.workDir ();
		myAssert (pWorkDir);
		std::string strFile = pWorkDir;
		strFile += "/logs";
		myMkdir (strFile.c_str());
		auto pLogFile = rConfig.logFile();
		strFile += pLogFile;
		initLogGlobal ();
		auto nInitLog = initLog ("appFrame", strFile.c_str(), logLevel);
		if (0 != nInitLog) {
			std::cout<<"initLog error nInitLog = "<<nInitLog<<std::endl;
			break;
		}
		rInfo ("Loger init OK");
		auto nInitMidFrame = InitMidFrame(cArg, argS/*, &rMgr*/);
		auto dumpMsg = rArgS.dumpMsg ();
		if (dumpMsg) {
			rInfo ("dupmMsg end plese check");
			break;
		}
		if (0 != nInitMidFrame) {
			nRet = 1;
			rError ("InitMidFrame error nRet =  "<<nInitMidFrame);
			break;
		}
		const auto c_maxLoopNum = 16;
		serverNode loopHandleS[c_maxLoopNum];
		auto proLoopNum =  getAllLoopAndStart(loopHandleS, c_maxLoopNum);
		rInfo ("initFun proLoopNum = "<<proLoopNum);
		if (proLoopNum > 0) {
			g_ServerNum = proLoopNum;
			
			auto detachServerS = rArgS.detachServerS ();
			
			if (detachServerS) {
				for (int i = 0; i < LoopNum; i++ ) {
					auto p = m_loopS[i].get();//pServerS[i];
					if (!p) {
						continue;
					}
					auto autoRun = p->autoRun ();
					if (autoRun) {
						p->start();
						p->detach();
					}
				}
			} else {
				for (int i = 0; i < LoopNum; i++ ) {
					auto p = m_loopS[i].get(); // pServerS[i];
					if (!p) {
						continue;
					}
					p->start();
				}
				for (int i = 0; i < LoopNum; i++ ) {
					auto p = m_loopS[i].get(); // pServerS[i];
					if (!p) {
						continue;
					}
					p->join();
				}
				std::cout<<"All server End"<<std::endl;
			}
			loggerDrop ();
		}
	} while (0);
	return nRet;
}

createTcpServerFT  serverMgr::  createTcpServerFn ()
{
    return m_createTcpServerFn;
}

void  serverMgr::setCreateTcpServerFn (createTcpServerFT  va)
{
    m_createTcpServerFn = va;
}

delTcpServerFT  serverMgr::delTcpServerFn ()
{
    return m_delTcpServerFn;
}

void  serverMgr::setDelTcpServerFn (delTcpServerFT  va)
{
    m_delTcpServerFn = va;
}

ubyte  serverMgr::upNum ()
{
    return m_outNum;
}

void serverMgr::setUpNum (ubyte  va)
{
    m_outNum = va;
}

void         lv0PushToCallStack (const char* szTxt)
{
	sPushToCallStack (server::s_loopHandleLocalTh, szTxt);
}

void         lv0PopFromCallStack ()
{
	sPopFromCallStack(server::s_loopHandleLocalTh);
}

void         lv0LogCallStack (int nL)
{
	sLogCallStack (server::s_loopHandleLocalTh, nL);
}


int  InitMidFrame(int nArgC, char** argS/*, PhyCallback* pCallbackS*/)
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
		// tSingleton<loopMgr>::createSingleton();
		// auto& rMgr = tSingleton<loopMgr>::single();
		auto& rMgr = tSingleton<serverMgr>::single();
		nRet = rMgr.init(nArgC, argS/*, *pCallbackS*/);
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
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	return rMgr.getAllLoopAndStart(pBuff, nBuffNum);
}

static int sRegMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun)
{
	int nRet = 0;
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
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
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
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
	// auto& rMgr = tSingleton<loopMgr>::single();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto pTH = rMgr.getLoop(pThis);
	//mTrace ("at then begin of sAddComTimer pTH = "<<pTH);
	cTimerMgr&  rTimeMgr =    pTH->getTimerMgr();
	rTimeMgr.addComTimer (firstSetp, udwSetp, pF, pUsrData, userDataLen);
	return nRet;
}
// static sendPackToLoopFT g_sendPackToLoopFun = nullptr;

static bool sDelNetPack (void* pUP)
{
	auto pArg = (std::pair<NetTokenType, server*>*) pUP; 
	auto pS = pArg->second;
	auto pISavePack = pS->getIPackSave ();
	auto pack = pISavePack->netTokenPackFind (pArg->first);
	if (pack) {
		mWarn ("pack delete by timer"<<*pack);
		pISavePack->netTokenPackErase (pArg->first);
		freePack (pack);
	}
	return false;
}
static int sSendPackToSomeSession(loopHandleType myServerId, netPacketHead* pN, uqword* pSessS, udword sessionNum)
{
	int nRet = 0;
	auto& rMgr = tSingleton<serverMgr>::single();
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
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; //tSingleton<serverMgr>::single ();
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
		rSerMgr.pushPackToLoop  (objSer, pack);
	} while (0);
	return nRet;
}

int midSendPackToLoopFun(packetHead* pack) /* 返回值貌似没用 */
{
	/* 发消息得起点函数 */
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	// auto& rMgr = tSingleton<loopMgr>::single ();
	auto& rMgr = tSingleton<serverMgr>::single();
	auto& rSerMgr = rMgr; // tSingleton<serverMgr>::single ();

	auto pS = rMgr.getLoop(pN->ubySrcServId);
	myAssert (pS);
	bool bIsRet = NIsRet(pN);
	bool  bNeetRet = false;
	auto bInOnceProc =  packInOnceProc(pack);

	do {
		myAssert(pN->ubyDesServId != c_emptyLoopHandle);
		if (pN->ubyDesServId == c_emptyLoopHandle) {
			mError ("ubyDesServId == c_emptyLoopHandle pack = "<<*pack);
			freePack (pack);
			break;
		}
		if (bInOnceProc) {
			rSerMgr.pushPackToLoop (pN->ubyDesServId, pack);
			break;
		}
		if (bIsRet) {
			if (EmptySessionID == pack->sessionID) {
				auto objSe = rMgr.getOnceUpServer ();
				myAssert (c_emptyLoopHandle != objSe);
				if (c_emptyLoopHandle == objSe) {       /*     由于本函数是处理首站发出, 不可能会出现这种情况        */
					mError(" can not find net server whith pack pack is : "<<*pack);
					freePack (pack);
				} else {
					rSerMgr.pushPackToLoop (objSe, pack);
				}
			} else {
				auto pSess = pS->getSession (pack->sessionID);
				myAssert (pSess);
				if (pSess) {
					pSess->send (pack);
				} else {
					mError(" can not find sesssion whith pack pack is : "<<*pack);
					freePack (pack);
				}
			}
			break;
		}
		
		/*  以下处理ask类型消息的发送    */
		myAssert(EmptySessionID == pack->sessionID);

		// auto& rMsgInfoMgr = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		auto& rMsgInfoMgr = rMgr.defMsgInfoMgr ();
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

		auto toNetPack = rMgr.toNetPack ();
		packetHead* pNew = nullptr;
		toNetPack (pN, pNew);
		
		if (bNeetRet) {
			iPackSave* pISave = pS->getIPackSave ();

			packetHead* sclonePack(packetHead* p);
			if (!pNew) {
				pNew =  sclonePack (pack);  /*  由于要保存原包,克隆一份 */
			}

			pISave->netTokenPackInsert (pack);  /* 保存pack 因为该函数是通过网络发送的第一站,故在此保存   */
			std::pair<NetTokenType, server*> pa;
			pa.first = pN->dwToKen;
			pa.second = pS;
			auto delTime = 61800;
			auto& rTimeMgr = pS->getTimerMgr ();
			rTimeMgr.addComTimer (delTime, sDelNetPack, &pa, sizeof (pa));
			pack = pNew;
		} else {
			if (pNew) {
				// freeFun (pack);
				freePack (pack);
				pack = pNew;
			}
		}
		pack->sessionID = EmptySessionID;
		pack->loopId = c_emptyLoopHandle;
		rSerMgr.pushPackToLoop (objSer, pack);
	} while (0);
	return nRet;
}

static int    sRegRpc(msgIdType askId, msgIdType retId, serverIdType	askDefProcSer,
			serverIdType	retDefProcSer)
{
	// auto& rMgr = tSingleton<loopMgr>::single().defMsgInfoMgr();
	auto& rMgr = tSingleton<serverMgr>::single().defMsgInfoMgr();
	return rMgr.regRpc (askId, retId, askDefProcSer, retDefProcSer);
}

serverIdType sGetDefProcServerId (msgIdType msgId)
{
	// auto& rMgr = tSingleton<loopMgr>::single().defMsgInfoMgr();
	auto& rMgr = tSingleton<serverMgr>::single().defMsgInfoMgr();
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
	auto& rMgr = tSingleton<serverMgr>::single();
	return rMgr.createServer(szName, serId,
			pNode, funFrame, arg);
}

static int sRegRouteFun(loopHandleType myServerId, loopHandleType objServerId, SessionIDType sessionId,  udword onlyId)
{
	int nRet = 0;
	do {
		auto& rMgr = tSingleton<serverMgr>::single();
		auto pS = rMgr.getLoop(myServerId);
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

serializePackFunType  serverMgr:: fromNetPack ()
{
    return m_fromNetPack;
}

int serverMgr::init(int nArgC, char** argS/*, PhyCallback& info*/)
{
	m_fromNetPack = nullptr;
	m_toNetPack = nullptr;
	auto& forLogic = getForLogicFun();
	g_allocPackFun = allocPack; // info.fnAllocPack;
	g_freePackFun = freePack; // info.fnFreePack;
	forLogic.fnCreateLoop = sCreateServer;
	forLogic.fnAllocPack = sAllocPack; // info.fnAllocPack;
	forLogic.fnFreePack = sFreePack; //  info.fnFreePack;
	forLogic.fnRegMsg = sRegMsg;
	forLogic.fnSendPackToLoop =  midSendPackToLoopFun;
	forLogic.fnSendPackToLoopForChannel = midSendPackToLoopForChannelFun;
	forLogic.fnSendPackToSomeSession = sSendPackToSomeSession;
	forLogic.fnLogMsg = logMsg; // info.fnLogMsg;
	forLogic.fnAddComTimer = sAddComTimer;//m_callbackS.fnAddComTimer;
	forLogic.fnNextToken = sNextToken; //info.fnNextToken;
	forLogic.fnRegRpc = sRegRpc;
	forLogic.fnGetDefProcServerId = sGetDefProcServerId;
	forLogic.fnRegRoute = sRegRouteFun;
	forLogic.fromNetPack = nullptr;
	forLogic.toNetPack = nullptr;

	int nR = 0;
	int nRet = 0;
	rConfig = tSingleton<frameConfig>::single ();
	nR= procArgSMid(nArgC, argS);
	do
	{
		if(0 != nRet) {
			mError(" procArgSMid error nR = "<<nR);
			nRet = 1;
			break;
		}
		auto workDir = rConfig.workDir();
		if (workDir) {
			strPath = workDir;
		} else {
			std::unique_ptr<char[]> t;
			getCurModelPath (t);
			strPath = t.get();
		}
		strPath += "/bin/";
		strPath += szName;
		strPath += dllExtName();
		auto hdll = loadDll (strPath.c_str());
		if (hdll) {
			typedef int  (*getSerializeFunSFT) (defMsgSerializePackFunS* pFunS, ForLogicFun* pForLogic);
			auto funGetSerializeFunS = (getSerializeFunSFT)(getFun (hdll, "getSerializeFunS"));
			if (funGetSerializeFunS) {
				static defMsgSerializePackFunS   s_FunS;
				funGetSerializeFunS(&s_FunS, &forLogic);
				auto serNum = sizeof (s_FunS) / sizeof (pSerializePackFunType3);
				if (serNum) {
					s_SerFunSet.init ((pSerializePackFunType3*)(&s_FunS), serNum);
				}
				// forLogic.pSerFunSPtr = &s_FunS;
				forLogic.fromNetPack = sFromNetPack;
				forLogic.toNetPack = sToNetPack;
			}
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

int serverMgr::procArgSMid(int nArgC, char** argS)
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

int serverMgr::createServerS()
{
	int nRet = 0;
	
	return nRet;
}

int serverMgr::createServer(const char* szName, loopHandleType serId,  serverNode* pNode, frameFunType funFrame, void* arg)
{
	loopHandleType pRet = c_emptyLoopHandle;
	loopHandleType pid = 0;
	loopHandleType sid = 0;
	fromHandle (serId, pid, sid);
	auto& p = m_loopS[sid];
	myAssert (!p);
	p = std::make_unique<server> ();
	p->initMid(szName, serId, pNode, funFrame, arg);
	m_CurLoopNum++;
	mInfo ("createServer szName = "<<szName<<" pid = "<<pid<<" sid = "<<sid<<" m_CurLoopNum = "<<m_CurLoopNum);
	pRet = serId;
	return pRet;
}

msgMgr& serverMgr::defMsgInfoMgr ()
{
	return m_defMsgInfoMgr;
}

/*
loopHandleType	serverMgr::procId()
{
	return tSingleton<mArgMgr>::single().procId();
}
loopHandleType	serverMgr::gropId()
{
	return m_gropId;
}

void	serverMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}
*/
server* serverMgr::getLoop(loopHandleType id)
{
	loopHandleType pid = 0;
	loopHandleType lid = 0;
	fromHandle (id, pid, lid);
	return m_loopS[lid].get();
}

int regSysProcPacketFun (regMsgFT fnRegMsg, serverIdType handle);
int serverMgr::getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
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

uword  serverMgr:: getAllCanRouteServerS (loopHandleType* pBuff, uword buffNum) // Thread safety
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

uword   serverMgr::getAllCanUpServerS (loopHandleType* pBuff, uword buffNum)
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

uword   serverMgr::getAllCanDownServerS (loopHandleType* pBuff, uword buffNum)
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

loopHandleType serverMgr:: getOnceUpServer ()
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

loopHandleType serverMgr:: getOnceDownServer ()
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

loopHandleType serverMgr::getOnceUpOrDownServer ()
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
/*
PhyCallback&   serverMgr:: getPhyCallback()
{
	return m_callbackS;
}
*/
ForLogicFun&  serverMgr::getForLogicFun()
{
	return  m_forLogic;
}

int getMidDllPath (std::unique_ptr<char[]>& pathBuf);

int   serverMgr :: initNetServer ()
{
    int    nRet = 0;
    do {
		auto& rArgS = tSingleton<mArgMgr>::single ();
		auto midNetLibName = rArgS.midNetLibName ();
		std::unique_ptr<char[]> binH;
		getMidDllPath (binH);
		std::string strPath = binH.get (); 
		strPath += midNetLibName;
		strPath += dllExtName ();
		// auto& rC = getPhyCallback();
		nRet = initComTcpNet (strPath.c_str(), allocPack /*rC.fnAllocPack*/, freePack /*rC.fnFreePack*/, logMsg /*rC.fnLogMsg*/);
		if (nRet) {
			mError ("initComTcpNet error nRet = "<<nRet<<" strPath = "
					<<strPath.c_str());
			break;
		}
    } while (0);
    return nRet;
}

serializePackFunType serverMgr:: toNetPack ()
{
    return m_toNetPack;
}

uword serverMgr:: canRouteNum ()
{
    return m_canUpRouteServerNum  + m_canDownRouteServerNum; // m_upNum;
}

bool  serverMgr:: isRootApp ()
{
    bool   nRet = 0;
    do {
		nRet =  m_canDownRouteServerNum && !m_canUpRouteServerNum;
    } while (0);
    return nRet;
}

uword serverMgr:: canUpRouteServerNum ()
{
    return m_canUpRouteServerNum;
}

void serverMgr:: setCanUpRouteServerNum (uword v)
{
    m_canUpRouteServerNum = v;
}

uword serverMgr:: canDownRouteServerNum ()
{
    return m_canDownRouteServerNum;
}

void  serverMgr:: setCanDownRouteServerNum (uword v)
{
    m_canDownRouteServerNum = v;
}

void serverMgr:: logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnConnect ();
			fu (fId, sessionId, userData);
		}
    } while (0);
}

void  serverMgr:: logicOnAccept(serverIdType	fId, SessionIDType sessionId, uqword userData)
{
	do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnLogicOnAccept ();
			fu (fId, sessionId, userData);
		}
    } while (0);
}

void  serverMgr:: onLoopBegin(ServerIDType fId)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopBegin ();
			fu (fId);
		}
    } while (0);
}

void  serverMgr:: onLoopEnd(ServerIDType fId)
{
    do {
		for (decltype (m_ModuleNum) i = 0; i < m_ModuleNum; i++) {
			auto& rM = m_ModuleS[i];
			auto fu = rM.fnOnLoopEnd ();
			fu (fId);
		}
    } while (0);
}

