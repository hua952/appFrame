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

int  InitMidFrame(int nArgC, const char* argS[],PhyCallback* pCallbackS)
{
	//std::cout<<"At the begin of InitMidFrame"<<std::endl;
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
	tSingleton<PhyInfo>::createSingleton();
	tSingleton<loopMgr>::createSingleton();
	auto& rMgr = tSingleton<PhyInfo>::single();
	//auto nRet = rMgr.init(nArgC, argS, *pCallbackS);
	nRet = rMgr.init(nArgC, argS, *pCallbackS);
	} while (0);
	return nRet;
}

int getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	//mInfo (" At then begin of getAllLoopAndStart");
	auto& rMgr = tSingleton<loopMgr>::single();
	return rMgr.getAllLoopAndStart(pBuff, nBuffNum);
}

PhyInfo::PhyInfo()
{
}

int PhyInfo::createServer (const char* szName, loopHandleType serId, serverNode* pNode, frameFunType funFrame, void* arg)
{
	//mTrace ("At the begin");
	auto& rMgr = tSingleton<loopMgr>::single();
	auto pRet = rMgr.createServer(szName, serId, pNode, funFrame, arg);
	//mTrace ("At the end");
	return pRet ;
}

int PhyInfo::regMsg(loopHandleType handle, uword uwMsgId, procRpcPacketFunType pFun)
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

int PhyInfo::removeMsg(loopHandleType handle, uword uwMsgId)
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

static bool sDelNetPack (void* pUP)
{
	auto pArg = (std::pair<NetTokenType, loopHandleType>*) pUP; 
	auto handle = pArg->second;
	auto& rMgr = tSingleton<loopMgr>::single ();
	auto pS = rMgr.getLoop(handle);
	auto pISavePack = pS->getIPackSave ();
	auto pack = pISavePack->threadTokenPackFind (pArg->first);
	if (pack) {
		auto fnFree = tSingleton<PhyInfo>::single (). getPhyCallback().fnFreePack;
		// auto dPack = pack;
		// auto pDN = P2NHead (dPack);
		// mTrace ("pArg->first = "<<pArg->first<<" handle = "<<handle);
		// mWarn (" pack delete by timer dwToKen = "<<pDN->dwToKen<<" msgId = "<<pDN->uwMsgID);
		pISavePack->netTokenPackErase (pArg->first);
		fnFree (pack);
	}
	return false;
}
static int sSendChMsg (packetHead* pack)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}

static int sSendPackToLoopFun(packetHead* pack) /* 返回值貌似没用 */
{
	int nRet = 0; // procPacketFunRetType_del;
	int nR = 0;
	auto pN = P2NHead (pack);
	/*
	auto chMsg = NIsChAddType (pN);
	if (chMsg) {
		nRet = sSendChMsg (pack);
	} else {
	*/
		auto& rMgr = tSingleton<loopMgr>::single ();
		auto curHandleFun = tSingleton<PhyInfo>::single ().getPhyCallback().fnGetCurServerHandle;
		auto curHandle = curHandleFun ();
		if (curHandle !=  pN->ubySrcServId) {
			mTrace ("curHandle = "<<curHandle<<"pN->ubySrcServId = "<<pN->ubySrcServId);
		}
		myAssert (pN->ubySrcServId == curHandle);
		auto pS = rMgr.getLoop(pN->ubySrcServId);
		myAssert (pS);
		// auto& rMsgInfoS = tSingleton<loopMgr>::single ().defMsgInfoMgr ();
		bool bIsRet = NIsRet(pN); // NrMsgInfoS.isRetMsg (pN->uwMsgID);
		if (!bIsRet) {
			pN->dwToKen = pS->nextToken ();
		}
		auto bInOnceProc =  packInOnceProc(pack);
		auto pISavePack = pS->getIPackSave ();
		auto addTimerFun = tSingleton<PhyInfo>::single().getForLogicFun().fnAddComTimer;
		auto delTime = rMgr.delSendPackTime ();
		std::pair<NetTokenType, loopHandleType> pa;
		pa.first = pN->dwToKen;
		pa.second = pN->ubySrcServId;
		if (!bInOnceProc && !bIsRet) {
			mTrace ("will insert ask pack -----dwToKen = "<<pN->dwToKen<<" msgId = "
					<<pN->uwMsgID<<" map.size = "<<0<<" pack = "<<pack
					<<" server handle = "<<pS->id ());
			pISavePack->netTokenPackInsert (pack);
			addTimerFun (pN->ubySrcServId, delTime, delTime, sDelNetPack, &pa, sizeof (pa));
		}
		nRet = g_sendPackToLoopFun (pack); // must in the end
	// }
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
	return g_allocPackFun (udwSize);
}

static freePackFT g_freePackFun = nullptr;
static void sFreePack (packetHead* pack)
{
	auto pN = P2NHead (pack);
	/*
	mTrace (" midFrame will delete pack msgID = "<<pN->uwMsgID<<" token = "<<pN->dwToKen
			<<" srcHandle = "<<(int)(pN->ubySrcServId)<<" desHandle = "<<(int)(pN->ubyDesServId)
			<<" pack = "<<pack);
			*/
	g_freePackFun (pack);
}

typedef void		(*freePackFT)(packetHead* pack);
int PhyInfo::init(int nArgC, const char* argS[], PhyCallback& info)
{
	auto nHomeR = getCurModelPath (m_home);
	myAssert (0 == nHomeR);
	m_callbackS = info;
	auto& forLogic = getForLogicFun();
	g_allocPackFun = info.fnAllocPack;
	g_freePackFun = info.fnFreePack;
	g_sendPackToLoopFun = info.fnSendPackToLoop;
	forLogic.fnCreateLoop = PhyInfo::createServer;
	forLogic.fnAllocPack = sAllocPack; // info.fnAllocPack;
	forLogic.fnFreePack = sFreePack; //  info.fnFreePack;
	forLogic.fnRegMsg = regMsg;
	forLogic.fnSendPackToLoop =  sSendPackToLoopFun;
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
	auto nRet = procArgS(nArgC, argS);
	do
	{
		if(0 != nRet)
		{
			break;
		}
		//mInfo ("before load moduleS moduleNum = "<<m_ModuleNum);
		auto& rModuleS = m_ModuleS;
		for(auto i = 0; i < m_ModuleNum; i++)
		{
			auto& rM = m_ModuleS[i];
			//mTrace(" Before load modle "<<i);
			rM.load(nArgC, argS, &forLogic );
			//mTrace(" After load modle "<<i);
		}
	}while(0);
	return nRet;
}

const char*  PhyInfo:: binHome ()
{
	return m_home.get();
}

int PhyInfo::procArgS(int nArgC, const char* argS[])
{
	using tempModuleInfo = std::set<std::string>;
	tempModuleInfo moduleS;
	for(int i = 1; i < nArgC; i++)
	{
		//mTrace ("proc arg : "<<argS[i]);
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
				//auto nRR = strR(buff[1], '+', buff, c_BuffNum);
				auto bI = moduleS.insert(buff[1]);
				//mTrace (" insert model "<<buff[1]);
				myAssert (bI.second);
			} else if(0 == strcmp(buff[0], "gropId")) {
				int nId = atoi (buff[1]);
				tSingleton<loopMgr>::single().setGropId (nId);
			} else if(0 == strcmp(buff[0], "procId")) {
				int nId = atoi (buff[1]);
				tSingleton<loopMgr>::single().setProcId(nId);
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

CModule* PhyInfo::getModuleS (int& ModuleNum)
{
	ModuleNum = m_ModuleNum;
	return m_ModuleS.get();
}

PhyCallback& PhyInfo::getPhyCallback()
{
	return   m_callbackS;
}

ForLogicFun&  PhyInfo::getForLogicFun()
{
	return m_forLogic;
}
 
loopMgr::loopMgr():m_CurLoopNum(0), m_procId(0), m_gropId(0)
{
	 //m_loopS = std::make_unique<impLoop[]> (c_MaxLoopNum);

    m_delSendPackTime = 5000;
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
	/*
	auto& rPhyInfo = tSingleton<PhyInfo>::single();
	int ModuleNum = 0;
	CModuleS* pMs =  getModuleS (ModuleNum);
	for (
	*/
	return nRet;
}
/*
static inline loopHandleType toHandle(loopHandleType g, loopHandleType p, loopHandleType l)
{
	loopHandleType nRet = g;
	nRet &= GroupMark;
	nRet <<= ProcNumBitLen;
	p &= ProcMark;
	nRet |= p;
	nRet <<= LoopNumBitLen;
	l &= LoopMark;
	nRet |= l;
	return nRet;
}
*/
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
	return m_procId;
}

loopHandleType	loopMgr::gropId()
{
	return m_gropId;
}

void		loopMgr::	setProcId(loopHandleType proc)
{
	m_procId = proc;
}

void			loopMgr::setGropId(loopHandleType grop)
{
	m_gropId = grop;
}

int		loopMgr::init(loopHandleType	procId, loopHandleType	gropId)
{
	m_procId = procId;
	m_gropId = gropId;
	return 0;
}

impLoop*  loopMgr::getLoop(loopHandleType id)
{
	loopHandleType pid = 0;
	loopHandleType lid = 0;
	fromHandle (id, pid, lid);
	//mTrace ("getLoop m_CurLoopNum = "<<m_CurLoopNum);
	//return lid < m_CurLoopNum ? &m_loopS[id] : nullptr;
	return m_loopS[lid].get();
}

int loopMgr::getAllLoopAndStart(serverNode* pBuff, int nBuffNum)
{
	auto pid = procId ();
	//mTrace ("will start all loop m_CurLoopNum = "<<m_CurLoopNum);
	int nRet = 0;
	for (auto i = 0; i < LoopNum && nRet < nBuffNum; i++)
	{
		auto &p = m_loopS[i];
		if (!p) {
			continue;
		}
		auto &node = pBuff[nRet++];
		auto pNode = p->getServerNode ();
		//mTrace ("get server i = "<<i<<" pNode = "<<pNode);
		if (pNode) {
			node = *pNode;
		} else {
			node.listenerNum = 0;
			node.connectorNum = 0;
		}
		auto sid = p->id ();
		node.handle = sid;//toHandle (pid, id);
	}

	//std::cout<<"loopMgr::getAllLoopAndStart i = "<<i<<" m_CurLoopNum = "<<m_CurLoopNum<<std::endl;
	mTrace ("at the end nRet = "<<nRet);
	return nRet;
}

PhyCallback&   loopMgr:: getPhyCallback()
{
	return tSingleton<PhyInfo>::single ().getPhyCallback();
}

