#include "xmlGlobalLoad.h"
#include "xmlMsgFileLoad.h"
#include "strFun.h"
#include "comFun.h"
#include "tSingleton.h"
#include "rLog.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/msgPmpFile.h"
#include "fromFileData/toolServerEndPointInfoMgr.h"

#include "xmlCommon.h"

xmlGlobalLoad:: xmlGlobalLoad ()
{
}

xmlGlobalLoad:: ~xmlGlobalLoad ()
{
}
/*
static int procConTarge (toolServerEndPointInfo& node)
{
	int nRet = 1;
	auto& rMap = tSingleton<moduleFileMgr>::single ().moduleS ();
	for (auto it = rMap.begin (); rMap.end () != it; ++it) {
		auto& rMod = *(it->second.get());
		auto pS = rMod.findServer (node.szTarget);
		if (pS) {
			strNCpy (node.szTarget, sizeof (node.szTarget),
					pS->strHandle ());
			nRet = 0;
			break;
		}
	}
	return nRet;
}
*/
int   xmlGlobalLoad:: secondProcess ()
{
    int   nRet = 0;
    do {
		auto& rGrobal = tSingleton<globalFile>::single ();
		auto& rRootV = rGrobal.rootServerS ();

		auto& rMap = tSingleton<moduleFileMgr>::single ().moduleS ();
		for (auto it = rMap.begin (); rMap.end () != it; ++it) {
			auto& rMod = *(it->second.get());
			auto& rSS = rMod.orderS ();
			for (auto ite = rSS.begin (); rSS.end () != ite; ++ite) {
				auto& rN = ite->get ()->serverInfo ();
				/*
				for (decltype (rN.connectorNum) i = 0; i < rN.connectorNum; i++) {
					procConTarge (rN.connectEndpoint[i]);
				}
				*/
				bool bFindDefLis = false;
				bool bFindDefCon = false;
				for (decltype (rN.listenerNum) i = 0; i < rN.listenerNum; i++) {
					if (rN.listenEndpoint[i].bDef) {
						bFindDefLis = true;
						break;
					}
				}
				for (decltype (rN.connectorNum) i = 0; i < rN.connectorNum; i++) {
					if (rN.connectEndpoint[i].bDef) {
						bFindDefCon = true;
						break;
					}
				}
				if (bFindDefLis && !bFindDefCon	) {
					rRootV.push_back (ite->get ()->strHandle());
				}
			}
		}
    } while (0);
    return nRet;
}
static const char* s_comMsg = R"(<?xml version='1.0' encoding='utf-8' ?>
<servers>
	<struct>
	</struct>
	<rpc>
		<comMsg addrType="1" cppDir="../libMsg/src" order="610">
			<addChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
					<token dataType="udword" />
				</ask>
				<ret neetSession="1">
					<result dataType="udword" />
				</ret>
			</addChannel>
			<delChannel>
				<ask>
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
				<ret>
					<result dataType="udword" />
				</ret>
			</delChannel>
			<listenChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
				<ret>
					<result dataType="udword" />
				</ret>
			</listenChannel>
			<quitChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
				<ret>
					<result dataType="udword" />
				</ret>
			</quitChannel>
			<sendToChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
					<excSender dataType="udword" commit="do not send ntf to me"/>
					<pack dataType="ubyte" length="1100000" haveSize="1" commit=""> </pack>
				</ask>
				<ret>
					<result dataType="udword" />
				</ret>
			</sendToChannel>
			<ntfExit>
				<ask>
					<exitType dataType="udword" />
				</ask>
				<ret neetSession="1">
					<result dataType="udword" />
				</ret>
			</ntfExit>
		</comMsg >
	</rpc>
</servers>)";

int  xmlGlobalLoad::xmlLoad (const char* szFile)
{
	int nRet = 0;
	// rInfo (" process xml : "<<szFile);
    rapidxml::file<> fdoc(szFile);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());
	rapidxml::xml_node<> *root = doc.first_node();
	auto& rGlobal = tSingleton<globalFile>::single ();
	auto& rMsgFileS =  rGlobal.msgFileS ();
	rapidxml::xml_node<char> * pAppS = nullptr;
	for(rapidxml::xml_node<char> * pRpc = root->first_node();  NULL != pRpc; pRpc = pRpc->next_sibling()) {
		auto pName = pRpc->name ();
		auto szPath = pRpc->value ();
		if(0 == strcmp(pName, "projectHome")) {
			rGlobal.setProjectHome (szPath);
		} else if(0 == strcmp(pName, "projectName")) {
			rGlobal.setProjectName (szPath);
		} else if(0 == strcmp(pName, "frameHome")) {
			rGlobal.setFrameHome(szPath);
		} else if(0 == strcmp(pName, "depIncludeHome")) {
			rGlobal.setDepIncludeHome(szPath);
		} else if(0 == strcmp(pName, "depLibHome")) {
			rGlobal.setDepLibHome(szPath);
		} else if(0 == strcmp(pName, "frameBinPath")) {
			rGlobal.setFrameBinPath(szPath);
		} else if(0 == strcmp(pName, "frameLibPath")) {
			rGlobal.setFrameLibPath(szPath);
		} else if(0 == strcmp(pName, "outPutPath")) {
			rGlobal.setOutPutPath(szPath);
		} else if(0 == strcmp(pName, "installPath")) {
			rGlobal.setInstallPath(szPath);
		} else if(0 == strcmp(pName, "frameInstallPath")) {
			rGlobal.setFrameInstallPath(szPath);
		} else if(0 == strcmp(pName, "defMsg")) {
			auto pPmp = std::make_shared <msgPmpFile> ();
			std::string pmpName = "defMsg";
			auto pPmpName = pRpc->first_attribute("pmpName");
			if (pPmpName) {
				pmpName = pPmpName->value ();
			}
			pPmp->setPmpName (pmpName.c_str());
			auto inR = rMsgFileS.insert(std::make_pair(pmpName, pPmp));
			myAssert (inR.second);
			auto& rFileV = pPmp->msgDefFileS ();
			rFileV.clear ();
			for (auto pNP = pRpc->first_attribute("file");
					pNP; pNP = pNP->next_attribute("file")) {
				// myAssert (pNP);
				// std::string pFilePath = pNP->value ();
				// pPmp->setDefFile (pNP->value ());
				rFileV.push_back (pNP->value ());
			}
		} else if(0 == strcmp(pName, "app")) {
			pAppS = pRpc;
		}
	}
	do {
		int nR = 0;
		
		std::unique_ptr<char[]>  dirBuf;
		nR = getDirFromFile (szFile, dirBuf);
		if (nR) {
			rError ("getDirFromFile error nR = "<<nR);
			nRet = 3;
			break;
		}
		for (auto it = rMsgFileS.begin (); rMsgFileS.end () != it; ++it) {
			xmlMsgFileLoad msgFileLoader;
			std::string strF = dirBuf.get ();
			strF += "/";
			auto pPmp = it->second;
			auto& rFileV = pPmp->msgDefFileS ();
			for (auto ite = rFileV.begin(); rFileV.end() != ite; ++ite) {
				auto strFile = strF;
				strFile += *ite;
				nR = msgFileLoader.xmlLoad (it->first.c_str(), strFile.c_str (), *pPmp);
				myAssert (0 == nR);
				if (0 == strcmp (it->first.c_str(), "defMsg")) {
					std::unique_ptr<char[]> comMsgBuf;
					strCpy (s_comMsg, comMsgBuf);
					nR = msgFileLoader.xmlLoadFromStr (it->first.c_str(), comMsgBuf.get(), *pPmp);
					myAssert (0 == nR);
				}
			}
		}
		if (!pAppS) {
			rError ("can not find appS node");
			nRet = 1;
			break;
		}
		nR = appSLoad (pAppS);
		if (nR) {
			rError ("appSLoad error nR = "<<nR);
			nRet = 2;
			break;
		}
		nR = secondProcess ();
		if (nR) {
			rError ("secondProcess error nR = "<<nR);
			nRet = 3;
			break;
		}
	} while (0);
	return nRet;
}

int   xmlGlobalLoad:: appSLoad (rapidxml::xml_node<char>* pAppS)
{
    int   nRet = 0;
	auto& rGlobal = tSingleton<globalFile>::single ();
	auto& rAppMgr = tSingleton<appFileMgr>::single ();
	auto& rMap = rAppMgr.appS ();

	for(rapidxml::xml_node<char> * pApp = pAppS->first_node();  pApp; pApp = pApp->next_sibling()) {
		std::shared_ptr<appFile> rApp;
		auto nR = onceAppLoad (pApp, rApp);
		if (0 != nR) {
			rError ("onceAppLoad ret err nR = "<<nR<<" name = "<<pApp->name ());
			nRet = 1;
			break;
		}
		rMap[rApp->appName ()] = rApp;
		rInfo ("after process app: "<<pApp->name ());
	}
    return nRet;
}
bool getKeyValue (char* szTxt, char*& key, char*& value)
{
	char* retS[3];
	int nR = strR(szTxt, '=', retS, 3);
	bool bRet = true;
	if (2 == nR) {
		key = retS[0];
		value = retS[1];
	} else {
		bRet = false;
	}
	return bRet;
}

int   xmlGlobalLoad:: onceAppLoad (rapidxml::xml_node<char>* pApp, std::shared_ptr<appFile>& rApp)
{
    int   nRet = 0;
	do {
		auto pName = pApp->name ();
		rInfo ("proc app name = "<<pName);
		
		rapidxml::xml_node<char>* pModuleS = pApp->first_node("module");
		if (!pModuleS) {
			nRet = 1;
			rError ("app : "<<pApp->name ()<<" not find moduleS ");
			break;
		}

		auto& rMap = tSingleton<appFileMgr>::single ().appS ();
		auto pA = std::make_shared<appFile> ();
		pA->setAppName (pName);
		auto nR = moduleSLoad (pModuleS, *pA);
		if (nR) {
			rError ("moduleSLoad  error nR = "<<nR<<" app name: "<<pName);
			nRet = 2;
			break;
		}
		rApp = pA;
		auto& rV = rApp->argS ();
		for (auto pArg = pApp->first_node ("appArg");
			pArg; pArg = pArg->next_sibling()) {
			std::unique_ptr<char[]> pBuff;
			strCpy(pArg->value (), pBuff);
			char* key = nullptr;
			char* value = nullptr;
			auto bGet = getKeyValue (pBuff.get(), key, value);
			if (bGet) {
				std::stringstream ss(key);
				std::string strT;
				ss>>strT;
				if (strT == "detachServerS" ) {
					auto bD = atoi (value) != 0;
					rApp->setDetachServerS (bD);
				}
			}
			rV.push_back (pArg->value ());
		}
    } while (0);
    return nRet;
}

int   xmlGlobalLoad:: moduleSLoad (rapidxml::xml_node<char>* pModuleS, appFile& rApp)
{
	int   nRet = 0;
	do {
		auto& rSet = rApp.moduleFileNameS ();
		auto& rMap = tSingleton<moduleFileMgr>::single ().moduleS ();
		moduleFileMgr::moduleMap  tmap;
		for(rapidxml::xml_node<char> * pM = pModuleS->first_node();  pM; pM = pM->next_sibling()) {
			auto pName = pM->name ();
			std::shared_ptr<moduleFile> rM;
			auto nR = onceModuleLoad (pM, rM, rApp);
			if (nR) {
				rError ("onceModuleLoad error nR = "<<nR<<" modualName = "<<pName);
				nRet = 1;
				break;
			}
			rM->setAppName (rApp.appName ());
			auto pRet = tmap.insert (std::make_pair(pName, rM));
			if (!pRet.second) {
				rError ("module have then same name in once app name = "<<pName);
				nRet = 2;
				break;
			}
			auto it = rMap.find (pName);
			if (rMap.end () != it) {
				rError ("module have then same name in global name = "<<pName);
				nRet = 3;
				break;
			}
		}

		if (nRet) {
			break;
		}
		for (auto it = tmap.begin (); tmap.end () != it; ++it) {
			rMap.insert (*it);
			rSet.insert (it->first);
		}
	} while (0);
	return nRet;
}

int   xmlGlobalLoad:: onceModuleLoad (rapidxml::xml_node<char>* pM, std::shared_ptr<moduleFile>& rM, appFile& rApp)
{
	int   nRet = 0;
	do {
		rInfo (" proc module : "<<pM->name ());
		auto newModule = std::make_shared <moduleFile> ();
		newModule->setModuleName (pM->name ());
		
		rapidxml::xml_node<char> *pServerS = nullptr;
		for(rapidxml::xml_node<char> * pSs = pM->first_node();  pSs; pSs= pSs->next_sibling()) {
			auto pName = pSs->name ();
			if(0 == strcmp(pName, "server")) {
				pServerS = pSs;
			}
		}
		if (!pServerS) {
			nRet = 1;
			rError ("module : "<<pM->name ()<<" not find serverS");
			break;
		}
		int nR = serverSLoad (pServerS, *newModule, rApp);
		if (nR) {
			rError (" serverSLoad  return err nR = "<<nR<<" moduleName = "<<pM->name ());
			nRet = 2;
			break;
		}
		rM = std::move (newModule);
	} while (0);
	return nRet;
}

static bool findDefCon (toolServerNode& node)
{
	bool bFind = false;
	for (decltype (node.connectorNum) i = 0; i < node.connectorNum; i++) {
		auto& endPoint = node.connectEndpoint [i]; 
		if (endPoint.bDef) {
			bFind = true;
			break;
		}
	}
	return bFind;
}
static bool moveFirst (moduleFile::serverOrder& rSS, int nSt)
{
	bool bFind = false;
	for (decltype (rSS.size()) i = nSt + 1; i < rSS.size(); i++) {
		auto& rIn = rSS[i].get()->serverInfo ();
		bFind = findDefCon (rIn);
		if (bFind) {
			auto t = rSS[i];
			rSS[i] = rSS[nSt];
			rSS[nSt] = t;
			bFind = true;
			break;
		}
	}
	return bFind;
}
static bool moveFirstListen (moduleFile::serverOrder& rSS, int nSt)
{
	bool bFind = false;
	for (decltype (rSS.size()) i = nSt + 1; i < rSS.size(); i++) {
		auto& rIn = rSS[i].get()->serverInfo ();
		if (rIn.listenerNum) {
			auto t = rSS[i];
			rSS[i] = rSS[nSt];
			rSS[nSt] = t;
			bFind = true;
			break;
		}
	}
	return bFind;
}

serverFile*     xmlGlobalLoad:: getServerByHandle(const char* szHandle)
{
    serverFile*     nRet = nullptr;
    do {
		auto& ssMap = allServer();
		for (auto it = ssMap.begin(); ssMap.end() != it; ++it) {
			auto pS = it->second.get();
			auto pSN = pS->strHandle ();
			if (strcmp (pSN, szHandle) == 0) {
				nRet = pS;
				break;
			}
		}
    } while (0);
    return nRet;
}

int   xmlGlobalLoad:: serverSLoad (rapidxml::xml_node<char>* pServerS,
		moduleFile& rM, appFile& rApp)
{
    int   nRet = 0;
	do {
		auto& rMap = rM.serverS ();
		auto& rSS = rM.orderS ();
		moduleFile::serverMap& tmp = rMap;
		auto& ssMap = allServer();
		for(rapidxml::xml_node<char> * pS = pServerS->first_node();  pS; pS = pS->next_sibling()) {
			auto rS = std::make_shared <serverFile>();
			rS->setModuleName (rM.moduleName());
			rS->setServerName (pS->name ());
			auto nR = onceServerLoad(pS, rS, rApp);
			if (nR) {
				rError ("onceServerLoad return error nR = "<<nR<<" server name: "<<pS->name ());
				nRet = 1;
				break;
			}
			std::string strServerName = pS->name();
			auto inRet = ssMap.insert (std::make_pair(strServerName, rS)); 
			myAssert(inRet.second);
			auto pRet = tmp.insert (std::make_pair(pS->name (), rS));
			tmp [pS->name ()] = rS;
			rSS.push_back (rS);
			
			std::unique_ptr <char[]> pSerName;
			strCpy (pS->name (), pSerName);
			toWord (pSerName.get ());
			std::string strFun = "reg";
			strFun += pSerName.get();
			strFun += R"(ProcPacketFun)";
			rS->setRegPackFunName (strFun.c_str ());
			std::string strFunName = R"(int )";
			strFunName += strFun;
			strFunName +=R"( (regMsgFT fnRegMsg))";
			rS->setRegPackFunDec (strFunName.c_str ());
			if (!pRet.second) {
				rError ("server have then same name in once module name = "<<pS->name ());
				nRet = 2;
				break;
			}
			rInfo ("temp insert server name : "<<pS->name ());
		}
		if (nRet) {
			break;
		}
		bool bFind = false;
		myAssert (!rSS.empty ());
		auto& rInfo = rSS.begin ()->get()->serverInfo ();
		bFind = findDefCon (rInfo);
		if (!bFind) {
			auto nSS = rSS.size ();
			for (decltype (nSS) i = 0; i < nSS - 1; i++) {
				auto bF = moveFirst (rSS, i);
				if (bF) {
					bFind = true;
				} else {
					break;
				}
			}
			if (!bFind) {
				auto& rInfo = rSS.begin ()->get()->serverInfo ();
				if (!rInfo.listenerNum) {
					for (decltype (nSS) i = 0; i < nSS - 1; i++) {
						auto bF = moveFirstListen (rSS, i);
						if (bF) {
							bFind = true;
						} else {
							break;
						}
					}
				}
			}
		}
	} while (0);
    return nRet;
}

static int procEndPoint (rapidxml::xml_node<char>* pXmlListen 
		, toolServerEndPointInfo& endPoint)
{
	int nRet = 0;
	// auto pIp = pXmlListen->first_attribute("ip");
	// myAssert (pIp);
	/*
	auto ipL = strlen (pIp->value());
	const auto c_maxIpLen = sizeof (endPoint.ip);
	myAssert (ipL + 1 < c_maxIpLen);
	strNCpy (endPoint.ip, c_maxIpLen, pIp->value());
	*/

	auto pPort = pXmlListen->first_attribute ("port");
	if (pPort) {
		endPoint.port = atoi (pPort->value ());
	} else {
		endPoint.port = 11000;
	}
	auto pEndPointName= pXmlListen->first_attribute ("name");
	myAssert (pEndPointName);
	if (pEndPointName) {
		strNCpy (endPoint.endPointName, sizeof (endPoint.endPointName), pEndPointName->value ());
	}
	auto pDefR = pXmlListen->first_attribute("defRoute");
	if (pDefR) {
		endPoint.bDef = atoi (pDefR->value ());
	} else {
		endPoint.bDef = true;
	}
	auto pTeag = pXmlListen->first_attribute("teag");
	if (pTeag) {
		endPoint.userData = atoi (pTeag->value ());
	} else {
		endPoint.userData = 0;
	}
	auto& rEndPointS = tSingleton<toolServerEndPointInfoMgr>::single().endPointS ();
	return nRet;
}
const char* szRootRpc[] = {"addChannel", "delChannel", "listenChannel"
			,"quitChannel", "sendToChannel"};
const char** getRpptRpc (int &num)
{
	num = sizeof (szRootRpc) / sizeof (szRootRpc[0]);
	return szRootRpc;
}

xmlGlobalLoad::allServerMap&  xmlGlobalLoad:: allServer ()
{
    return m_allServer;
}

int   xmlGlobalLoad:: onceServerLoad (rapidxml::xml_node<char>* pS,
		std::shared_ptr<serverFile> newServer, appFile& rApp)
{
    int   nRet = 0;
    do {
		auto& rGrobal = tSingleton<globalFile>::single ();
		// rInfo (" proc thServer : "<<pS->name ());
		auto szServerName = pS->name ();
		newServer->setServerName (szServerName);
		std::string strH = pS->name ();
		strH += "Handle";
		newServer->setStrHandle (strH.c_str());
		auto pFpsA = pS->first_attribute("fpsSetp");
		if (pFpsA) {
			auto dwSetp = atoi (pFpsA->value());
			newServer->setFpsSetp (dwSetp);
		}
		auto pSleepA = pS->first_attribute("sleepSetp");
		if (pSleepA) {
			auto dwSetp = atoi (pSleepA->value());
			newServer->setSleepSetp (dwSetp);
		}
		auto pAutoRun= pS->first_attribute("autoRun");
		if (pAutoRun) {
			auto autoRun = atoi (pAutoRun->value()) != 0;
			newServer->setAutoRun(autoRun);
			if (!autoRun) {
				auto pM = rApp.mainLoopServer();
				if (!pM) {
					rApp.setMainLoopServer (newServer->strHandle());
				}
			}
		}

		auto& rEndPointS = tSingleton<toolServerEndPointInfoMgr>::single().endPointS ();
		auto& rInfo = newServer->serverInfo ();
		for(auto pXmlListen = pS->first_node (); pXmlListen;
				pXmlListen = pXmlListen->next_sibling ()) {
			auto pNodeName = pXmlListen->name ();
			if (strcmp (pNodeName, "listen")) {
				continue;
			}
			const auto c_MaxListen = sizeof(rInfo.listenEndpoint) / 
				sizeof (rInfo.listenEndpoint[0]);
			myAssert (rInfo.listenerNum < c_MaxListen );
			auto& endPoint = rInfo.listenEndpoint[rInfo.listenerNum++];
			procEndPoint (pXmlListen, endPoint);
			auto pXmlIp = pXmlListen->first_attribute ("ip");
			if (pXmlIp) {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						pXmlIp->value ());
			} else {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						"0.0.0.0");
			}
			auto inRet = rEndPointS.insert (std::make_pair(endPoint.endPointName, std::make_pair(newServer.get(), &endPoint)));
			myAssert (inRet.second);
			/*
			auto pDefR = pXmlListen->first_attribute("defRoute");
			if (pDefR) {
				endPoint.bDef = atoi (pDefR->value ());
			}
			auto pTeag = pXmlListen->first_attribute("teag");
			if (pTeag) {
				endPoint.userData = atoi (pTeag->value ());
			} else {
				endPoint.userData = 0;
			}
			*/
		}
		for (auto pXmlCon = pS->first_node (); pXmlCon;
				pXmlCon = pXmlCon->next_sibling ()) {
			auto pNodeName = pXmlCon->name ();
			if (strcmp (pNodeName, "connect")) {
				continue;
			}
			const auto c_MaxCon = sizeof(rInfo.connectEndpoint) / 
				sizeof (rInfo.connectEndpoint[0]);
			myAssert (rInfo.connectorNum < c_MaxCon);
			auto& endPoint = rInfo.connectEndpoint[rInfo.connectorNum++];
			procEndPoint (pXmlCon, endPoint);
			/*
			auto pDefR = pXmlCon->first_attribute("defRoute");
			if (pDefR) {
				endPoint.bDef = atoi (pDefR->value ());
			}
			auto pXmlTarget = pXmlCon->first_attribute ("target");
			if (pXmlTarget) {
				strNCpy (endPoint.szTarget, sizeof (endPoint.szTarget),
						pXmlTarget->value ());
			}
			*/
			auto pXmlTarget = pXmlCon->first_attribute ("targetEndPoint");
			if (pXmlTarget) {
				strNCpy (endPoint.targetEndPoint, sizeof (endPoint.targetEndPoint),
						pXmlTarget->value ());
			}

			auto pXmlIp = pXmlCon->first_attribute ("ip");
			if (pXmlIp) {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						pXmlIp->value ());
			} else {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						"127.0.0.1");
			}
			auto inRet = rEndPointS.insert (std::make_pair(endPoint.endPointName, std::make_pair(newServer.get(), &endPoint)));
			myAssert (inRet.second);
			/*
			auto pXmlPort = pXmlCon->first_attribute ("port");
			if (pXmlPort) {
				endPoint.port = atoi (pXmlPort->value ());
			}
			auto pTeag = pXmlCon->first_attribute("teag");
			if (pTeag) {
				endPoint.userData = atoi (pTeag->value ());
			} else {
				endPoint.userData = 0;
			}
			*/
		} if (rInfo.connectorNum) {
			if (1 == rInfo.connectorNum) {
				auto& rEP = rInfo.connectEndpoint [0];
				myAssert (rEP.bDef);
			} else {
				decltype (rInfo.connectorNum) nFind = 0;
				for (decltype (rInfo.connectorNum) i = 0; i < rInfo.connectorNum; i++) {
					auto& rEP = rInfo.connectEndpoint [i];
					if (rEP.bDef) {
						nFind++;
					}
				}
				myAssert (1 == nFind);
			}
		}
		auto& rGlobalGen = tSingleton <globalFile>::single ();
		auto pPmp = rGlobalGen.findMsgPmp ("defMsg");
		auto& rMsgMgr = pPmp->msgFileS();
		auto& rProcS = newServer->procMsgS ();
		auto& rXmlCommon = tSingleton<xmlCommon>::single ();
		auto pXmlRrocRpc = pS->first_node ("procRpc");
		if (pXmlRrocRpc) {
			for (auto pXmlRpc = pXmlRrocRpc->first_node (); pXmlRpc; pXmlRpc = pXmlRpc->next_sibling ()) {
				procRpcNode node;
				node.retValue = "procPacketFunRetType_del";
				node.rpcName = pXmlRpc->name ();
				auto pAskType = pXmlRpc->first_attribute("askType");
				if (!pAskType) {
					rError ("rpc have no askType rpcName = "<<node.rpcName.c_str ());
					nRet = 2;
					break;
				}
				auto szAskType  = pAskType->value ();
				auto pRetValue = pXmlRpc->first_attribute("retValue");
				if (pRetValue) {
					node.retValue = pRetValue->value ();
				}
				if (0 == strcmp (szAskType, "ask")) {
					node.bAsk = true;
				} else {
					if (0 == strcmp (szAskType, "ret")) {
						node.bAsk = false;
					} else {
						rError ("rpc have the error asktype rpcName = "<<node.rpcName.c_str ()<<" askType = "<<szAskType);
						nRet = 3;
						break;
					}
				}

				auto inRet = rProcS.insert (node);
				if (!inRet.second) {
					rError ("have the same rpc rpcName = "<<node.rpcName.c_str ()<<" askType = "<<szAskType);
					nRet = 4;
					break;
				}
				rInfo ("insert rpc ok rpcname = "<<node.rpcName.c_str ()<<" askType = "<<szAskType);
				std::string strMsg = node.rpcName;
				if (node.bAsk) {
					strMsg += "Ask";
				} else {
					strMsg += "Ret";
				}
				auto pMsg = rMsgMgr.findMsg (strMsg.c_str ());
				myAssert (pMsg);
				auto pDefProc = pMsg->defProServerId ();
				bool bSet = pDefProc ? 0 == strcmp ("c_emptyLoopHandle", pDefProc) : false;
				if (bSet) {
					auto pHandle = newServer->strHandle ();
					myAssert (pHandle);
					pMsg->setDefProServerId (pHandle);
				}
			}
		}
		bool bIsRoot = newServer->isRoot ();
		int rootRpcNum = 0;
		auto ppRpc = getRpptRpc (rootRpcNum);
		for (decltype (rootRpcNum) i = 0; i < rootRpcNum; i++) {
			procRpcNode node;
			node.retValue = "procPacketFunRetType_del";
			node.bAsk = bIsRoot;
			node.rpcName = ppRpc[i];
			auto inRet = rProcS.insert (node);
			myAssert (inRet.second);
		}
		if (nRet) {
			break;
		}
		{
			procRpcNode node;
			node.retValue = "procPacketFunRetType_exitNow";
			node.bAsk = true;
			node.rpcName = "ntfExit";
			auto inRet = rProcS.insert (node);
			myAssert (inRet.second);
			node.retValue = "procPacketFunRetType_del";
			node.bAsk = false;
			node.rpcName = "ntfExit";
			inRet = rProcS.insert (node);
			myAssert (inRet.second);
		}
    } while (0);
    return nRet;
}

