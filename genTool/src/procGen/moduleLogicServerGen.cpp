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
#include "tSingleton.h"
#include "rLog.h"
#include "tSingleton.h"
#include <fstream>
#include <string>
#include <map>
#include <set>

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
			nR = genOnFrameFun (rMod, pName);
			if (nR) {
				nRet = 3;
				break;
			}
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
#include "msg.h"

struct conEndPointT 
{
	std::string first;
	ServerIDType	targetHandle;
	uword       second;
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
	osCN<<R"(auto pEndPointS = std::make_unique<
		std::unique_ptr<conEndPointT []>[]>(serverNum);
	)";
	osLN<<R"(    auto pPortS = std::make_unique<std::unique_ptr<uword[]>[]> (serverNum);
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
			// osCN<<",";
			// osLN<<",";
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
			osLN<<"pPortS ["<<(int)i<<"] = std::make_unique<uword[]>("<<
				(int)rSS[i]->serverInfo().listenerNum<<R"();
	)";
			for (decltype (rSS[i]->serverInfo().listenerNum) k = 0; k < rSS[i]->serverInfo().listenerNum; k++) {
				osLN<<"pPortS["<<(int)i<<"]["<<(int)k<<R"(] = )"<<
					rSS[i]->serverInfo().listenEndpoint[k].port<<R"(;
	)";
			} // for
		} // if
		if (rSS[i]->serverInfo().connectorNum) {
			osCN<<"pEndPointS ["<<(int)i<<"] = std::make_unique<conEndPointT []>("<<
				(int)rSS[i]->serverInfo().connectorNum<<R"();
	)";
			for (decltype (rSS[i]->serverInfo().connectorNum) k = 0; k < rSS[i]->serverInfo().connectorNum; k++) {
				osCN<<"auto& rEndP = pEndPointS["<<(int)i<<"]["<<(int)k<<R"(];
	rEndP.first = ")"<<rSS[i]->serverInfo().connectEndpoint[k].ip<<
				R"(";
	rEndP.second = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].port<<R"(;
	rEndP.bDef = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].bDef<<R"(;
	rEndP.targetHandle = )"<<
			rSS[i]->serverInfo().connectEndpoint[k].szTarget<<R"(;
		)";
			} // for
		} // if
	} // for

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
		for (decltype(rInfo.listenerNum) j = 0; j < rInfo.listenerNum; j++) {
			auto& ep = rInfo.listenEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), "0.0.0.0");
			ep.bRegHandle = false;
			ep.port = pPortS [i][j];
		}
		for (decltype(rInfo.connectorNum) j = 0; j < rInfo.connectorNum; j++) {
			auto& ep = rInfo.connectEndpoint [j];
			strNCpy (ep.ip, sizeof(ep.ip), pEndPointS [i][j].first.c_str());
			ep.bRegHandle = false;
			ep.port = pEndPointS[i][j].second;
			ep.bDef = pEndPointS[i][j].bDef;
			ep.targetHandle = pEndPointS[i][j].targetHandle;
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
private:
)"<<serVar.str()<<R"(
};
#endif
		)";
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto& rServer = *rSS[i];
		auto pName = rServer.serverName ();
		std::string serverHFile = frameFunDir;
		serverHFile += "/";
		serverHFile += pName;
		serverHFile += ".h";
		auto bE = isPathExit  (serverHFile.c_str());
		if (bE) {
			continue;
		}
		std::ofstream osH(serverHFile.c_str ());
		if (!osH) {
			rError ("open file: "<<serverHFile.c_str ()<<" error");
			nRet = 4;
			break;
		}
		osH<<R"(#ifndef )"<<pName<<R"(_h__
#define )"<<pName<<R"(_h__
#include "logicServer.h"
class )"<<pName<<R"( : public  logicServer
{
public:
	int onFrameFun () override;
	int onServerInitGen(ForLogicFun* pForLogic) override;
	int onServerInit(ForLogicFun* pForLogic);
};
#endif
)";
		auto serverCppFile = frameFunDir;
		serverCppFile += "/";
		serverCppFile += pName;
		serverCppFile += ".cpp";
		auto bEC = isPathExit  (serverCppFile.c_str());
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

static int sProcMsgReg (const char* serverName,
		msgGroupFile* pGroup, rpcFile* pRpc,
		bool bAsk, const char* strHandle, const char* szMsgDir,
		std::ostream& os, std::ostream& ss)
{
	int nRet = 0;
	auto pFullMsg = pGroup->fullChangName ();
	auto szMsgStructName = bAsk ? pRpc->askMsgName () : pRpc->retMsgName ();
	auto& rMsgMgr = tSingleton <msgFileMgr>::single ();
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

		// auto msgFunDec = pMsg->msgFunDec ();
		auto msgProcFun = pMsg->msgFunName ();
		auto& rMsgMgr = tSingleton <msgFileMgr>::single ();
		auto pAskMsg = rMsgMgr.findMsg (askMsgStructName);
		myAssert (pAskMsg);
		auto pRetMsg = rMsgMgr.findMsg (retMsgStructName);
		os<<R"(static int )"<<pPackFun<<
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
	// os<<msgFunDec<<R"(;
	auto pServer = 
    )"<<msgProcFun <<R"(()";
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
	os<<R"();
	)";
	if (bAsk ) {
		os<<R"(pRet = ret.pop();
	bool bInOnceProc = packInOnceProc(pAsk);
	if (bInOnceProc) {
		nRet =  procPacketFunRetType_doNotDel;
	}
		)";
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
		// std::ifstream is (strFile.c_str ());
		bool is = isPathExit (strFile.c_str ());
		if (is) {
			break;
		}
		// is.close ();
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
#include "logicServer.h"
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<strDec<<R"(
{
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
		os<<R"(#include "mainLoop.h")"<<std::endl;
		auto pServerF = rMod.moduleData ().findServer (szServerName);
		myAssert (pServerF);
		std::string strMsgGen = procMsgPath;
		strMsgGen += "/";
		strMsgGen += szServerName;
		auto strHandle = pServerF->strHandle ();
		auto& rMap = pServerF->procMsgS ();
		using groupSet = std::set<std::string>;
		groupSet  groupS;
        for (auto it = rMap.begin(); rMap.end() != it; ++it) {
			auto& rMgr = tSingleton <rpcFileMgr>::single ();
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
		auto& rGMgr = tSingleton <msgGroupFileMgr>::single ();
		auto& rRpcFileMgr = tSingleton <rpcFileMgr>::single ();
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
			sProcMsgReg (szServerName, pGroup, pRpc, rProcRpc.bAsk,
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
int )"<<pName<<R"( :: onServerInitGen(ForLogicFun* pForLogic)
{
	int nRet = 0;
	do {
		)"<<regPackFunDec <<R"(;
		)"<<regPackFunName<<R"( (pForLogic->fnRegMsg);
		nRet = onServerInit(pForLogic);
	} while (0);
	return nRet;
})";
		} // for
    } while (0);
    return nRet;
}

