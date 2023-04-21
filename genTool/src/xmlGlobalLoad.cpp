#include "xmlGlobalLoad.h"
#include "strFun.h"
#include "tSingleton.h"
#include "rLog.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/moduleFileiMgr.h"
#include "xmlCommon.h"

xmlGlobalLoad:: xmlGlobalLoad ()
{
}

xmlGlobalLoad:: ~xmlGlobalLoad ()
{
}

int  xmlGlobalLoad::xmlLoad (const char* szFile)
{
	int nRet = 0;
	rInfo (" process xml : "<<szFile);
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
		} else if(0 == strcmp(pName, "msgFile")) {
			rMsgFileS.push_back (szPath);
		} else if(0 == strcmp(pName, "app")) {
			pAppS = pRpc;
		}
	}
	do {
		if (!pAppS) {
			rError ("can not find appS node");
			nRet = 1;
			break;
		}
		int nR = 0;
		nR = appSLoad (pAppS);
		if (nR) {
			rError ("appSLoad error nR = "<<nR);
			nRet = 2;
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

int   xmlGlobalLoad:: onceAppLoad (rapidxml::xml_node<char>* pApp, std::shared_ptr<appFile>& rApp)
{
    int   nRet = 0;
	do {
		rInfo ("proc app name = "<<pApp->name ());
		rapidxml::xml_node<char>* pModuleS = pApp->first_node("module");
		if (!pModuleS) {
			nRet = 1;
			rError ("app : "<<pApp->name ()<<" not find moduleS ");
			break;
		}

		auto& rMap = tSingleton<appFileMgr>::single ().appS ();
		auto pA = std::make_shared<appFile> ();
		auto pName = pApp->name ();
		pA->setAppName (pName);
		auto nR = moduleSLoad (pModuleS, *pA);
		if (nR) {
			rError ("moduleSLoad  error nR = "<<nR<<" app name: "<<pName);
			nRet = 2;
			break;
		}
		rApp = pA;
    } while (0);
    return nRet;
}

int   xmlGlobalLoad:: moduleSLoad (rapidxml::xml_node<char>* pModuleS, appFile& rApp)
{
	int   nRet = 0;
	do {
		auto& rSet = rApp.moduleFileNameS ();
		auto& rMap = tSingleton<moduleFileiMgr>::single ().moduleS ();
		moduleFileiMgr::moduleMap  tmap;
		for(rapidxml::xml_node<char> * pM = pModuleS->first_node();  pM; pM = pM->next_sibling()) {
			auto pName = pM->name ();
			std::shared_ptr<moduleFile> rM;
			auto nR = onceModuleLoad (pM, rM);
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

int   xmlGlobalLoad:: onceModuleLoad (rapidxml::xml_node<char>* pM, std::shared_ptr<moduleFile>& rM)
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
		int nR = serverSLoad (pServerS, *newModule);
		if (nR) {
			rError (" serverSLoad  return err nR = "<<nR<<" moduleName = "<<pM->name ());
			nRet = 2;
			break;
		}
		rM = std::move (newModule);
	} while (0);
	return nRet;
}

int   xmlGlobalLoad:: serverSLoad (rapidxml::xml_node<char>* pServerS, moduleFile& rM)
{
    int   nRet = 0;
	do {
		auto& rMap = rM.serverS ();
		moduleFile::serverMap& tmp = rMap;
		for(rapidxml::xml_node<char> * pS = pServerS->first_node();  pS; pS = pS->next_sibling()) {
			std::shared_ptr<serverFile> rS;
			auto nR = onceServerLoad(pS, rS);
			if (nR) {
				rError ("onceServerLoad return error nR = "<<nR<<" server name: "<<pS->name ());
				nRet = 1;
				break;
			}
			rS->setModuleName (rM.moduleName());
			auto pRet = tmp.insert (std::make_pair(pS->name (), rS));
			tmp [pS->name ()] = rS;
			
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
	} while (0);
    return nRet;
}

int   xmlGlobalLoad:: onceServerLoad (rapidxml::xml_node<char>* pS, std::shared_ptr<serverFile>& rS)
{
    int   nRet = 0;
    do {
		rInfo (" proc thServer : "<<pS->name ());
		auto newServer = std::make_shared <serverFile>();
		auto szServerName = pS->name ();
		newServer->setServerName (szServerName);
		auto pXmlRrocRpc = pS->first_node ("procRpc");
		if (!pXmlRrocRpc) {
			nRet = 1;
			break;
		}
		auto& rProcS = newServer->procMsgS ();
		auto& rXmlCommon = tSingleton<xmlCommon>::single ();
		for (auto pXmlRpc = pXmlRrocRpc->first_node (); pXmlRpc; pXmlRpc = pXmlRpc->next_sibling ()) {
			procRpcNode node;
			node.rpcName = pXmlRpc->name ();
			auto pAskType = pXmlRpc->first_attribute("askType");
			if (!pAskType) {
				rError ("rpc have no askType rpcName = "<<node.rpcName.c_str ());
				nRet = 2;
				break;
			}
			auto szAskType  = pAskType->value ();
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
		}
		if (nRet) {
			break;
		}
		rS = newServer;
    } while (0);
    return nRet;
}

