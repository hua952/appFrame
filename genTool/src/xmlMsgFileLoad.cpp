#include "xmlMsgFileLoad.h"
#include "xmlCommon.h"
#include "tSingleton.h"
#include "strFun.h"
#include "rLog.h"
#include "fromFileData/structFileMgr.h"
#include "fromFileData/structFile.h"
#include "fromFileData/dataFile.h"
#include "fromFileData/rpcFile.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/msgGroupFile.h"
#include "fromFileData/msgGroupFileMgr.h"

xmlMsgFileLoad:: xmlMsgFileLoad ()
{
}

xmlMsgFileLoad:: ~xmlMsgFileLoad ()
{
}

int   xmlMsgFileLoad:: xmlLoad (const char* szFile)
{
    int   nRet = 0;
	do {
		rapidxml::file<> fdoc(szFile);
		rapidxml::xml_document<> doc;
		doc.parse<0>(fdoc.data());

		rapidxml::xml_node<> *root = doc.first_node();
		rapidxml::xml_node<>* pStructS = root->first_node ("struct");
		rapidxml::xml_node<>* pRpcS = root->first_node ("rpc");

		auto nR = structLoad (pStructS);
		if (nR) {
			rError (" structLoad  error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = rpcLoad (pRpcS);
		if (nR) {
			rError (" rpcLoad error nR = "<<nR);
			nRet = 2;
			break;
		}
	} while (0);
	return nRet;
}

int   xmlMsgFileLoad:: structLoad (rapidxml::xml_node<char>* pStructS)
{
    int   nRet = 0;
    do {
		if (!pStructS) {
			break;
		}
		for(rapidxml::xml_node<char> * pC = pStructS->first_node();  pC; pC = pC->next_sibling()) {
			auto pName = pC->name ();
			auto nR = onceStructLoad (pC);
			if (nR) {
				nRet = 1;
				break;
			}
		}
		if (nRet) {
			break;
		}
    } while (0);
    return nRet;
}

int   xmlMsgFileLoad:: onceStructLoad (rapidxml::xml_node<char>* pStruct)
{
    int   nRet = 0;
	do {
		auto&  rMgr = tSingleton <structFileMgr>::single ().structS ();
		auto pName = pStruct->name ();
		auto it = rMgr.find (pName);
		if (rMgr.end () != it) {
			rError (" two struct have the save name: "<<pName);
			nRet = 1;
			break;
		}

		auto pS = std::make_shared <structFile>();
		pS->setStructName (pName);
		auto pCom = pStruct->first_attribute("commit");
		if(pCom) {
			pS->setCommit (pCom->value ());
		}
		auto& rV = pS->dataOrder ();
		auto& rDataS = pS->dataS ();
		auto& rCommon = tSingleton<xmlCommon>::single ();

		for(rapidxml::xml_node<char> * pC = pStruct->first_node();  pC; pC = pC->next_sibling()) {
			auto pDN = rCommon.getStringA (pC, "dataType");
			if(!pDN) {
				nRet = 2;
				break;
			}
			auto pN = pC->name ();
			auto it = rDataS.find (pN);
			if (rDataS.end () != it) {
				rError (" two data have the save name : "<<pN<<"struct name is : "<<pName);
				nRet = 3;
				break;
			}
			auto pD = std::make_shared <dataFile> ();
			pD->setDataName (pN);
			pD->setDataType (pDN);
			pD->setCommit (rCommon.getStringA (pC, "commit"));
			int nR = 0;
			int nL = 0;
			nR = rCommon.getIntA (pC, "length", nL);
			if(0 == nR) {
				pD->setDataLength (nL);
			}
			bool bZE = false;
			nR = rCommon.getBoolA (pC, "zeroEnd", bZE);
			if(0 == nR) {
				pD->setZeroEnd(bZE);
			}
			nR = rCommon.getBoolA (pC, "haveSize", bZE);
			if(0 == nR) {
				pD->setHaveSize(bZE);
			}
			nR = rCommon.getBoolA (pC, "wordSize", bZE);
			if(0 == nR) {
				pD->setWordSize(bZE);
			}
			rV.push_back (pD);
			rDataS [pN] = pD;
		}
		if (nRet) {
			break;
		}
		rMgr [pName] = pS;
    } while (0);
    return nRet;
}

int   xmlMsgFileLoad:: rpcLoad (rapidxml::xml_node<char>* pRpcS)
{
    int   nRet = 0;
    do {
		if (!pRpcS) {
			break;
		}
		for(rapidxml::xml_node<char> * pG = pRpcS->first_node();  pG; pG = pG->next_sibling()) {
			auto nR = onceRpcGroupLoad (pG);
			if (nR) {
				rError (" onceRpcGroupLoad ret error nR = "<<nR);
				nRet = 1;
				break;
			}
		}
		if (nRet) {
			break;
		}
    } while (0);
    return nRet;
}

int   xmlMsgFileLoad:: onceRpcGroupLoad (rapidxml::xml_node<char>* pGroup)
{
    int   nRet = 0;
    do {
		if (!pGroup) {
			nRet = 1;
			break;
		}
		auto&  rGroupMgr = tSingleton <msgGroupFileMgr>::single ();
		auto&  rRpcMgr = tSingleton <rpcFileMgr>::single ();
		auto&  rMsgMgr = tSingleton <msgFileMgr>::single ();
		auto&  rMsgS = rMsgMgr.msgS ();
		auto&  rRpcS = rRpcMgr.rpcS ();
		auto&  rGroupS = rGroupMgr.msgGroupS ();
		auto pGName = pGroup->name ();
		auto pG = std::make_shared <msgGroupFile> ();
		pG->setMsgGroupName (pGName);
		std::string strGroupName = pGName;
		auto groupRet = rGroupS.insert (std::make_pair(strGroupName, pG));
		if (!groupRet.second) {
			rError ("group have the same name : "<<pGName);
			nRet = 7;
			break;
		}
		auto& rNameS = pG->rpcNameS ();
		for (auto pRpc = pGroup->first_node();  pRpc; pRpc = pRpc->next_sibling()) {
			auto pRpcName = pRpc->name ();
			auto nR = rNameS.insert (pRpcName);
			if (!nR.second) {
				rError ("rpc have the save name : "<<pRpcName);
				nRet = 2;
				break;
			}
			auto pRpcFile = std::make_shared <rpcFile> ();
			pRpcFile->setRpcName (pRpcName);
			std::string strRpcName = pRpcName;
			auto rpcRet = rRpcS.insert (std::make_pair(strRpcName, pRpcFile));
			if (!rpcRet.second) {
				rError ("rpc have the same name : "<<pRpcName);
				nRet = 6;
				break;
			}
			auto pAsk = pRpc->first_node("ask");
			if (!pAsk) {
				rError ("rpc: "<<pRpcName<<" have no ask");
				nRet = 3;
				break;
			}
			auto pAskMsg = std::make_shared<msgFile> ();
			std::string strAskName = pRpcName;
			strAskName += "Ask";
			pAskMsg->setStructName (strAskName.c_str ());
			auto askRet = rMsgS.insert (std::make_pair(strAskName, pAskMsg));
			if (!askRet.second) {
				rError (" msg have the same name: "<<strAskName);
				nRet = 4;
				break;
			}
			auto pRet = pRpc->first_node("ret");
			if (pRet) {
				std::string strRetName = pRpcName;
				strRetName += "Ret";
				auto pRetMsg = std::make_shared <msgFile> ();
				pRetMsg->setStructName (strRetName.c_str ());
				auto retRet = rMsgS.insert (std::make_pair (strRetName, pRetMsg));
				if (!retRet.second	) {
					rError (" msg have the same name : "<<strRetName.c_str ());
					nRet = 5;
					break;
				}
			}
		}
		if (nRet) {
			break;
		}
    } while (0);
    return nRet;
}


