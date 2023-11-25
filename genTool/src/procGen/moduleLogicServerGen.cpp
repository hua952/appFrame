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
#include "fromFileData/toolServerEndPointInfoMgr.h"
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
		// auto& rEndPointS = tSingleton<toolServerEndPointInfoMgr>:: single().endPointS ();
		auto& rData = rMod.moduleData();
		auto& rSS = rData.orderS ();
		auto& rGloble = tSingleton<xmlGlobalLoad>::single ();
		auto& rGlobleFile = tSingleton<globalFile>::single ();
		auto& rConfig = tSingleton<configMgr>::single ();
		auto structBadyType = rConfig.structBadyType ();

		auto pPmp = rGlobleFile.findMsgPmp ("defMsg");
		auto serializePackFunStName = pPmp->serializePackFunStName ();

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
)";
		auto bH = rGlobleFile.haveServer ();
		if (bH) {
			os<<R"(#include "loopHandleS.h"
)";
		}
os<<R"(
#include "tSingleton.h"
#include "msg.h"
#include <map>
#include <string>
#include <cstring>
#include "myAssert.h"
#include "rpcInfo.h"

struct conEndPointT 
{
	std::string ip;
	std::string strEndPointName;
	ServerIDType	targetHandle;
	uword       port;
	uword       userData;
	bool        bDef;
	bool		rearEnd; 
	bool        regRoute;
};

static int OnFrameCli (void* pArgS)
{
	auto pS = (logicServer*)pArgS;
	return pS->willExit()?procPacketFunRetType_exitNow:pS->onFrameFun();
}

static void getEndPointAttr (int nArgC, char** argS, conEndPointT& rEndP)
{
	char* retS[3];
	for (decltype (nArgC) i = 0; i < nArgC; i++) {
		std::unique_ptr<char[]> pArg;
		strCpy (argS[i], pArg);
		auto nRet = strR (pArg.get(), '=', retS, 3);
		if (2 != nRet) {
			continue;
		}
		std::string strKey = retS[0];
		if (strKey != "endPoint") {
			continue;
		}
		const int c_2RetMax = 6;
		char* retS2[c_2RetMax];
		auto nRet2 = strR (retS[1], '!', retS2, c_2RetMax);
		myAssert (nRet2 < c_2RetMax);
		std::string strName;
		char* retS3[3];
		for (decltype (nRet2) j = 0; j < nRet2; i++) {
			auto nRet3 = strR (retS2[j], ':', retS3, 3);
			if (nRet3 != 2) {
				continue;
			}
			if (0 == strcmp (retS3[0], "name")) {
				strName = retS3[1];
				break;
			}
		}
		if (strName != rEndP.strEndPointName) {
			break;
		}
		for (decltype (nRet2) j = 0; j < nRet2; j++) {
			auto nRet3 = strR (retS2[j], ':', retS3, 3);
			if (nRet3 != 2) {
				continue;
			}
			if (0 == strcmp (retS3[0], "ip")) {
				rEndP.ip = retS3[1];
			} else if (0 == strcmp (retS3[0], "port")) {
				rEndP.port = (decltype (rEndP.port))(atoi(retS3[1]));
			} else if (0 == strcmp (retS3[0], "teag")) {
				rEndP.userData = (decltype (rEndP.userData))(atoi(retS3[1]));
			}
		}
	}
}

dword )"<<strMgrClassName<<R"(::afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	dword nRet = 0;
	do {
		m_pForLogicFun = pForLogic;
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
	std::stringstream  osAutoRun;
	std::stringstream  osSerPat;
	osName<<R"(const char* serverNameS[] = {)";
	osSerPat<<R"(logicServer* osSerPat[] = {)";
	osHS<<R"(ServerIDType  serverHS[] = {)";
	osFps<<R"(udword fpsSetpS[] = {)";
	osSleep<<R"(udword sleepSetpS[] = {)";
	osAutoRun<<R"(bool autoRunS[] = {)";
	osNum<<R"(    ubyte listenNumS []= {)";
	osConNum<<R"(    ubyte connectNumS []= {)";
	osCN<<R"(auto pConEndPointS = std::make_unique<
		std::unique_ptr<conEndPointT []>[]>(serverNum);
	)";
	
	osLN<<R"(auto pLenEndPointS = std::make_unique<
		std::unique_ptr<conEndPointT []>[]>(serverNum);
	)";

	auto& rENS = rGlobleFile.endPointGlobalS ();
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto pName = rSS[i]->serverName ();
		auto pH = rSS[i]->strHandle();
		if (i) {
			osName<<",";
			osSerPat<<",";
			osFps<<",";
			osSleep<<",";
			osAutoRun<<",";
			osHS<<",";
			osNum<<",";
			osConNum<<",";
		}
		osName<<R"(")"<<pName<<R"(")";
		osSerPat<<R"(&m_)"<<pName;
		osHS<<pH;
		osFps<<rSS[i]->fpsSetp ();
		osSleep<<rSS[i]->sleepSetp();
		osAutoRun<<rSS[i]->autoRun()?"true":"false";
		osNum<<(int)(rSS[i]->serverInfo().listenerNum);
		osConNum<<(int)(rSS[i]->serverInfo().connectorNum);
		
		if (rSS[i]->serverInfo().listenerNum) {
			osCN<<"pLenEndPointS ["<<(int)i<<"] = std::make_unique<conEndPointT []>("<<
				(int)rSS[i]->serverInfo().listenerNum<<R"();
	)";
			for (decltype (rSS[i]->serverInfo().listenerNum) k = 0; k < rSS[i]->serverInfo().listenerNum; k++) {
				auto ite = rENS.find (rSS[i]->serverInfo().listenEndpoint[k].endPointName);
				myAssert (rENS.end () != ite);
				std::stringstream ss;
				ss<<"rEndPL"<<(int)i<<(int)k;
				std::string rEndPLK = ss.str();
				osCN<<"auto& "<<rEndPLK<<" = pLenEndPointS["<<(int)i<<"]["<<(int)k<<R"(];
	)"<<rEndPLK<<R"(.port= )"<<
			/*rSS[i]->serverInfo().listenEndpoint[k].port*/ite->second<<R"(;
	)"<<rEndPLK<<R"(.userData = )"<<
			rSS[i]->serverInfo().listenEndpoint[k].userData<<R"(;
	)"<<rEndPLK<<R"(.bDef = )"<<
			rSS[i]->serverInfo().listenEndpoint[k].bDef<<R"(;
	)"<<rEndPLK<<R"(.strEndPointName = ")"<<rSS[i]->serverInfo().listenEndpoint[k].endPointName<<R"(";
		)";
			} // for
		} // if

		if (rSS[i]->serverInfo().connectorNum) {
			osCN<<"pConEndPointS ["<<(int)i<<"] = std::make_unique<conEndPointT []>("<<
				(int)rSS[i]->serverInfo().connectorNum<<R"();
	)";

			auto sendReg = rSS[i]->rearEnd () ? true : rSS[i]->regRoute ();
			std::string strTargetHandle = "EmptySessionID";
			for (decltype (rSS[i]->serverInfo().connectorNum) k = 0; k < rSS[i]->serverInfo().connectorNum; k++) {
				std::stringstream ss;
				ss<<"rEndPC"<<(int)i<<(int)k;
				std::string rEndPCK = ss.str();
				auto pFS = rGloble.getServerByListenEndPointName (rSS[i]->serverInfo().connectEndpoint[k].targetEndPoint);
				myAssert (pFS);
				if (!pFS) {
					rError(" can not getServerByListenEndPointName name = "<<rSS[i]->serverInfo().connectEndpoint[k].targetEndPoint);
					break;
				}
				if (sendReg) {
					strTargetHandle = pFS->strHandle ();
				}
				/*
				auto itF = rEndPointS.find (rSS[i]->serverInfo().connectEndpoint[k].endPointName);
				myAssert (itF != rEndPointS.end());
				auto pFS = itF->second.first;
				if (!pFS) {
					rError (" handle name error : "<<rSS[i]->serverInfo().connectEndpoint[k].endPointName);
					nRet = 3;
					break;
				}
				*/
				auto ite = rENS.find (rSS[i]->serverInfo().connectEndpoint[k].targetEndPoint);
				myAssert (rENS.end () != ite);
				osCN<<"auto& "<<rEndPCK<<" = pConEndPointS["<<(int)i<<"]["<<(int)k<<R"(];

	)"<<rEndPCK<<R"(.ip = ")"<<rSS[i]->serverInfo().connectEndpoint[k].ip<<
				R"(";
	)"<<rEndPCK<<R"(.port = )"<<
			/*rSS[i]->serverInfo().connectEndpoint[k].port*/ite->second<<R"(;
	)"<<rEndPCK<<R"(.bDef = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].bDef<<R"(;
	)"<<rEndPCK<<R"(.userData = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].userData<<R"(;
	)"<<rEndPCK<<R"(.rearEnd = )"<<rSS[i]->rearEnd () <<R"(;
	)"<<rEndPCK<<R"(.regRoute = )"<<rSS[i]->regRoute () <<R"(;
	)"<<rEndPCK<<R"(.targetHandle = )"<<strTargetHandle<<R"(;
	)"<<rEndPCK<<R"(.strEndPointName = ")"<<rSS[i]->serverInfo().connectEndpoint[k].endPointName<<R"(";
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
	osAutoRun<<"};";
	osConNum<<"};";
	os<<"    "<<osName.str()<<std::endl;
	os<<"    "<<osSerPat.str()<<std::endl;
	os<<"    "<<osHS.str()<<std::endl;
	os<<"    "<<osSleep.str()<<std::endl;
	os<<"    "<<osAutoRun.str()<<std::endl;
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
		rInfo.autoRun = autoRunS[i];
		rInfo.listenerNum = listenNumS [i];
		rInfo.connectorNum = connectNumS [i];
		auto pThis = &rServer;
		for (decltype(rInfo.listenerNum) j = 0; j < rInfo.listenerNum; j++) {
			getEndPointAttr (nArgC, argS, pLenEndPointS [i][j]);
			auto& ep = rInfo.listenEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), pLenEndPointS [i][j].ip.c_str());
			ep.bDef = pLenEndPointS[i][j].bDef;
			ep.logicData = pLenEndPointS[i][j].userData;
			ep.userData = (uqword)(&ep);
			ep.port = pLenEndPointS[i][j].port;
		}
		for (decltype(rInfo.connectorNum) j = 0; j < rInfo.connectorNum; j++) {
			getEndPointAttr (nArgC, argS,  pConEndPointS [i][j]);
			auto& ep = rInfo.connectEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), pConEndPointS [i][j].ip.c_str());
			ep.port = pConEndPointS[i][j].port;
			ep.logicData = pConEndPointS[i][j].userData;
			ep.userData = (uqword)(&ep);
			ep.bDef = pConEndPointS[i][j].bDef;
			ep.rearEnd = pConEndPointS[i][j].rearEnd ;
			ep.regRoute = pConEndPointS[i][j].regRoute;
			ep.targetHandle = pConEndPointS[i][j].targetHandle;
		}
		fnCreateLoop (serverNameS[i], serverHS[i], &rInfo, OnFrameCli, &rServer);
		rServer.onServerInitGen (pForLogic);
	}

	} while (0);
	return nRet;
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
	virtual dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
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
	int sendAddChannel (udword& token, channelKey& rCh);
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
		
		std::stringstream strRegRoute;
		/*
		auto sendReg = rServer.rearEnd () ? true : rServer.regRoute ();
		if (sendReg) {
			strRegRoute<<R"(	regRouteAsk askMsg;
	auto pN = askMsg.getPack ();
	pN->ubySrcServId = )"<<strHandle ()<<R"(;
	pN->ubyDesServId = (decltype(pN->ubyDesServId))userData;
	sendMsg (askMsg);
)";
		}
		*/
		osCpp<<R"(#include "comMsgRpc.h"
#include "gLog.h"
#include ")"<<pName<<R"(.h"
int )"<<pName<<R"(::onFrameFun ()
{
	return procPacketFunRetType_del;
}

void )"<<pName<<R"(::logicOnAcceptSession(SessionIDType sessionId, uqword userData)
{
}

void )"<<pName<<R"(::logicOnConnect(SessionIDType sessionId, uqword userData)
{
	gInfo ("connect finish userData = "<<userData);
)";osCpp<<R"(
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

static void   sOutListenChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(auto& rChS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	auto it = rChS.find(rCh);
	rRet.m_result = 0;
	do {
		if (rChS.end() == it) {
			gError("can not find channel chId = "<<std::hex<<rCh.key
			<<" "<<rCh.value);
			// rRet.m_result = 1;
			rRet.m_result =createChannel (rCh, srcSer, seId);
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
}
static void   sOutSendToChannel (bool bAsk, std::ostream& ps)
{
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
		auto  fnSendPackToLoop =  getForMsgModuleFunS ().fnSendPackToLoopForChannel;
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
			p->pAsk = 0;
			fnSendPackToLoop (p);
		}
	} while (0);
	)";
		}
}
static void   sOutRegRoute(bool bAsk, std::ostream& ps)
{
	/*
	if (bAsk) {
			ps<<R"(auto  fnReg =  getForMsgModuleFunS ().fnRegRoute;
	fnReg (serverId(), srcSer, seId, rAsk.m_nolyId);)";
		
	}
	*/
}

static void   sOutQuitChannel (bool bAsk, std::ostream& ps)
{
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
static void   sOutDelChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(auto rSendS = channelS ();
	auto& rCh = *((channelKey*)(&rAsk.m_chKey[0]));
	rSendS.erase(rCh);
	rRet.m_result = 0;
	)";
	}
}
static void   sOutNtfExit (bool bAsk, std::ostream& ps)
{
	if (!bAsk) {
		ps<<R"(	auto& es =  exitHandleS ();
	auto nu = es.erase (srcSer);
	myAssert (nu);
	if (es.empty ()) {
		setWillExit (true);
		gInfo("all other local server alder exit");
	}
	)";
	}
}

static void   sOutAddChannel (bool bAsk, std::ostream& ps)
{
	if (bAsk) {
		ps<<R"(
	auto& rCh = *((channelKey*)rAsk.m_chKey);
	rRet.m_result =createChannel (rCh, srcSer, seId);
		/*
		auto& rChS = channelS ();
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
	*/
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
}

static int sProcMsgReg (const char* pModName, const char* serverName,
		const procRpcNode& rProcRpc, const char* strHandle, const char* szMsgDir,
		std::ostream& os, std::ostream& ss)
{
	int nRet = 0;
	bool bAsk = rProcRpc.bAsk;
	auto& rGlobalFile = tSingleton<globalFile>::single ();
	auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
	myAssert (pPmp);
	auto& rRpcFileMgr = pPmp->rpcFileS(); // tSingleton <rpcFileMgr>::single ();
	auto pRpc = rRpcFileMgr.findRpc (rProcRpc.rpcName.c_str());
	myAssert (pRpc);
	auto& rRpc = *pRpc;
	auto pGName = rRpc.groupName ();
	auto& rGMgr = pPmp->msgGroupFileS ();
	auto pGroup = rGMgr.findGroup (pGName);
	myAssert (pGroup);
	auto pFullMsg = pGroup->fullChangName ();
	auto szMsgStructName = bAsk ? pRpc->askMsgName () : pRpc->retMsgName ();
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
		// auto isChannel = pRpc->isChannel ();
		auto msgProcFun = pMsg->msgFunName ();
		auto pAskMsg = rMsgMgr.findMsg (askMsgStructName);
		myAssert (pAskMsg);
		auto pRetMsg = rMsgMgr.findMsg (retMsgStructName);

		std::string strMgrClassName = pModName;
		strMgrClassName += "ServerMgr";
		os<<R"(
static int )"<<pPackFun<<
				R"((pPacketHead pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = )"<<rProcRpc.retValue<<R"(;
    )";
	os<<pAskMsg->msgName ()<<R"( ask (pAsk);
	// ask.unZip();
	pAsk = ask.getPack ();
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
		os<<pRetMsg->msgName ()<<R"( ret (pRet);
		// ret.unZip();
		pRet = ret.getPack ();
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
#include "myAssert.h"
)";
		auto bH = rGlobalFile.haveServer ();
		if (bH) {
			ps<<R"(#include "loopHandleS.h"
)";
		}

ps<<R"(#include "logicServer.h"
#include ")"<<strMgrClassName<<R"(.h")"<<std::endl<<R"(
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<strDec<<R"(
{
	)";
	auto rpcName = pRpc->rpcName ();
		
	if (strcmp (rpcName, "ntfExit") == 0) {
		sOutNtfExit (bAsk, ps);
	} else if (strcmp (rpcName, "addChannel") == 0) {
		sOutAddChannel (bAsk, ps);
	} else if (strcmp (rpcName, "delChannel") == 0) {
		sOutDelChannel (bAsk, ps);
	}  else if (strcmp (rpcName, "listenChannel") == 0) {
		sOutListenChannel (bAsk, ps);
	} else if (strcmp (rpcName, "sendToChannel") == 0) {
		sOutSendToChannel (bAsk, ps);
	} else if (strcmp (rpcName, "quitChannel") == 0) {
		sOutQuitChannel (bAsk, ps);
	} else if (strcmp (rpcName, "regRoute") == 0) {
		sOutRegRoute(bAsk, ps);
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
			sProcMsgReg (pModuleName, szServerName, rProcRpc,
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
		sendToAllGateServer (askMsg);
		/*
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		*/
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
		sendToAllGateServer (askMsg);
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
		sendToAllGateServer (askMsg);
		/*
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (askMsg, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		*/
	} while (0);
	return nRet;
}
int )"<<pName<<R"( :: sendMsgToChannel(CMsgBase& rMsg, channelKey& chK, bool ntfMe)
{
	int  nRet = 0;
    do {
		// rMsg.zip();
		// rMsg.toPack();
		auto pack = rMsg.pop();
		nRet = sendPackToChannel (pack, chK, ntfMe);
    } while (0);
    return nRet;
}

int )"<<pName<<R"( :: sendPackToChannel(packetHead* pack, channelKey& chK, bool ntfMe)
{
	int nRet = 0;
	do {
		auto fnFreePack = getForMsgModuleFunS ().fnFreePack;
		/*
		auto toNetPack = getForMsgModuleFunS().toNetPack;
		packetHead* pNew = nullptr;
		toNetPack (pack, pNew);
		if (pNew) {
			fnFreePack (pack);
			pack = pNew;
		}
		*/
		auto pSN = P2NHead(pack);
		pSN->ubySrcServId = serverId ();
		auto sendSize = NetHeadSize + pSN->udwLength;
		auto pT = (sendToChannelAsk*)(0);
		auto uqwS = (uqword)&(pT->m_pack[0]);
		uqwS += sendSize;
		auto p = allocPacket ((udword)uqwS);
		p->pAsk = 0;
		p->sessionID = EmptySessionID;
		auto pNN = P2NHead (p);
		pNN->ubySrcServId = serverId ();
		pNN->ubyDesServId = c_emptyLoopHandle;
		pNN->uwMsgID = comMsg2FullMsg(comMsgMsgId_sendToChannelAsk);
		auto pU = (sendToChannelAsk*)(N2User(pNN));
		memcpy (pU->m_pack, pSN, sendSize);
		pU->m_packNum = (udword)sendSize;
		pU->m_excSender = ntfMe ? 0 : 1;
		auto pK = (channelKey*)(&pU->m_chKey[0]);
		*pK = chK;
		fnFreePack (pack);
		auto fnSendPackToLoopForChannel = getForMsgModuleFunS ().fnSendPackToLoopForChannel;
		fnSendPackToLoopForChannel (p);
		/*
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendPackToSomeServer (p, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		*/
	} while (0);
	return nRet;
}

int )"<<pName<<R"( :: sendAddChannel (udword& token, channelKey& rCh)
{
	int nRet = 0;
	addChannelAskMsg ask;
	auto pU = ask.pack();
	auto pK = (channelKey*)(&(pU->m_chKey[0]));
	*pK = rCh;
	//auto nR = createUuid ((char*)pK);
	auto fnNextToken = getForMsgModuleFunS().fnNextToken;
	auto newToken = fnNextToken (serverId());
	/*
	auto& rSendS = channelSendS ();
	channelSendValue v;
	auto inRet = rSendS.insert(std::make_pair(newToken, v));
	myAssert (inRet.second);
	*/
	do {
		/*
		if (!inRet.second) {
			gError("insert channel token err: token = "<<newToken);
			nRet = 1;
			break;
		}
		*/
		pU->m_token = newToken;
		sendToAllGateServer (ask);
		/*
		auto rootServerNum = sizeof (s_RootSer) / sizeof (s_RootSer[0]);
		auto nR = sendMsgToSomeServer (ask, s_RootSer, rootServerNum);
		if (nR) {
			nRet = 2;
			gError("sendMsgToSomeServer error nR = "<<nR);
			break;
		}
		*/
		token = newToken;
	} while (0);
	return nRet;
}
)";
		} // for
    } while (0);
    return nRet;
}

