#include "xmlMsgFileLoad.h"
#include "xmlCommon.h"
#include "tSingleton.h"
#include "strFun.h"
#include <cstring>
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
#include "fromFileData/msgPmpFile.h"

xmlMsgFileLoad:: xmlMsgFileLoad ()
{
}

xmlMsgFileLoad:: ~xmlMsgFileLoad ()
{
}

int  xmlMsgFileLoad::xmlLoadFromStr (const char* szPmpName, char* szCon, msgPmpFile& rPmp)
{
    int   nRet = 0;
	do {
		rapidxml::xml_document<> doc;
		doc.parse<0>(szCon);

		rapidxml::xml_node<>* pStructS = doc.first_node ("struct");
		rapidxml::xml_node<>* pRpcS = doc.first_node ("rpc");
		auto pPowerCom = doc.first_node ("powerCom");
		bool powerCom = true;
		if (pPowerCom) {
			powerCom = atoi(pPowerCom->value());
		}
		auto nR = structLoad (pStructS, rPmp, powerCom);
		if (nR) {
			rError (" structLoad  error nR = "<<nR);
			nRet = 1;
			break;
		}
		nR = rpcLoad (pRpcS, rPmp, powerCom);
		if (nR) {
			rError (" rpcLoad error nR = "<<nR);
			nRet = 2;
			break;
		}
	} while (0);
	return nRet;
}

int xmlMsgFileLoad:: xmlLoad (const char* szPmpName, const char* szFile, msgPmpFile& rPmp)
{
    int   nRet = 0;
	do {
		rapidxml::file<> fdoc(szFile);
		nRet = xmlLoadFromStr (szPmpName,fdoc.data (), rPmp);
	} while (0);
	return nRet;
}

int   xmlMsgFileLoad:: structLoad (rapidxml::xml_node<char>* pStructS, msgPmpFile& rPmp, bool powerCom)
{
    int   nRet = 0;
    do {
		if (!pStructS) {
			break;
		}
		for(rapidxml::xml_node<char> * pC = pStructS->first_node();  pC; pC = pC->next_sibling()) {
			auto pName = pC->name ();
			auto nR = onceStructLoad (pC, rPmp, powerCom);
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

int   xmlMsgFileLoad:: structLoadBody (structFile& rSt, rapidxml::xml_node<char>* pStruct)
{
    int   nRet = 0;
    do {
		auto pName = pStruct->name ();
		auto pCom = pStruct->first_attribute("commit");
		if(pCom) {
			rSt.setCommit (pCom->value ());
		}

		auto pPowerCom= pStruct->first_attribute("powerCom");
		if (pPowerCom) {
			rSt.setPowerCom(atoi (pPowerCom->value()));
		}
		auto& rV = rSt.dataOrder ();
		auto& rDataS = rSt.dataS ();
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
			int nL = 1;
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
    } while (0);
    return nRet;
}

int   xmlMsgFileLoad:: onceStructLoad (rapidxml::xml_node<char>* pStruct, msgPmpFile& rPmp, bool powerCom)
{
    int   nRet = 0;
	do {
		// auto&  rMgr = tSingleton <structFileMgr>::single ().structS ();
		// auto&  rVec = tSingleton <structFileMgr>::single ().structOrder();
		auto&  rMgr = rPmp.structFileS().structS ();
		auto&  rVec = rPmp.structFileS().structOrder();
		auto pName = pStruct->name ();
		auto it = rMgr.find (pName);
		if (rMgr.end () != it) {
			rError (" two struct have the save name: "<<pName);
			nRet = 1;
			break;
		}

		auto pS = std::make_shared <structFile>();
		pS->setStructName (pName);
		pS->setPowerCom(powerCom);
		structLoadBody (*(pS.get ()), pStruct);
		rMgr [pName] = pS;
		rVec.push_back (pS);
    } while (0);
    return nRet;
}

int   xmlMsgFileLoad:: rpcLoad (rapidxml::xml_node<char>* pRpcS, msgPmpFile& rPmp, bool canProto)
{
    int   nRet = 0;
    do {
		if (!pRpcS) {
			break;
		}
		for(rapidxml::xml_node<char> * pG = pRpcS->first_node();  pG; pG = pG->next_sibling()) {
			auto nR = onceRpcGroupLoad (pG, rPmp, canProto);
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

int   xmlMsgFileLoad:: onceRpcGroupLoad (rapidxml::xml_node<char>* pGroup, msgPmpFile& rPmp,  bool powerCom)
{
    int   nRet = 0;
    do {
		if (!pGroup) {
			nRet = 1;
			break;
		}
		// auto&  rGroupMgr = tSingleton <msgGroupFileMgr>::single ();
		// auto&  rRpcMgr = tSingleton <rpcFileMgr>::single ();
		// auto&  rMsgMgr = tSingleton <msgFileMgr>::single ();

		auto&  rGroupMgr = rPmp.msgGroupFileS ();
		auto&  rRpcMgr = rPmp.rpcFileS();
		auto&  rMsgMgr = rPmp.msgFileS();

		auto&  rMsgS = rMsgMgr.msgS ();
		auto&  rRpcS = rRpcMgr.rpcS ();

		// auto&  rMgr = tSingleton <structFileMgr>::single ().structS ();
		// auto&  rVec = tSingleton <structFileMgr>::single ().structOrder();
		// auto&  rRpcOrder = rRpcMgr.rpcOrder ();
		auto&  rGroupS = rGroupMgr.msgGroupS ();
		auto pGName = pGroup->name ();
		auto pG = std::make_shared <msgGroupFile> ();
		pG->setMsgGroupName (pGName);
		auto& rXmlCommon = tSingleton<xmlCommon>::single ();

		auto pA = pGroup->first_attribute("order");
		if (pA) {
			pG->setHasOrder (true);
			pG->setGroupOrder (atoi (pA->value ()));
		}
		bool bExtPH = false;
		int nR = rXmlCommon.getBoolA (pGroup, "extPH", bExtPH);
		if (0 == nR) {
			pG->setExtPH(bExtPH);
		}
		int nInt = 0;
		nR = rXmlCommon.getIntA(pGroup, "addrType", nInt);
		if (0 == nR) {
			pG->setAddrType((packAddrType)nInt);
		}

		std::string strFull = pGName;
		strFull += "2FullMsg";
		pG->setFullChangName (strFull.c_str ());
		std::string strRpcSrcFileName = pGName;
		strRpcSrcFileName += "Rpc";
		pG->setRpcSrcFileName (strRpcSrcFileName.c_str ());
		std::string strGroupName = pGName;
		auto groupRet = rGroupS.insert (std::make_pair(strGroupName, pG));
		if (!groupRet.second) {
			rError ("group have the same name : "<<pGName);
			nRet = 7;
			break;
		}
		int szRootRpcNum = 0;
		const char** getRpptRpc (int &num);
		auto& rNameS = pG->rpcNameS ();
		for (auto pRpc = pGroup->first_node();  pRpc; pRpc = pRpc->next_sibling()) {
			auto pRpcName = pRpc->name ();
			auto pRpcFile = std::make_shared <rpcFile> ();
			pRpcFile->setRpcName (pRpcName);
			pRpcFile->setGroupName (pGName);
			
			std::string strRpcName = pRpcName;
			auto rpcRet = rRpcS.insert (std::make_pair(strRpcName, pRpcFile));
			if (!rpcRet.second) {
				rError ("rpc have the same name : "<<pRpcName);
				nRet = 6;
				break;
			}
			rNameS.push_back (pRpcName);
			std::string strIdSave = pGName;
			strIdSave += "MsgId_";
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
			pAskMsg->setPowerCom (powerCom);
			std::string strAskMsgName = strAskName;
			strAskMsgName += "Msg";
			pRpcFile->setAskMsgName (strAskName.c_str ());
			pAskMsg->setMsgName (strAskMsgName.c_str ());
			std::string strID = strIdSave;
			strID += strAskName;
			pAskMsg->setStrMsgId (strID.c_str());
			std::stringstream ssFullMsgId;
			ssFullMsgId<< strFull<<R"(()"<<strID<<")";
			pAskMsg->setMsgFullName (ssFullMsgId.str().c_str());
			bool bCExtPH = false;
			nR = rXmlCommon.getBoolA (pAsk, "extPH", bCExtPH);
			if (0 == nR) {
				pAskMsg->setExtPH(bCExtPH);
			} else {
				pAskMsg->setExtPH(pG->extPH());
			}
			bool neetSe = false;
			nR = rXmlCommon.getBoolA (pAsk, "neetSession", neetSe);
			if (0 == nR) {
				pAskMsg->setNeetSession(neetSe);
			}
			int nCInt = 0;
			nR = rXmlCommon.getIntA(pAsk, "addrType", nCInt);
			if (0 == nR) {
				pAskMsg->setAddrType((packAddrType)nCInt);
			} else {
				pAskMsg->setAddrType(pG->addrType ());
			}
			std::unique_ptr <char[]> pmAskName;
			strCpy (strAskName.c_str (), pmAskName);
			toWord (pmAskName.get());
			std::string strAskPack = "on";
			strAskPack += pmAskName.get();
			pAskMsg->setPackFunName (strAskPack.c_str ());

			std::string strMsgProc = "proc";
			auto pAskStructName = pAskMsg->structName ();
			strMsgProc += pmAskName.get();
			pAskMsg->setMsgFunName (strMsgProc.c_str ());

			structFile* pS = (structFile*)(pAskMsg.get ());
			auto nR = structLoadBody (*(pS), pAsk);
			if (nR) {
				nRet = 4;
				break;
			}
			auto askRet = rMsgS.insert (std::make_pair(strAskName, pAskMsg));
			if (!askRet.second) {
				rError (" msg have the same name: "<<strAskName);
				nRet = 5;
				break;
			}
			std::string strAskDec = "void ";
			strAskDec += strMsgProc;
			strAskDec += R"( ()";
			bool askHasData = pAskMsg->hasData ();
			if (askHasData) {
				strAskDec += "const ";
				strAskDec += pAskStructName;
				strAskDec += "& rAsk ";
			}	
			bool haveRetData = false;
			auto pRet = pRpc->first_node("ret");
			if (pRet) {
				std::string strRetName = pRpcName;
				strRetName += "Ret";
				auto pRetMsg = std::make_shared <msgFile> ();
				pRetMsg->setStructName (strRetName.c_str ());
				pRetMsg->setPowerCom (powerCom);
				pRpcFile->setRetMsgName (strRetName.c_str ());
				std::string strMsgName = strRetName;
				strMsgName += "Msg";
				pRetMsg->setMsgName (strMsgName.c_str ());
				std::string strID = strIdSave;
				strID += strRetName;
				pRetMsg->setStrMsgId (strID.c_str());

				std::stringstream ssFullMsgId;
				ssFullMsgId<< strFull<<R"(()"<<strID<<")";
				pRetMsg->setMsgFullName (ssFullMsgId.str().c_str());

				bool bCExtPH = false;
				int nR = rXmlCommon.getBoolA (pRet, "extPH", bCExtPH);
				if (0 == nR) {
					pRetMsg->setExtPH(bCExtPH);
				} else {
					pRetMsg->setExtPH(pG->extPH());
				}
				bool neetSe = false;
				nR = rXmlCommon.getBoolA (pRet, "neetSession", neetSe);
				if (0 == nR) {
					pRetMsg->setNeetSession(neetSe);
				}
				int nCInt = 0;
				nR = rXmlCommon.getIntA(pRet, "addrType", nCInt);
				if (0 == nR) {
					pRetMsg->setAddrType((packAddrType)nCInt);
				} else {
					pRetMsg->setAddrType(pG->addrType ());
				}
				structFile* pS = (structFile*)(pRetMsg.get ());
				nR = structLoadBody (*(pS), pRet);
				if (nR) {
					nRet = 8;
					break;
				}
			std::string strRetPack = "on";
			
			std::unique_ptr <char[]> pmRetName;
			strCpy (strRetName.c_str (), pmRetName);
			toWord (pmRetName.get());
			strRetPack += pmRetName.get();
			pRetMsg->setPackFunName (strRetPack.c_str ());
			std::string strRetMsgProc = "proc";
			strRetMsgProc += pmRetName.get ();
			pRetMsg->setMsgFunName (strRetMsgProc.c_str ());
			std::string strDec = "void ";
			strDec += strRetMsgProc;
			strDec += R"( ()";
			if (askHasData) {
				strDec += "const ";
				strDec += strAskName;
				strDec += "& rAsk";
			}
			auto retHasData = pRetMsg->hasData ();
			if (retHasData) {
				if (askHasData) {
					strAskDec += ", ";
					strDec += ", ";
				}
				strAskDec += strRetName;
				strAskDec += "& rRet";

				strDec += strRetName;
				strDec += "& rRet";
				/*
				if (isRootRpc ) {
					strAskDec += ", channelKey& rCh";
					strDec += ", channelKey& rCh";
				}
				*/
			}
			auto retNeetSe = pRetMsg->neetSession ();
			if (retNeetSe) {
				strDec += ", serverIdType srcSer, SessionIDType seId";
			}

			strDec += ")";
			pRetMsg->setMsgFunDec (strDec.c_str ());
			auto retRet = rMsgS.insert (std::make_pair (strRetName, pRetMsg));
				if (!retRet.second	) {
					rError (" msg have the same name : "<<strRetName.c_str ());
					nRet = 9;
					break;
				}
				// rMgr [strRetName] = pS;
				// rVec.push_back (pS);
			}
			auto askNeetSe = pAskMsg->neetSession ();
			if (askNeetSe) {
				strAskDec += ", serverIdType srcSer, SessionIDType seId";
			}
			strAskDec += ")";
			pAskMsg->setMsgFunDec (strAskDec.c_str ());
		}
		if (nRet) {
			break;
		}
    } while (0);
    return nRet;
}


