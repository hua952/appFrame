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

int   moduleLogicServerGen:: genH (moduleGen& rMod)
{
	int   nRet = 0;
	do {
		auto genPath = rMod.genPath ();
		std::string strFilename = genPath;
		strFilename += "/logicServer.h";
		std::ofstream os(strFilename);
		if (!os) {
			rError ("open file for write error fileName = "<<strFilename.c_str ());
			nRet = 1;
			break;
		}
		os<<R"(#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
class logicServer
{
public:
    serverNode&   serverInfo ();
	const char*   serverName ();
	void          setServerName (const char* v);
	int onFrameFun ();
	int onServerInit(ForLogicFun* pForLogic);
private:
	serverNode   m_serverInfo;
	std::unique_ptr <char[]>  m_serverName;
};
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	int   procArgS (int nArgC, const char* argS[]);
	ubyte  serverNum ();
	void   setServerNum (ubyte ubyNum);
	logicServer*   serverS ();
private:
	ubyte  m_serverNum;
	std::unique_ptr <logicServer[]>  m_serverS;
};
#endif)";
	} while (0);
	return nRet;
}

static const char* g_szCppStaticTxt = R"(#include "logicServer.h"
#include "myAssert.h"
#include "gLog.h"
#include "strFun.h"
#include "loopHandleS.h"
#include <map>

serverNode&  logicServer:: serverInfo ()
{
	return m_serverInfo;
}

const char*  logicServer:: serverName ()
{
	return m_serverName.get ();
}

int logicServer::onFrameFun ()
{
	return procPacketFunRetType_del;
}

void  logicServer::setServerName (const char* v)
{
	strCpy (v, m_serverName);
}

logicServerMgr::logicServerMgr ()
{
}
logicServerMgr::~logicServerMgr ()
{
}

ubyte  logicServerMgr::serverNum ()
{
	return  m_serverNum;
}

void   logicServerMgr::setServerNum (ubyte ubyNum)
{
	m_serverNum = ubyNum;
}

logicServer*  logicServerMgr:: serverS ()
{
	return m_serverS.get ();
}

static int OnFrameCli(void* pArgS)
{
	auto pS = (logicServer*)pArgS;
	return pS->onFrameFun ();
}
 
int  logicServerMgr:: procArgS (int nArgC, const char* argS[])
{
	int nRet;
	return nRet;
}

int logicServer::onServerInit(ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto handle = serverInfo().handle;
)";
/*
	return nRet;
}
)";
*/
int   moduleLogicServerGen:: genCpp (moduleGen& rMod)
{
    int   nRet = 0;
    do {
		auto genPath = rMod.genPath ();
		std::string strFilename = genPath;	
		strFilename += "/logicServer.cpp";
		std::ofstream os(strFilename);
		if (!os) {
			rError ("open file for write error fileName = "<<strFilename.c_str ());
			nRet = 1;
			break;
		}
		os<<g_szCppStaticTxt;

	auto& rData = rMod.moduleData ();
	auto& rSS = rData.orderS ();
	auto serverNum = rSS.size ();
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		auto pName = rSS[i]->serverName ();
		auto pH = rSS[i]->strHandle();
		auto pInitFun = rSS[i]->initFunDec ();
		auto pFunName = rSS[i]->initFunName ();
		os<<R"(    if (handle == )"<<pH<<R"() {
		)"<<pInitFun<<R"(;
		return )"<<pFunName<<R"( (*this, pForLogic);
	}
)";
	}
	os<<"}";
		os<<R"(
struct conEndPointT 
{
	std::string first;
	ServerIDType	targetHandle;
	uword       second;
	bool        bDef;
};
void  logicServerMgr::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	int  nR = 0;
	)";
	os<<R"(
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
	osName<<R"(const char* serverNameS[] = {)";
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
			osFps<<",";
			osSleep<<",";
			osHS<<",";
			osNum<<",";
			// osCN<<",";
			// osLN<<",";
			osConNum<<",";
		}
		osName<<R"(")"<<pName<<R"(")";
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
	osHS<<"};";
	osNum<<"};";
	osFps<<"};";
	osSleep<<"};";
	osConNum<<"};";
	os<<"    "<<osName.str()<<std::endl;
	os<<"    "<<osHS.str()<<std::endl;
	os<<"    "<<osSleep.str()<<std::endl;
	os<<"    "<<osFps.str()<<std::endl;
	os<<osNum.str()<<std::endl;
	os<<osConNum.str()<<std::endl;
	os<<osLN.str ();
	os<<osCN.str ();
	os<<R"(
	m_serverS = std::make_unique<logicServer[]> (serverNum);
	for (decltype (serverNum) i = 0; i < serverNum; i++) {
		)";

		os<<R"(auto& rServer = m_serverS[i];
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
		rServer.onServerInit (pForLogic);
	}
}
)";
	/*
int nIndex = 0;
for (auto it = rSS.begin (); rSS.end () != it; ++it) {
	auto& rServer = *(it->get());
	auto pName = rServer.serverName ();
	auto regPackFunName = rServer.regPackFunName ();
	auto regPackFunDec = rServer.regPackFunDec ();
	auto pHd = rServer.strHandle ();
	auto frameFunName = rServer.frameFunName ();
	auto frameFunDec = rServer.frameFunDec ();
		os<<std::endl<<"    "<<frameFunDec<<R"(;
	fnCreateLoop (")"<<pName<<R"(", )"<<pHd<<R"(, &m_serverS[)"
		<<nIndex++<<R"(].serverInfo (), OnFrameCli, &rServer);
	
    )"<<regPackFunDec<<R"(;
	)"<<regPackFunName<<R"( (fnRegMsg);
	rServer.onServerInit (pForLogic);
)";
}
*/
/*
	auto initFunName = rServer.initFunName ();
	auto initFunDec = rServer.initFunDec ();
	os<<R"(
	)"<<initFunDec <<R"(;
	)"<<initFunName <<R"( (pForLogic);)";
}
	os<<R"(
}
)";
*/
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

static int sProcMsgReg (msgGroupFile* pGroup, rpcFile* pRpc,
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

		auto msgFunDec = pMsg->msgFunDec ();
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
	os<<msgFunDec<<R"(;
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
		auto pMsgFunDec = pMsg->msgFunDec ();
		ps<<R"(#include "tSingleton.h"
#include "msg.h"
#include "logicServer.h"
#include ")"<<pGSrcName<<R"(.h")"<<std::endl<<std::endl
		<<pMsgFunDec<<R"(
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
			sProcMsgReg (pGroup, pRpc, rProcRpc.bAsk,
					strHandle, strMsgFile.c_str (), os, ss);
		} // for
		ss<<R"(    return nRet;
}
)";
	os<<ss.str()<<std::endl;
    } while (0);
    return nRet;
}


