#include "moduleLogicServerGen.h"
#include "strFun.h"
#include "moduleGen.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/serverFile.h"
#include "fromFileData/msgGroupFile.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/rpcFile.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgPmpFile.h"
#include "fromFileData/globalFile.h"
#include "xmlGlobalLoad.h"
#include "tSingleton.h"
#include "rLog.h"
#include "tSingleton.h"
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "configMgr.h"

static bool sIsPathExit  (const char* szFile)
{
	bool bRet = tSingleton<configMgr>::single().reProc ();
	if (bRet) {
		bRet = false;
	} else {
		bRet = isPathExit (szFile);
	}
	return bRet;
}

moduleLogicServerGen:: moduleLogicServerGen ()
{
}

moduleLogicServerGen:: ~moduleLogicServerGen ()
{
}

int   moduleLogicServerGen:: startGen (moduleGen& rMod)
{
    int   nRet = 0;
	do {
		auto& rData = rMod.moduleData ();
		auto modName = rData.moduleName ();
		int nR = 0;
		nR = genH (rMod);
		if (nR) {
			rError ("moduleLogicServerGen write h file error modName = "<<modName<<" nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = genCpp (rMod);
		if (nR) {
			rError ("moduleLogicServerGen write cpp file error modName = "<<modName<<" nR = "<<nR);
			nRet = 2;
			break;
		}

		auto& rSS = rData.orderS ();
		for (auto it = rSS.begin (); rSS.end () != it; ++it) {
			auto pName = it->get()->serverName ();
			/*
			nR = genOnFrameFun (rMod, pName);
			if (nR) {
				nRet = 3;
				break;
			}
			*/
			nR = genPackFun (rMod, pName);
			if (nR) {
				nRet = 4;
				break;
			}
		}
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genMgrCpp (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto& rData = rMod.moduleData();
		auto& rSS = rData.orderS ();
		auto& rGloble = tSingleton<xmlGlobalLoad>::single ();
		auto serverNum = rSS.size ();
		auto genPath = rMod.genPath ();
		std::string strFilename = genPath;
		auto pModName = rData.moduleName ();
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		std::string genMgrCpp = genPath;
		genMgrCpp += "/";
		genMgrCpp += strMgrClassName;
		genMgrCpp += ".cpp";
		std::ofstream osCpp (genMgrCpp.c_str());
		if (!osCpp) {
			rError ("open file for write error fileName = "<<genMgrCpp.c_str ());
			nRet = 2;
			break;
		}
		auto& os = osCpp;
		osCpp<<R"(#include ")"<<strMgrClassName<<R"(.h"
#include "strFun.h"
#include "loopHandleS.h"
#include "tSingleton.h"
#include "msg.h"

struct conEndPointT 
{
	std::string first;
	ServerIDType	targetHandle;
	uword       second;
	uword       userData;
	bool        bDef;
};

static int OnFrameCli (void* pArgS)
{
	auto pS = (logicServer*)pArgS;
	return pS->onFrameFun ();
}

void )"<<strMgrClassName<<R"(::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	do {
		auto& rFunS = getForMsgModuleFunS();
		rFunS = *pForLogic;
		auto fnCreateLoop = pForLogic->fnCreateLoop;
		auto fnRegMsg = pForLogic->fnRegMsg;
		int  nR = 0;
	)"
	<<R"(
	ubyte serverNum = )"<<serverNum<<R"(;
)";
	std::stringstream  osName;
	std::stringstream  osHS;
	std::stringstream  osNum;
	std::stringstream  osConNum;
	std::stringstream  osLN;
	std::stringstream  osCN;
	std::stringstream  osFps;
	std::stringstream  osSleep;
	std::stringstream  osSerPat;
	osName<<R"(const char* serverNameS[] = {)";
	osSerPat<<R"(logicServer* osSerPat[] = {)";
	osHS<<R"(ServerIDType  serverHS[] = {)";
	osFps<<R"(udword fpsSetpS[] = {)";
	osSleep<<R"(udword sleepSetpS[] = {)";
	osNum<<R"(    ubyte listenNumS []= {)";
	osConNum<<R"(    ubyte connectNumS []= {)";
	osCN<<R"(auto pConEndPointS = std::make_unique<
		std::unique_ptr<conEndPointT []>[]>(serverNum);
	)";
	
	osLN<<R"(auto pLenEndPointS = std::make_unique<
		std::unique_ptr<conEndPointT []>[]>(serverNum);
	)";
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto pName = rSS[i]->serverName ();
		auto pH = rSS[i]->strHandle();
		if (i) {
			osName<<",";
			osSerPat<<",";
			osFps<<",";
			osSleep<<",";
			osHS<<",";
			osNum<<",";
			osConNum<<",";
		}
		osName<<R"(")"<<pName<<R"(")";
		osSerPat<<R"(&m_)"<<pName;
		osHS<<pH;
		osFps<<rSS[i]->fpsSetp ();
		osSleep<<rSS[i]->sleepSetp();
		osNum<<(int)(rSS[i]->serverInfo().listenerNum);
		osConNum<<(int)(rSS[i]->serverInfo().connectorNum);
		
		if (rSS[i]->serverInfo().listenerNum) {
			osCN<<"pLenEndPointS ["<<(int)i<<"] = std::make_unique<conEndPointT []>("<<
				(int)rSS[i]->serverInfo().listenerNum<<R"();
	)";
			for (decltype (rSS[i]->serverInfo().listenerNum) k = 0; k < rSS[i]->serverInfo().listenerNum; k++) {
				osCN<<"auto& rEndP = pLenEndPointS["<<(int)i<<"]["<<(int)k<<R"(];
	rEndP.second = )"<<
			rSS[i]->serverInfo().listenEndpoint[k].port<<R"(;
	rEndP.userData = )"<<
			rSS[i]->serverInfo().listenEndpoint[k].userData<<R"(;
	rEndP.bDef = )"<<
			rSS[i]->serverInfo().listenEndpoint[k].bDef<<R"(;
		)";
			} // for
		} // if

		if (rSS[i]->serverInfo().connectorNum) {
			osCN<<"pConEndPointS ["<<(int)i<<"] = std::make_unique<conEndPointT []>("<<
				(int)rSS[i]->serverInfo().connectorNum<<R"();
	)";
			for (decltype (rSS[i]->serverInfo().connectorNum) k = 0; k < rSS[i]->serverInfo().connectorNum; k++) {
				auto pFS = rGloble.getServerByHandle (rSS[i]->serverInfo().connectEndpoint[k].szTarget);
				myAssert (pFS);
				if (!pFS) {
					rError (" handle name error : "<<rSS[i]->serverInfo().connectEndpoint[k].szTarget);
					nRet = 3;
					break;
				}
				osCN<<"auto& rEndP = pConEndPointS["<<(int)i<<"]["<<(int)k<<R"(];
	rEndP.first = ")"<<rSS[i]->serverInfo().connectEndpoint[k].ip<<
				R"(";
	rEndP.second = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].port<<R"(;
	rEndP.bDef = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].bDef<<R"(;
	rEndP.userData = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].userData<<R"(;
	rEndP.targetHandle = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].szTarget<<R"(;
		)";
			} // for
			if (nRet) {
				break;
			}
		} // if

		if (nRet) {
			break;
		}
	} // for

	if (nRet) {
		break;
	}

	osName<<"};";
	osSerPat<<"};";
	osHS<<"};";
	osNum<<"};";
	osFps<<"};";
	osSleep<<"};";
	osConNum<<"};";
	os<<"    "<<osName.str()<<std::endl;
	os<<"    "<<osSerPat.str()<<std::endl;
	os<<"    "<<osHS.str()<<std::endl;
	os<<"    "<<osSleep.str()<<std::endl;
	os<<"    "<<osFps.str()<<std::endl;
	os<<osNum.str()<<std::endl;
	os<<osConNum.str()<<std::endl;
	os<<osLN.str ();
	os<<osCN.str ();
	os<<R"(
	m_serverS = std::make_unique<logicServer* []> (serverNum);
	setServerNum (serverNum);
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		m_serverS [i] = osSerPat [i];
		)";
		os<<R"(auto& rServer = *(m_serverS[i]);
		auto& rInfo = rServer.serverInfo ();
		rServer.setServerName (serverNameS[i]);
		rInfo.handle = serverHS[i];
		rInfo.fpsSetp = fpsSetpS[i];
		rInfo.sleepSetp = sleepSetpS[i];
		rInfo.listenerNum = listenNumS [i];
		rInfo.connectorNum = connectNumS [i];
		// rInfo.fnOnAccept = rServer.sLogicOnAcceptSession;
		// rInfo.fnOnConnect = rServer.sLogicOnConnect;
		auto pThis = &rServer;
		for (decltype(rInfo.listenerNum) j = 0; j < rInfo.listenerNum; j++) {
			auto& ep = rInfo.listenEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), "0.0.0.0");
			ep.bDef = pLenEndPointS[i][j].bDef;
			ep.userData = pLenEndPointS[i][j].userData;
			ep.port = pLenEndPointS[i][j].second;
		}
		for (decltype(rInfo.connectorNum) j = 0; j < rInfo.connectorNum; j++) {
			auto& ep = rInfo.connectEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), pConEndPointS [i][j].first.c_str());
			ep.port = pConEndPointS[i][j].second;
			ep.userData = pConEndPointS[i][j].userData;
			ep.bDef = pConEndPointS[i][j].bDef;
			ep.targetHandle = pConEndPointS[i][j].targetHandle;
		}
		fnCreateLoop (serverNameS[i], serverHS[i], &rInfo, OnFrameCli, &rServer);
		rServer.onServerInitGen (pForLogic);
	}

	} while (0);
}
)";
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genH (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto genPath = rMod.genPath ();
		auto& rData = rMod.moduleData ();
		auto pModName = rData.moduleName ();

		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
		auto& rRpcFileMgr = pPmp->rpcFileS(); // tSingleton <rpcFileMgr>::single ();
		auto& rMsgMgr = pPmp->msgFileS();// tSingleton <msgFileMgr>::single ();
		std::string frameFunDir = rMod.frameFunDir ();
		std::string genMgrH = genPath;
		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		genMgrH += "/";
		genMgrH += strMgrClassName;
		genMgrH += ".h";
		std::ofstream osMgrH(genMgrH.c_str());
		if (!osMgrH) {
			rError ("open file for write error fileName = "<<genMgrH.c_str ());
			nRet = 2;
			break;
		}
		std::stringstream serInc;
		std::stringstream serVar;
		auto& rSS = rData.orderS ();
		auto serverNum = rSS.size ();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			serInc<<R"(#include ")"<<pName<<R"(.h"
)";
			serVar<<"    "<<pName<<R"(    m_)"<<pName<<R"(;
)";
		}
		osMgrH<<R"(#ifndef )"<<pModName<<R"(ServerMgr_h__
#define )"<<pModName<<R"(ServerMgr_h__
#include "logicServer.h"
)"<<serInc.str()<<R"(
class )"<<strMgrClassName<<R"( : public logicServerMgr
{
public:
	virtual void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
)"<<serVar.str()<<R"(
};
#endif
		)";
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto& rServer = *rSS[i];
		auto pName = rServer.serverName ();

		std::string serverHFile = rMod.genPath ();
		// std::string serverHFile = frameFunDir;
		serverHFile += "/";
		serverHFile += pName;
		serverHFile += ".h";
		/*
		auto bE = sIsPathExit (serverHFile.c_str());
		if (bE) {
			continue;
		}
		*/
		std::ofstream osH(serverHFile.c_str ());
		if (!osH) {
			rError ("open file: "<<serverHFile.c_str ()<<" error");
			nRet = 4;
			break;
		}
		std::set<std::string> procMsgSet;
		auto& rPMap = rServer.procMsgS ();
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto pG = pRpc->groupName ();
			procMsgSet.insert (pG);
		}

		osH<<R"(#ifndef )"<<pName<<R"(_h__
#define )"<<pName<<R"(_h__
#include "logicServer.h"
)";
		for (auto iter = procMsgSet.begin(); procMsgSet.end() != iter; ++iter) {
			osH<<R"(#include ")"<<iter->c_str()<<R"(Rpc.h")"<<std::endl;
		}
osH<<R"(
class )"<<pName<<R"( : public  logicServer
{
public:
	)"<<pName<<R"( ();
	int onFrameFun () override;
	int onServerInitGen(ForLogicFun* pForLogic) override;
	int onServerInit(ForLogicFun* pForLogic);
	int sendAddChannel (udword& token);
	int onAddChannelResult (udword token, udword result, channelKey& rKey);
	int sendDelChannel (channelKey & chK);
	int sendListenChannel (channelKey & chK);
	int sendQuitChannel (channelKey & chK);
	int sendPackToChannel(packetHead* pack, channelKey& chK, bool ntfMe = false);
	int sendMsgToChannel(CMsgBase& rMsg, channelKey& chK, bool ntfMe = false);
	void logicOnAcceptSession(SessionIDType sessionId, uqword userData)override;
	void logicOnConnect(SessionIDType sessionId, uqword userData)override;
	void*  userData ();
	void  setUserData (void* v);
	void onLoopBegin() override;
	void onLoopEnd() override;
private:
	void*  m_userData;
public:
)";
		for (auto ite = rPMap.begin(); rPMap.end() != ite; ++ite) {
			auto& rProcRpc = *(ite);
			auto rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto& rRpc = *pRpc;
			auto szMsgStructName = rProcRpc.bAsk ? 
				pRpc->askMsgName () : pRpc->retMsgName ();
			auto pMsg = rMsgMgr.findMsg (szMsgStructName);
			auto pDec = pMsg->msgFunDec ();
			osH<<"    "<<pDec<<";"<<std::endl;
		}
osH<<R"(
};
#endif
)";
		auto serverLoopBeginCppFile = frameFunDir;
		serverLoopBeginCppFile += "/";
		serverLoopBeginCppFile += pName;
		serverLoopBeginCppFile += "LoopBegin.cpp";
		auto bECLoopBegin = sIsPathExit (serverLoopBeginCppFile.c_str());
		if (bECLoopBegin) {
			continue;
		}
		std::ofstream osLoopBeginCpp (serverLoopBeginCppFile.c_str ()) ;
		if (!osLoopBeginCpp) {
			nRet = 5;
			rError ("open file: "<<serverLoopBeginCppFile.c_str ()<<" error");
			break;
		}
	osLoopBeginCpp<<R"(#include ")"<<pName<<R"(.h"
#include "gLog.h"

void )"<<pName<<R"(::onLoopBegin()
{
}

void )"<<pName<<R"(::onLoopEnd()
{
}
)";
		auto serverCppFile = frameFunDir;
		serverCppFile += "/";
		serverCppFile += pName;
		serverCppFile += ".cpp";
		auto bEC = sIsPathExit (serverCppFile.c_str());
		if (bEC) {
			continue;
		}
		std::ofstream osCpp (serverCppFile.c_str ()) ;
		if (!osCpp) {
			nRet = 5;
			rError ("open file: "<<serverCppFile.c_str ()<<" error");
			break;
		}
		osCpp<<R"(#include ")"<<pName<<R"(.h"
int )"<<pName<<R"(::onFrameFun ()
{
	return procPacketFunRetType_del;
}

void )"<<pName<<R"(::logicOnAcceptSession(SessionIDType sessionId, uqword userData)
{
}

void )"<<pName<<R"(::logicOnConnect(SessionIDType sessionId, uqword userData)
{
}

int )"<<pName<<R"(::onAddChannelResult (udword token, udword result, channelKey& rKey)
{
	int nRet = 0;
	do {
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: onServerInit(ForLogicFun* pForLogic)
{
	int nRet = 0;
	return nRet;
}
)";
		auto pH = rServer.strHandle();
	}
	if (nRet) {
		break;
	}
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genCpp (moduleGen& rMod)
{
    int   nRet = 0;
    do {
		auto genPath = rMod.genPath ();
		int nR = 0;
		nR = genMgrCpp (rMod);
		if (nR) {
			rError ("genMgrCpp  error nR = "<<nR);
			break;
		}
		nR = genServerReadOnlyCpp (rMod);
		if (nR) {
			rError ("genServerReadOnlyCpp error nR = "<<nR);
			break;
		}
	} while (0);
    return nRet;
}

int   moduleLogicServerGen:: genOnFrameFun (moduleGen& rMod, const char* szServerName)
{
    int   nRet = 0;
    do {
		std::string strFile = rMod.frameFunDir ();
		strFile += "/";
		strFile += szServerName;
		strFile += "FrameFun.cpp";
		std::ifstream is (strFile.c_str ());
		if (is) {
			break;
		}
		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			break;
		}
		auto& rData = rMod.moduleData ();
		auto pSer = rData.findServer (szServerName);
		myAssert (pSer);
		if (!pSer) {
			nRet = 2;
			break;
		}
		auto pFrameDec = pSer->frameFunDec ();
		auto pInitDec = pSer->initFunDec ();
		os<<R"(
#include "mainLoop.h"
#include "gLog.h"
#include "msg.h"
#include "logicServer.h"
#include "tSingleton.h"

)"<<pFrameDec<<R"(
{
	return procPacketFunRetType_del;
}
)"<<pInitDec<<R"(
{
	int nRet = 0;
	return nRet;
}
)";
    } while (0);
    return nRet;
}

static int sProcMsgReg (const char* pModName, const char* serverName,
		msgGroupFile* pGroup, rpcFile* pRpc,
		bool bAsk, const char* strHandle, const char* szMsgDir,
		std::ostream& os, std::ostream& ss)
{
	int nRet = 0;
	auto pFullMsg = pGroup->fullChangName ();
	auto szMsgStructName = bAsk ? pRpc->askMsgName () : pRpc->retMsgName ();
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
	myAssert (pPmp);
	auto& rMsgMgr = pPmp->msgFileS ();
	auto pMsg = rMsgMgr.findMsg (szMsgStructName);
	do {	
		myAssert (pMsg);
		if (!pMsg) {
			nRet = 1;
			break;
		}
		auto pMsgId = pMsg->strMsgId ();
		auto pPackFun = pMsg->packFunName ();

		auto askMsgStructName = pRpc->askMsgName ();
		auto retMsgStructName = pRpc->retMsgName ();
		auto isChannel = pRpc->isChannel ();
		auto msgProcFun = pMsg->msgFunName ();
		auto pAskMsg = rMsgMgr.findMsg (askMsgStructName);
		myAssert (pAskMsg);
		auto pRetMsg = rMsgMgr.findMsg (retMsgStructName);

		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		os<<R"(
static int )"<<pPackFun<<
				R"((packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    )";
	os<<pAskMsg->msgName ()<<R"( ask (nullptr);
    ask.fromPack(pAsk);
	)";
	bool bHaveRet = true;
	if (bAsk) {
		if (pRetMsg) {
			std::string strRetMsgName = pRetMsg->msgName ();
			os<<strRetMsgName<<R"( ret;

    )";
		} else {
			bHaveRet = false;
		}
	} else {
		myAssert (pRetMsg);
		os<<pRetMsg->msgName ()<<R"( ret (nullptr);
	ret.fromPack(pRet);
	)";
	}
	auto neetSe = pMsg->neetSession();
	if (neetSe) {
		if (bAsk) {
			os<<R"(	auto pPack = pAsk;
	)";
		} else {
			os<<R"(auto pPack = pRet;
	)";
		}
		os<<R"(auto pN = P2NHead(pPack);
	)";
	}
	os<<R"(tSingleton<)"<<strMgrClassName<<R"(>::single().m_)"
		<<serverName<<R"(.)"<<msgProcFun <<R"(()";
	auto askHasData = pAskMsg->hasData ();
	if (askHasData) {
		os<<R"(*ask.pack())";
	}
	if (pRetMsg) {
		auto retHasData = pRetMsg->hasData ();
		if (retHasData) {
			if (askHasData) {
				os<<", ";
			}
			os<<R"(*ret.pack())";
		}
	}
	
	if (neetSe) {
		os<<", pN->ubySrcServId, pPack->sessionID";
	}
	os<<R"();
	)";
	if (bAsk ) {
		if (bHaveRet) {
			os<<R"(pRet = ret.pop();
		)";
		}
	} else {
		os<<R"(ret.pop ();
)";
	}
	os<<R"(
	ask.pop ();
	return nRet;
}
)";
		ss<<R"(    fnRegMsg ()"<<strHandle<<", "<<pFullMsg<<
			R"(()"<<pMsgId<<R"(), )"<<pPackFun<<R"();
)";
		std::string strFile = szMsgDir;
		strFile += "/";
		auto msgFunName = pMsg->msgFunName ();
		strFile += msgFunName;
		strFile += ".cpp";
		bool is = sIsPathExit (strFile.c_str ());
		if (is) {
			break;
		}
		std::ofstream ps (strFile.c_str ());
		if (!ps) {
			rError ("open file : "<<strFile.c_str ()<<" error");
			nRet = 2;
			break;
		}
		auto pGSrcName = pGroup->rpcSrcFileName ();
		std::string strDec;
		
		pMsg->getClassMsgFunDec (serverName, strDec);
		ps<<R"(#include "tSingleton.h"
#include "msg.h"
#include "gLog.h"
#include "loopHandleS.h"
#include "logicServer.h"
#include ")"<<strMgrClassName<<R"(.h")"<<std::endl<<R"(
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<strDec<<R"(
{
	)";
	auto rpcName = pRpc->rpcName ();
		
	if (strcmp (rpcName, "addChannel") == 0) {
		if (bAsk) {
			ps<<R"(auto& rChS = channelS ();
	channelValue sidS;
	uqword uqwSe = srcSer;
	uqwSe <<= 32;
	uqwSe |= seId;
	sidS.insert(uqwSe);
	auto& rCh = *((channelKey*)rAsk.m_chKey);
	auto inRet = rChS.insert(std::make_pair(rCh, sidS));
	myAssert (inRet.second);
	if (inRet.second) {
		rRet.m_result = 0;
	} else {
		rRet.m_result = 1;
	}
)";	
		} else {
			ps<<R"(auto rSendS = channelSendS ();
	do {
		auto& rCh = *((channelKey*)rAsk.m_chKey);
	if (rRet.m_result) {
		onAddChannelResult (rAsk.m_token, rRet.m_result, rCh);
		rSendS.erase (rAsk.m_token);
		gError(" and channel error result = "<<rRet.m_result);
		break;
	}
	auto it = rSendS.find (rAsk.m_token);
	myAssert (rSendS.end () != it);
	if (rSendS.end () == it) {
		gError ("can not find sendS token = "<<rAsk.m_token);
		break;
	}
	uqword uqwSe = srcSer;
	uqwSe <<= 32;
	uqwSe |= seId;
	auto& rSidS = it->second;
	auto inRet = rSidS.insert(uqwSe);
	myAssert (inRet.second);
	if (!inRet.second) {
		gError ("addChennel ret have the save sessionId key = "<<uqwSe);
		onAddChannelResult (rAsk.m_token, 2, rCh);
		rSendS.erase (it);
		break;
	}
	auto rootNum = sizeof(s_RootSer) / sizeof(s_RootSer[0]);
	if (rSidS.size() == rootNum) {
		onAddChannelResult (rAsk.m_token, 0, rCh);
		rSendS.erase (it);
	}
	}while (0);
	)";
		}
	} else if (strcmp (rpcName, "delChannel") == 0) {
		if (bAsk) {
			ps<<R"(auto rSendS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	rSendS.erase(rCh);
	rRet.m_result = 0;
	)";
		}
	}  else if (strcmp (rpcName, "listenChannel") == 0) {
		if (bAsk) {
			ps<<R"(auto rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	rRet.m_result = 0;
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			rRet.m_result = 1;
			break;
		}
		auto& rSidS = it->second;
		uqword uqwSe = srcSer;
		uqwSe <<= 32;
		uqwSe |= seId;
		auto inRet = rSidS.insert(uqwSe);
		if (!inRet.second) {
			gWarn("alder in channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
		}
	} while (0);
	)";
		}
	} else if (strcmp (rpcName, "sendToChannel") == 0) {
		if (bAsk) {
			ps<<R"(auto rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	rRet.m_result = 0;
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			rRet.m_result = 1;
			break;
		}
		auto& rSidS = it->second;
		auto bodySize = rAsk.m_packNum - NetMsgLenSize;
		myAssert (bodySize >= 0);
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoop;
		for (auto it = rSidS.begin(); rSidS.end() != it; ++it) {
			uqword uqwK = *it;
			auto sessionId = (SessionIDType)(uqwK);
			uqwK>>=32;
			auto toSid = (serverIdType)uqwK;
			if (rAsk.m_excSender && toSid == srcSer && sessionId == seId ) {
				continue;
			}
			auto p = allocPacket (bodySize);
			auto pNN = P2NHead (p);
			memcpy(pNN, rAsk.m_pack, rAsk.m_packNum);
			pNN->ubySrcServId = srcSer;
			pNN->ubyDesServId = toSid;
			p->sessionID = sessionId;
			fnSendPackToLoop (p);
		}
	} while (0);
	)";
		}
	} else if (strcmp (rpcName, "quitChannel") == 0) {
		if (bAsk) {
			ps<<R"(auto rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	rRet.m_result = 0;
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			rRet.m_result = 1;
			break;
		}
		auto& rSidS = it->second;
		uqword uqwSe = srcSer;
		uqwSe <<= 32;
		uqwSe |= seId;
		rSidS.erase(uqwSe);
	} while (0);
	)";
		}
	}
ps<<R"(
}
)";
	} while (0);
	return nRet;
}

int   moduleLogicServerGen:: genPackFun (moduleGen& rMod, const char* szServerName)
{
    int   nRet = 0;
    do {
		std::string strFile = rMod.genPath ();
		strFile += "/msg/";
		myMkdir (strFile.c_str ());
		strFile += szServerName;
		strFile += "ProPacket.cpp";
		std::ofstream os (strFile.c_str ());
		if (!os) {
			rError ("open file : "<<strFile.c_str ());
			nRet = 1;
		}
		auto procMsgPath = rMod.procMsgPath ();
		os<<R"(#include "msgGroupId.h")"<<std::endl;
		os<<R"(#include "msgStruct.h")"<<std::endl;
		os<<R"(#include "loopHandleS.h")"<<std::endl;
		os<<R"(#include "tSingleton.h")"<<std::endl;
		os<<R"(#include "mainLoop.h")"<<std::endl;
		auto pServerF = rMod.moduleData ().findServer (szServerName);
		auto pModuleName = rMod.moduleData ().moduleName ();
		myAssert (pServerF);

		std::string strMgrClassName = pModuleName;
		strMgrClassName += "ServerMgr";
		os<<R"(#include ")"<<strMgrClassName<<R"(.h")"<<std::endl;
		
		std::string strMsgGen = procMsgPath;
		strMsgGen += "/";
		strMsgGen += szServerName;
		auto strHandle = pServerF->strHandle ();
		auto& rMap = pServerF->procMsgS ();
		using groupSet = std::set<std::string>;
		groupSet  groupS;
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
        for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rMgr = pPmp->rpcFileS ();
			auto pRpc = rMgr.findRpc (it->rpcName.c_str ());
			myAssert (pRpc);
			auto pGName = pRpc->groupName ();
			groupS.insert (pGName);
        }
		for (auto it = groupS.begin(); groupS.end() != it; ++it) {
			auto& pGName = *it;
			os<<R"(#include ")"<<pGName<<R"(MsgId.h")"<<std::endl;
			os<<R"(#include ")"<<pGName<<R"(Rpc.h")"<<std::endl;
			std::string strMsgFile = strMsgGen;
			strMsgFile += "/";
			strMsgFile += pGName;
			myMkdir (strMsgFile.c_str ());
		}
		os<<std::endl;
		auto pRegFunName = pServerF->regPackFunDec ();
		std::stringstream ss;
		ss<<std::endl<<pRegFunName<<R"(
{
	int nRet = 0;
)";
		auto& rGMgr = pPmp->msgGroupFileS ();
		auto& rRpcFileMgr = pPmp->rpcFileS ();
		for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rProcRpc = *(it);
			auto& rpcFileName = rProcRpc.rpcName;
			auto pRpc = rRpcFileMgr.findRpc (rpcFileName.c_str ());
			myAssert (pRpc);
			auto& rRpc = *pRpc;
			auto pGName = rRpc.groupName ();
			auto pGroup = rGMgr.findGroup (pGName);
			myAssert (pGroup);
			std::string strMsgFile = strMsgGen;
			strMsgFile += "/";
			strMsgFile += pGName;
			sProcMsgReg (pModuleName, szServerName, pGroup, pRpc, rProcRpc.bAsk,
					strHandle, strMsgFile.c_str (), os, ss);
		} // for
		ss<<R"(    return nRet;
}
)";
	os<<ss.str()<<std::endl;
    } while (0);
    return nRet;
}


int   moduleLogicServerGen:: genServerReadOnlyCpp (moduleGen& rMod)
{
    int   nRet = 0;
    do {
		auto& rData = rMod.moduleData ();
		auto genPath = rMod.genPath ();
		auto& rSS = rData.orderS ();
		auto serverNum = rSS.size ();
		for (decltype (serverNum) i = 0; i < serverNum; i++) {
			auto& rServer = *rSS[i];
			auto pName = rServer.serverName ();
			std::string strFile = genPath;
			strFile += "/";
			strFile += pName;
			strFile += "Gen.cpp";
			auto regPackFunName = rServer.regPackFunName ();
			auto regPackFunDec = rServer.regPackFunDec ();
			std::ofstream os (strFile.c_str ());
			os<<R"(#include ")"<<pName<<R"(.h"
#include "comMsgRpc.h"
#include "tSingleton.h"
#include "loopHandleS.h"
#include "msg.h"
#include "gLog.h"
#include "comMsgMsgId.h"
#include "msgGroupId.h"


int )"<<pName<<R"( :: onServerInitGen(ForLogicFun* pForLogic)
{
	int nRet = 0;
	do {
		)"<<regPackFunDec <<R"(;
		)"<<regPackFunName<<R"( (pForLogic->fnRegMsg);
		nRet = onServerInit(pForLogic);
	} while (0);
	return nRet;
}

void* )"<<pName<<R"( ::userData ()
{
    return m_userData;
}

void )"<<pName<<R"(::setUserData (void* v)
{
    m_userData = v;
}

)"<<pName<<R"(::)"<<pName<<R"(()
{
	m_userData = nullptr;
}

int )"<<pName<<R"( :: sendDelChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		delChannelAskMsg  askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendListenChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		listenChannelAskMsg askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendQuitChannel (channelKey & chK)
{
	int nRet = 0;
	do {
		quitChannelAskMsg askMsg;
		auto pU = askMsg.pack ();
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}
int )"<<pName<<R"( :: sendMsgToChannel(CMsgBase& rMsg, channelKey& chK, bool ntfMe)
{
	int  nRet = 0;
    do {
		auto pack = rMsg.toPack ();
		rMsg.pop ();
		nRet = sendPackToChannel (pack, chK, ntfMe);
    } while (0);
    return nRet;
}

int )"<<pName<<R"( :: sendPackToChannel(packetHead* pack, channelKey& chK, bool ntfMe)
{
	int nRet = 0;
	do {
		auto pSN = P2NHead(pack);
		pSN->ubySrcServId = serverId ();
		auto sendSize = AllNetHeadSize(pSN) + pSN->udwLength;
		auto pT = (sendToChannelAsk*)(0);
		auto uqwS = (uqword)&(pT->m_pack[0]);
		uqwS += sendSize;
		auto p = allocPacket ((udword)uqwS);
		auto pNN = P2NHead (p);
		pNN->ubySrcServId = serverId ();
		pNN->uwMsgID = comMsg2FullMsg(comMsgMsgId_sendToChannelAsk);
		auto pU = (sendToChannelAsk*)(N2User(pNN));
		memcpy (pU->m_pack, pSN, sendSize);
		pU->m_packNum = (udword)sendSize;
		pU->m_excSender = ntfMe ? 0 : 1;
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		auto fnFreePack = getForMsgModuleFunS ().fnFreePack;
		fnFreePack (pack);
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendPackToSomeServer (p, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendAddChannel (udword& token)
{
	int nRet = 0;
	addChannelAskMsg ask;
	auto pU = ask.pack();
	auto pK = (channelKey*)(&(pU->m_chKey[0]));
	auto nR = createUuid ((char*)pK);
	auto fnNextToken = getForMsgModuleFunS().fnNextToken;
	auto newToken = fnNextToken (serverId());
	auto& rSendS = channelSendS ();
	channelSendValue v;
	auto inRet = rSendS.insert(std::make_pair(newToken, v));
	myAssert (inRet.second);
	do {
		if (!inRet.second) {
			gError("insert channel token err: token = "<<newToken);
			nRet = 1;
			break;
		}
		pU->m_token = newToken;
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (ask, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		token = newToken;
	} while (0);
	return nRet;
}
)";
		} // for
    } while (0);
    return nRet;
}

