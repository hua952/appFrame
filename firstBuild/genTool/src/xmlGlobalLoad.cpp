#include "xmlGlobalLoad.h"
#include "xmlMsgFileLoad.h"
#include "strFun.h"
#include "comFun.h"
#include "tSingleton.h"
#include "rLog.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/appFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/msgPmpFile.h"
#include "fromFileData/toolServerEndPointInfoMgr.h"
#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <sstream>
#include<unordered_map>
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
	<powerCom>0</powerCom>
	<struct>
	</struct>
	<rpc>
		<comMsg addrType="1" cppDir="../libMsg/src" order="610">
			<addChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
					<token dataType="udword" />
				</ask>
			</addChannel>
			<delChannel>
				<ask>
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
			</delChannel>
			<listenChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
			</listenChannel>
			<quitChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
				</ask>
			</quitChannel>
			<sendToChannel>
				<ask neetSession="1">
					<chKey dataType="ubyte" length="16" haveSize="0"/>
					<excSender dataType="udword" commit="do not send ntf to me"/>
					<pack dataType="ubyte" length="110000" haveSize="1" commit=""> </pack>
				</ask>
			</sendToChannel>
			<ntfExit>
				<ask>
					<exitType dataType="udword" />
				</ask>
				<ret neetSession="1">
					<result dataType="udword" />
				</ret>
			</ntfExit>
			<regRoute>
				<ask neetSession="1">
					<nolyId dataType="udword" />
					<signature dataType="ubyte" length="256" haveSize="1" commit="" />
				</ask>
			</regRoute>
		</comMsg >
	</rpc>
)";

int   xmlGlobalLoad:: perProc(rapidxml::xml_document<>& doc)
{
    int   nRet = 0;
    do {
		rapidxml::xml_node<> *root = doc.first_node();
		myAssert (root);
		auto pAppS = root->first_node("app");
		myAssert (pAppS);
		std::set<std::string> appNameS;
		std::string netServerNum = "4";
		auto pNetNum = pAppS->first_attribute("netNum");
		if (pNetNum) {
			netServerNum = pNetNum->value();
		}
		auto findGate = false;
		for(rapidxml::xml_node<char> * pApp = pAppS->first_node();  NULL != pApp; pApp= pApp->next_sibling()) {
			auto inRet = appNameS.insert(pApp->name());
			myAssert (inRet.second);
			int appNetType = 0;
			auto pA = pApp->first_attribute("appNetType");
			if (pA) {
				appNetType = atoi(pA->value());
			}
			if (1 == appNetType) {
				myAssert(!findGate);
				findGate = true;
			}
		}
		if (!findGate) {
			auto gateServerName = "gateAuto";
			auto it = appNameS.find(gateServerName);
			myAssert (appNameS.end() == it);
			rapidxml::xml_node<>* pGateNode = doc.allocate_node(rapidxml::node_element, gateServerName);
			myAssert (pGateNode);
			pAppS->append_node(pGateNode);
			pGateNode->append_attribute(doc.allocate_attribute("appNetType", "1"));
			rapidxml::xml_node<>* pModuleS = doc.allocate_node(rapidxml::node_element, "module");
			myAssert (pModuleS);
			pGateNode->append_node(pModuleS);
			rapidxml::xml_node<>* pM= doc.allocate_node(rapidxml::node_element, "gateM");
			pModuleS->append_node(pM);
			rapidxml::xml_node<>* pServerS = doc.allocate_node(rapidxml::node_element, "server");
			pM->append_node(pServerS);
		}
		for(rapidxml::xml_node<char> * pApp = pAppS->first_node();  NULL != pApp; pApp= pApp->next_sibling()) {
			auto pModuleS = pApp->first_node("module");
			myAssert(pModuleS);
			auto pM = pModuleS->first_node();
			myAssert (pM);
			myAssert (!pM->next_sibling());
			auto pServerS = pM->first_node("server");
			myAssert (pServerS);
			auto findNetServer = false;
			std::set<std::string> serverNameS;
			int appNetType = 0;
			auto pA = pApp->first_attribute("appNetType");
			if (pA) {
				appNetType = atoi(pA->value());
			}
			for(rapidxml::xml_node<char> * pS = pServerS->first_node();  NULL != pS; pS = pS->next_sibling()) {
				auto inS = serverNameS.insert(pS->name());
				myAssert (inS.second);
				auto pA = pS->first_attribute("route");
				if (pA) {
					bool bR = atoi(pA->value());
					if (bR) {
						findNetServer = true;
						break;
					}
				}
			}
			if (!findNetServer && appNameS.size() > 1) {
				auto netServerName = "netThAuto";
				auto it = serverNameS.find(netServerName);
				myAssert (serverNameS.end() == it);

				rapidxml::xml_node<>* pNetNode = doc.allocate_node(rapidxml::node_element, netServerName);
				myAssert (pNetNode);
				pServerS->append_node(pNetNode);
				pNetNode->append_attribute(doc.allocate_attribute("route", "1"));
				std::string netNum = netServerNum;
				if (0 == appNetType) {
					netNum = "1";
				}
				static char szOpenNum[4];
				strNCpy (szOpenNum, 4, netNum.c_str());
				pNetNode->append_attribute(doc.allocate_attribute("openNum", szOpenNum));
			}
		}
		
    } while (0);
    return nRet;
}

int  xmlGlobalLoad::xmlLoad (const char* szFile)
{
	int nRet = 0;
	rInfo (" sizeof(std::string) =  "<<sizeof(std::string));
	rInfo (" sizeof(std::stringstream) =  "<<sizeof(std::stringstream));
	rInfo (" sizeof(std::vector<int>) =  "<<sizeof(std::vector<int>));
	rInfo (" sizeof(std::list<int>) =  "<<sizeof(std::list<int>));
	rInfo (" sizeof(std::deque<int>) =  "<<sizeof(std::deque<int>));
	rInfo (" sizeof(std::set<int>) =  "<<sizeof(std::set<int>));
	rInfo (" sizeof(std::map<int, int>) =  "<<sizeof(std::map<int, int>));
	rInfo (" sizeof(std::unordered_map<int, int>) =  "<<sizeof(std::unordered_map<int, int>));

    rapidxml::file<> fdoc(szFile);
	rapidxml::xml_document<> doc;
	doc.parse<0>(fdoc.data());
	int nR = 0;
	nR = perProc(doc);

	do {
		if (nR) {
			nRet = 1;
			break;
		}
		rapidxml::xml_node<> *root = doc.first_node();
		auto& rGlobal = tSingleton<globalFile>::single ();
		auto& rMsgFileS =  rGlobal.msgFileS ();
		auto& rV = rGlobal.argS ();
		for (auto pArg = root->first_node ("appArg");
				pArg; pArg = root->next_sibling()) {
			rV.push_back (pArg->value ());
		}
		rapidxml::xml_node<char> * pAppS = nullptr;
		for(rapidxml::xml_node<char> * pRpc = root->first_node();  NULL != pRpc; pRpc = pRpc->next_sibling()) {
			auto pName = pRpc->name ();
			auto szPath = pRpc->value ();
			if(0 == strcmp(pName, "projectDir")) {
				rGlobal.setProjectDir (szPath);
			} else if(0 == strcmp(pName, "projectName")) {
				rGlobal.setProjectName (szPath);
			} else if(0 == strcmp(pName, "configDef")) {
				rGlobal.setConfigDef(szPath);
			}  else if(0 == strcmp(pName, "configFile")) {
				rGlobal.setConfigFile(szPath);
			} else if(0 == strcmp(pName, "frameInstallPath")) {
				rGlobal.setFrameInstallPath(szPath);
			} else if(0 == strcmp(pName, "thirdPartyDir")) {
				rGlobal.setThirdPartyDir(szPath);
			} else if(0 == strcmp(pName, "endPoint")) {
				procEndPointS (pRpc);
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
					rFileV.push_back (pNP->value ());
				}
			} else if(0 == strcmp(pName, "app")) {
				pAppS = pRpc;
			}
		}
		
		std::unique_ptr<char[]>  dirBuf;
		nR = getDirFromFile (szFile, dirBuf);
		if (nR) {
			rError ("getDirFromFile error nR = "<<nR);
			nRet = 3;
			break;
		}
		rGlobal.setXmlDir (dirBuf.get());
		auto pPmpT = rMsgFileS.find ("defMsg");
		if (pPmpT == rMsgFileS.end()) {
			auto pT = std::make_shared <msgPmpFile> ();
			pT->setPmpName ("defMsg");
			auto inR = rMsgFileS.insert(std::make_pair("defMsg", pT));
			myAssert (inR.second);
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
			}
			if (0 == strcmp (it->first.c_str(), "defMsg")) {
				std::unique_ptr<char[]> comMsgBuf;
				strCpy (s_comMsg, comMsgBuf);
				nR = msgFileLoader.xmlLoadFromStr (it->first.c_str(), comMsgBuf.get(), *pPmp);
				myAssert (0 == nR);
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

int   xmlGlobalLoad:: procEndPointS (rapidxml::xml_node<char>* pEndPointS)
{
    int   nRet = 0;
    do {
		auto& rES = tSingleton<globalFile>::single ().endPointGlobalS ();
		for (auto pC = pEndPointS->first_node (); pC; pC = pC->next_sibling ()) {
			auto pPort = pC->first_attribute ("port");
			myAssert (pPort);
			auto port = (uword)(atoi (pPort->value()));
			auto inRet = rES.insert(std::make_pair (pC->name (), port));
			myAssert (inRet.second);
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
		
		auto& rMap = tSingleton<appFileMgr>::single ().appS ();
		auto pA = std::make_shared<appFile> ();
		pA->setAppName (pName);
		rApp = pA;
		auto pRearEnd = pApp->first_attribute("appNetType");
		if (pRearEnd) {
			auto dwSetp = atoi (pRearEnd->value());
			pA->setNetType((ubyte)dwSetp);
		}
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
		rapidxml::xml_node<char>* pModuleS = pApp->first_node("module");
		if (!pModuleS) {
			rInfo ("app : "<<pApp->name ()<<" not find moduleS ");
			break;
		}
		auto nR = moduleSLoad (pModuleS, *pA);
		if (nR) {
			rError ("moduleSLoad  error nR = "<<nR<<" app name: "<<pName);
			nRet = 2;
			break;
		}
		auto& rMS = rApp->moduleFileNameS ();
		auto haveNetServer = rApp->haveNetServer ();
		auto haveServer = rApp->haveServer ();
		if (!haveNetServer && haveServer) {
			auto pM = tSingleton<moduleFileMgr>::single().findModule (rMS.begin()->c_str());
			myAssert (pM);
			
		}
		auto pM = rApp->mainLoopServer ();
		if (!pM) {
			const char* szMain = nullptr;
			if (!rMS.empty()) {
				for (auto it = rMS.begin (); it != rMS.end (); it++) {
					auto pM = tSingleton<moduleFileMgr>::single().findModule (it->c_str());
					if (!pM) {
						auto& pO = pM->orderS ();
						for (auto ite = pO.begin (); ite != pO.end (); ite++) {
							auto rIte = ite->get();
							auto openNum = rIte->openNum ();
							if (1 == openNum) {
								szMain = rIte->strTmpHandle ();
								rApp->setMainLoopServer (szMain);
								rIte->setAutoRun(false);
								break;
							}
						}
					}
					if (szMain) {
						break;
					}
				}
				
			}
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

serverFile*  xmlGlobalLoad:: getServerByListenEndPointName(const char* szEnName)
{
    serverFile*     nRet = nullptr;
	do {
		auto& ssMap = allServer();
		for (auto it = ssMap.begin(); ssMap.end() != it; ++it) {
			auto pS = it->second.get();
			auto& rNode = pS->serverInfo ();
			for (decltype (rNode.listenerNum) i = 0; i < rNode.listenerNum; i++) {
				auto& rE = rNode.listenEndpoint [i];
				if (strcmp (rE.endPointName, szEnName) == 0) {
					nRet = pS;
					break;
				}
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
			strFunName +=R"( (regMsgFT fnRegMsg, ServerIDType serId))";
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
	auto& rGrobal = tSingleton<globalFile>::single ();
	/*
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
	*/
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
	// auto& rEndPointS = tSingleton<toolServerEndPointInfoMgr>::single().endPointS ();
	return nRet;
}
const char* szRootRpc[] = {"addChannel", "delChannel", "listenChannel"
			,"quitChannel", "sendToChannel"/*, "regRoute"*/};
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
		auto szServerName = pS->name ();
		newServer->setServerName (szServerName);
		std::string strH = pS->name ();
		std::string strTmpHandle = pS->name ();
		strH += "Handle";
		newServer->setStrHandle (strH.c_str());
		strTmpHandle += "Tmp";
		strTmpHandle += "Handle";
		newServer->setStrTmpHandle (strTmpHandle.c_str());
		auto pFpsA = pS->first_attribute("fpsSetp");
		if (pFpsA) {
			auto dwSetp = atoi (pFpsA->value());
			newServer->setFpsSetp (dwSetp);
		}
		auto pOpenNum = pS->first_attribute("openNum");
		if (pOpenNum) {
			auto dwSetp = atoi (pOpenNum->value());
			newServer->setOpenNum(dwSetp);
		}
		auto pRoute = pS->first_attribute("route");
		if (pRoute) {
			auto dwSetp = atoi (pRoute->value());
			newServer->setRoute(dwSetp);
		}
		auto pSleepA = pS->first_attribute("sleepSetp");
		if (pSleepA) {
			auto dwSetp = atoi (pSleepA->value());
			newServer->setSleepSetp (dwSetp);
		}
		auto pRearEnd = pS->first_attribute("rearEnd");
		if (pRearEnd) {
			auto dwSetp = atoi (pRearEnd->value());
			newServer->setRearEnd (dwSetp);
		}
		auto pRegRoute = pS->first_attribute("regRoute");
		if (pRegRoute ) {
			auto dwSetp = atoi (pRegRoute->value());
			newServer->setRegRoute(dwSetp);
		}
		auto pAutoRun= pS->first_attribute("autoRun");
		if (pAutoRun) {
			auto autoRun = atoi (pAutoRun->value()) != 0;
			newServer->setAutoRun(autoRun);
			if (!autoRun) {
				auto pM = rApp.mainLoopServer();
				if (!pM) {
					rApp.setMainLoopServer (newServer->strTmpHandle ());
				}
			}
		}

		// auto& rEndPointS = tSingleton<toolServerEndPointInfoMgr>::single().endPointS ();
		auto& rInfo = newServer->serverInfo ();
		auto& rENS = rGrobal.endPointGlobalS ();
		for(auto pXmlListen = pS->first_node (); pXmlListen;
				pXmlListen = pXmlListen->next_sibling ()) {
			auto pNodeName = pXmlListen->name ();
			if (strcmp (pNodeName, "listen")) {
				continue;
			}
			auto pEName = pXmlListen->first_attribute("name");
			myAssert (pEName);
			if (!pEName) {
				rError ("listen endpoint hanve no name");
				nRet = 1;
				break;
			}
			auto it = rENS.find (pEName->value ());
			myAssert (rENS.end () != it);
			if ((rENS.end () == it)) {
				rError ("endpoint hanve error name: "<<pEName->value());
				nRet = 2;
				break;
			}
			const auto c_MaxListen = sizeof(rInfo.listenEndpoint) / 
				sizeof (rInfo.listenEndpoint[0]);
			myAssert (rInfo.listenerNum < c_MaxListen );
			auto& endPoint = rInfo.listenEndpoint[rInfo.listenerNum++];
			procEndPoint (pXmlListen, endPoint);
			endPoint.port = it->second;
			strNCpy(endPoint.endPointName, sizeof (endPoint.endPointName), pEName->value ());
			auto pXmlIp = pXmlListen->first_attribute ("ip");
			if (pXmlIp) {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						pXmlIp->value ());
			} else {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						"0.0.0.0");
			}
		}
		if (nRet) {
			break;
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
			auto pXmlTarget = pXmlCon->first_attribute ("targetEndPoint");
			if (pXmlTarget) {
				strNCpy (endPoint.targetEndPoint, sizeof (endPoint.targetEndPoint),
						pXmlTarget->value ());
				strNCpy(endPoint.endPointName, sizeof (endPoint.endPointName), pXmlTarget->value ());
			}
			auto pXmlIp = pXmlCon->first_attribute ("ip");
			if (pXmlIp) {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						pXmlIp->value ());
			} else {
				strNCpy (endPoint.ip, sizeof (endPoint.ip),
						"127.0.0.1");
			}
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
					auto pTmpHandle = newServer->strTmpHandle();
					myAssert (pTmpHandle);
					pMsg->setDefProServerTmpId (pTmpHandle);
				}
			}
		}
		if (nRet) {
			break;
		}
		ubyte  netType = rApp.netType ();
		// bool bIsRoot = newServer->isRoot ();
		if (appNetType_gate == netType ) {
			int rootRpcNum = 0;
			auto ppRpc = getRpptRpc (rootRpcNum);
			for (decltype (rootRpcNum) i = 0; i < rootRpcNum; i++) {
				procRpcNode node;
				node.retValue = "procPacketFunRetType_del";
				node.bAsk = true;
				node.rpcName = ppRpc[i];
				auto inRet = rProcS.insert (node);
				myAssert (inRet.second);
			}
		}
		
		if (appNetType_gate == netType || appNetType_server == netType) {
			procRpcNode node;
			node.retValue = "procPacketFunRetType_del"; /* 特殊情况单独处理 */
			node.bAsk = appNetType_gate == netType;
			node.rpcName = "regRoute";
			auto inRet = rProcS.insert (node);
			myAssert (inRet.second);
		}
		{
			procRpcNode node;
			node.retValue = "procPacketFunRetType_exitNow"; /* 特殊情况单独处理 */
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

