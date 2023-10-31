#include "msgGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
#include <type_traits>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include "strFun.h"
#include "comFun.h"
#include "fromFileData/msgGroupFileMgr.h"
#include "fromFileData/msgGroupFile.h"
#include "fromFileData/msgFileMgr.h"
#include "fromFileData/msgFile.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/structFileMgr.h"
#include "fromFileData/structFile.h"
#include "fromFileData/appFileMgr.h"
#include "fromFileData/appFile.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/moduleFileMgr.h"
#include "fromFileData/serverFile.h"
#include "fromFileData/dataFile.h"
#include "fromFileData/msgPmpFile.h"
#include <sstream>
#include "myAssert.h"
#include "rLog.h"
#include "mainLoop.h"
#include "configMgr.h"

msgGen::msgGen (msgPmpFile& rPmp):m_rPmp(rPmp)
{
}

msgGen:: ~msgGen ()
{
}

int  msgGen:: startGen ()
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		auto pPmpName = rPmp.pmpName ();
		strFilename += "/";
		strFilename += pPmpName;
		strFilename += "/src";
		myMkdir (strFilename.c_str ());
		auto strPro = strFilename;
		strPro += "/protobuf/";
		myMkdir (strPro.c_str());
		setProtoDir (strPro.c_str());
		auto& proOs = protoOs ();
		
		std::string strIDFull = pPmpName;
		std::string strFile = srcDir ();
		strFile += "/serialize/";
		myMkdir (strFile.c_str());
		strFile += strIDFull;
		auto strSerFile = strFile;
		strSerFile += "Ser_com.cpp";
		setComSerFileName (strSerFile.c_str());
		strSerFile = strFile;
		strSerFile += "Ser_Proto.cpp";
		setProtoSerFileName (strSerFile.c_str());

		proOs<<R"(syntax = "proto3";
)";
		int nR = 0;
		nR = mkDir ();
		if (nR) {
			nRet = 1;
			break;
		}
		nR = CMakeListGen ();
		if (nR) {
			nRet = 2;
			break;
		}
		nR = msgGroupIdGen ();
		if (nR) {
			nRet = 3;
			break;
		}
		nR = msgStructGen ();
		if (nR) {
			nRet = 4;
			break;
		}
		nR = rpcHGen ();
		if (nR) {
			nRet = 5;
			break;
		}
		nR = rpcCppGen ();
		if (nR) {
			nRet = 6;
			break;
		}
		
		nR = rpcInfoHGen();
		if (nR) {
			nRet = 8;
			break;
		}
		strPro += pPmpName;
		strPro += ".proto"; 
		std::ofstream protoOsF (strPro.c_str());
		if (!protoOsF) {
			nRet = 9;
			break;
		}
		protoOsF<<proOs.str();
		protoOsF.close ();
		std::stringstream protocOs;
		protocOs<<"protoc --cpp_out="<<protoDir() <<" --proto_path="<<protoDir ()<<" "<<strPro;
		system (protocOs.str().c_str());
    } while (0);
    return nRet;
}

int  msgGen:: rpcInfoCppGen ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/rpcInfo.cpp";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#include "msgGroupId.h"
#include "msg.h"
#include <fstream>
#include <map>
#include "myAssert.h"
#include "loopHandleS.h"
)";
		auto& rPmp = m_rPmp;
		auto& rMap = rPmp.msgGroupFileS ().msgGroupS ();
		for (auto it = rMap.begin ();rMap.end () != it; ++it) {
			auto& strName = it->first;
			os<<R"(#include ")"<<strName<<R"(MsgId.h")"<<std::endl;
			os<<R"(#include ")"<<strName<<R"(Rpc.h")"<<std::endl;
		}
		os<<R"(

void dumpStructS ()
{
	std::ofstream os("dumpStructS.txt");
)";

	auto& rSS = dumpOs ();
	os<<rSS.str()<<R"(
}

int  checkStructS (const char* szFile)
{
	int nRet = 0;
	do {
		uword ut = 0x201;
		auto pB = (ubyte*)&ut;
		if (1 != pB[0] || 2 != pB[1]) {
			nRet = 1;
			break;
		}
		std::ifstream ifs(szFile);
		if (!ifs) {
			nRet = 2;
			break;
		}
		std::map<std::string, uqword> tempMap;
		std::string strLine;
		while (std::getline (ifs, strLine)) {
			std::stringstream ts (strLine);
			std::string strKey;
			uqword uqwValue;
			ts>>strKey>>uqwValue;
			auto inRet = tempMap.insert (std::make_pair(strKey, uqwValue));
			myAssert (inRet.second);
		}
		std::map<std::string, uqword>::iterator it;

		)"<<m_checkOs.str()<<R"(
	} while (0);
	return nRet;
}

void regRpcS (const ForMsgModuleFunS* pForLogic)
{
	auto regRpc = pForLogic->fnRegRpc;
)";
	auto& rMsgMgr = rPmp.msgFileS ();
	
	for (auto ite = rMap.begin ();rMap.end () != ite; ++ite) {
		auto& rG = *(ite->second.get());
		auto& rRpcS = rG.rpcNameS ();
	for (auto it = rRpcS.begin (); rRpcS.end () != it; ++it) {
		std::string strAsk = *it;
		strAsk += "Ask";
		auto pAsk = rMsgMgr.findMsg (strAsk.c_str ());
		myAssert (pAsk);
		auto askId = pAsk->strMsgId ();
		std::string retId = "c_null_msgID";
		std::string trySId = *it;
		trySId += "Ret";
		auto pAskDefPro = pAsk->defProServerId ();
		const char* pRetDefPro = "c_emptyLoopHandle";
		auto pRet = rMsgMgr.findMsg (trySId.c_str ());

		auto pFull = rG.fullChangName ();
		if (pRet) {
			pRetDefPro = pRet->defProServerId ();
			retId = pFull;
			retId += R"(()";
			retId += pRet->strMsgId ();
			retId += ")";
		} 
		os<<R"(    regRpc ()"<<pFull<<R"(()"<<askId<<R"(), )"<<
			retId<<", "<<pAskDefPro<<", "<<pRetDefPro<<");"<<std::endl;
	}
	} // for (G
	os<<R"(}
)";
    } while (0);
    return nRet;
}

std::stringstream&  msgGen:: dumpOs ()
{
    return m_dumpOs;
}

int  msgGen:: rpcInfoHGen ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/rpcInfo.h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#ifndef rpcInfo_h__
#define rpcInfo_h__
#include "msg.h"

void dumpStructS ();
int  checkStructS (const char* szFile);
void regRpcS (const ForMsgModuleFunS* pForLogic);
#endif)";
    } while (0);
    return nRet;
}

int   msgGen:: genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool bRet, std::string& strOut, std::string& strToPack)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		std::stringstream ss;
		std::stringstream ssP;
		auto strN = rMsg.structName ();
		auto msgName = rMsg.msgName ();
		std::string strClass = msgName;
		auto strFunWon = strClass;
		strFunWon += ":: ";
		auto fullChangName = rG.fullChangName ();
		auto strMsgId = rMsg.strMsgId ();
		std::string pSize = "0";
		if (rMsg.hasData ()) {
			pSize = "sizeof (";
			pSize += rMsg.structName ();
			pSize += ")";
		}
		auto extPH = rMsg.extPH ();
		std::string strAlloc = "allocPacket";
		if (extPH) {
			strAlloc = "allocPacketExt";
		}
		ss<<strFunWon<<strClass<<R"( ()
{
	m_pPacket = (packetHead*))"<<strAlloc<<R"(()"<<pSize;
		if (extPH) {
			ss<<", 1";
		}
		auto addrType = rMsg.addrType ();
		auto pAddr = "NSetAddSer(pN);";
		if (addrType_ch == addrType) {
			pAddr = "NSetAddCh(pN);";
		}
		ss<<R"();
	netPacketHead* pN = P2NHead(m_pPacket);
	)"<<pAddr<<R"(
	pN->uwMsgID = )"<<fullChangName<<R"(()"
	<<strMsgId<<R"();
	)";
	if (bRet) {
		ss<<R"(NSetRet(pN);
	)";
	}

	auto &rDv = rMsg.dataOrder ();

	if (!rDv.empty ()) {
		auto& rData = *(rDv.rbegin ()->get ());
		auto rDN = rData.dataName ();
		auto len = rData.dataLength ();
		auto zeroEnd = rData.zeroEnd ();
		auto haveSize = rData.haveSize ();
		auto bt = rConfig.structBadyType ();

		if (structBadyTime_com != bt) {
			auto powerCom = rMsg.powerCom ();
			if (powerCom) {
				bt = structBadyTime_com;
			}
		}

		if (structBadyTime_com == bt) {
			if (len) {
				if (haveSize) {
					ssP<<R"(packetHead* )"<<strFunWon<<R"(toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
    )";

				ssP<<strN<<R"(* p = (()"<<strN<<R"++(*)(N2User(pN)));
	)++";
				ssP<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	bool bWZ = false;
	)";
				if (zeroEnd	) {
					ssP<<R"(if(p->m_)"<<rDN<<R"(Num) {
	)"
					<<R"(bWZ = )"<<R"(p->m_)"<<rDN<<R"([)"
					<<R"(p->m_)"<<rDN<<R"(Num - 1] != 0;
	} else {
		bWZ = true;
	}
	if(bWZ) {
		p->m_)"<<rDN<<R"(Num++;
			}
			)";
				}
				ssP<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	pN->udwLength = sizeof()"<<strN<<R"() - sizeof(p->m_)"
				<<rDN<<R"([0]) * ()"<<len<<R"( - p->m_)"<<rDN<<R"(Num);
	)";
				if (zeroEnd) {
					ssP<<R"(if(bWZ) { 
			p->m_)"<<rDN<<R"([p->m_)"<<rDN<<R"(Num - 1] = 0;
				}
				)";
				}
				ssP<<R"(return m_pPacket;
}
)";
				}
} // if (len > 1)
		strToPack += ssP.str();
		} else if (structBadyTime_proto == bt) {
			std::stringstream  fromOs;
			std::stringstream  toOs;
		fromOs<<R"(bool )"<<strFunWon<<R"(fromPack(packetHead* p)
{
	bool nRet = true;
	do {
		myAssert (!m_pPacket);
		auto pN = P2NHead (p);

	)";
	auto fromFuName = rMsg.fromPbFuName ();
	fromOs<<msgName<<R"(	newC;
		)"<<msgName<<R"(Proto  msgPb;
		auto bRet = msgPb.ParseFromArray (N2User(pN), pN->udwLength);
		myAssert (bRet);
		auto pU = newC.pack ();
		m_pPacket = newC.pop ();
		auto pNN = P2NHead(m_pPacket);
		auto newLen = pNN->udwLength;
		*pNN = *pN;
		pNN->udwLength = newLen;
		)"<<fromFuName<<R"((*pU, msgPb);
		releasePack(p);
	while (0);
	return nRet;
)";

	toOs<<R"(packetHead* )"<<strFunWon<<R"(toPack()
{
	packetHead* nRet = nullptr;
	do {
		
	)";
	auto toPbFu = rMsg.toPbFuName ();
	toOs<<msgName<<R"(Proto  msgPb;
		myAssert(m_pPacket);
		auto pU = pack();
		)";
	toOs<<toPbFu<<R"((*pU, msgPb);
		
		auto newLen = msgPb.ByteSize ();
		auto pNewP = (packetHead*)allocPacket(newLen);
		auto pNU = ()";toOs<<strN<<R"(*)(N2User(pNN));
		auto pNN = P2NHead(pNewP);
		auto pN = P2NHead(m_pPacket);
		*pNN = *pN;
		pNN->udwLength = newLen;
		auto pD = m_pPacket;
		m_pPacket = pNewP;
)";
	
	fromOs<<R"(
	} while (0);
	return nRet;
}
	)";
	toOs<<R"(
	} while (0);
	return nRet;
}
	)";

		strToPack += fromOs.str();
		strToPack += toOs.str();
		} else {
			myAssert (0);
		}
		if (len) {
			auto haveSize = rData.haveSize ();
			if (haveSize) {
				ss<<R"(auto p = (()"<<strN<<R"(*)(N2User(pN)));
	p->m_)"<<rDN<<R"(Num = 0;
	)";
			}

			auto zeroEnd = rData.zeroEnd ();
			if (zeroEnd) {

				ss<<strN<<R"(* p2 = (()"<<strN<<R"++(*)(N2User(pN)));
	)++";
				ss<<R"(p2->m_)"<<rDN<<R"([0] = 0;
	)";
			}
		}
	}
	ss<<R"(
}
)"<<strFunWon<<strClass<<R"( (packetHead* p):CMsgBase(p)
{
}
)";

	if (!rDv.empty ()) {
		ss<<strN<<R"(* )"<<strFunWon<<R"(pack()
{
    return (()"<<strN<<R"(*)(P2User(m_pPacket)));
}
)";

}
ss<<std::endl;
	strOut = ss.str ();
    } while (0);
    return nRet;
}

int   msgGen:: genOnceStruct (structFile& rS, std::string& strOut, std::stringstream& protoStr, std::string& strSerializeOut)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		auto& rDv = rS.dataOrder ();
		if (rDv.empty ()) {
			break;
		}
		std::stringstream os;
		auto sName = rS.structName ();
		os<<R"(
struct )"<<sName<<R"(
{
)";

		protoStr<<R"(message )"<<sName<<R"(Proto
{
)";
		std::stringstream osFrom;
		std::stringstream osTo;
		auto bt = rConfig.structBadyType ();
		if (structBadyTime_com != bt) {
			auto powerCom = rS.powerCom ();
			if (powerCom) {
				bt = structBadyTime_com;
			}
		}

		if (structBadyTime_proto == bt) {
			auto pFromFull = rS.fromPbFuFullName ();
			osFrom<<pFromFull<<std::endl<<"{"<<std::endl<<"	";
			auto pToFull = rS.toPbFuFullName ();
			osTo<<pToFull<<std::endl<<"{"<<std::endl<<"	";
		}
		
		int nIndex = 1;
		for (auto iter = rDv.begin(); rDv.end() != iter; ++iter) {
			auto& rData = *(iter->get());
			std::string strD;
			genOnceData (rS, rData, strD, protoStr, nIndex++, osFrom, osTo);
			os<<strD;
		}
		os<<R"(};
)";

protoStr<<R"(}

)";

	if (structBadyTime_proto == bt) {
		osFrom<<"}"<<std::endl<<std::endl;
		osTo<<"}"<<std::endl<<std::endl;
		strSerializeOut += osFrom.str();
		strSerializeOut += osTo.str();
	}
		strOut = os.str();
    } while (0);
    return nRet;
}

void  msgGen:: procDataCheck (const char* structName, const char* valueName, bool bArry)
{
    do {
		std::string dDataStruct = structName;
		dDataStruct += ".";
		dDataStruct += valueName;
		
		std::stringstream ss;
		ss<<R"((uqword))";
		if (!bArry) {
			ss<<"&";
		}
		ss<<R"(((()"<<structName<<R"(*)0)->)"<<valueName<<R"())";
		m_dumpOs<<R"(	os<<")"<<dDataStruct<<R"(    "<<)"<<ss.str()<<R"(<<std::endl;)"<<std::endl;
		m_checkOs<<R"(	it = tempMap.find (")"<<dDataStruct<<R"(");
		if (it == tempMap.end ()) {
				nRet = 3;
				break;
			}
			if (it->second != )"<<ss.str()<<R"() {
				nRet = 4;
				break;
			}
			)";
    } while (0);
}

int   msgGen:: genOnceData (structFile& rS, dataFile& rData, std::string& strOut, std::stringstream& protoStr, int nIndex, std::stringstream& fromOs, std::stringstream& toOs)
{
    int   nRet = 0;
    do {

		auto& rPTS = tSingleton <globalFile>::single ().protoDataTypeS ();
		auto pStructName = rS.structName ();
		auto pDN = rData.dataName ();
		auto strDN = rData.memberName ();
		std::stringstream ss;
		std::string sizeName = "m_";
		sizeName += pDN;
		sizeName += "Num";
		auto& numName= sizeName;
		bool  haveSize  = rData.haveSize ();
		if (haveSize) {
			std::string strST = "udword";
			bool wordSize = rData.wordSize ();
			if (wordSize) {
				strST = "uword";
			}
			
			ss<<"    " << strST<<"    "<<sizeName<<";"<<std::endl;
			
			procDataCheck (pStructName, sizeName.c_str(), false);
		}
		std::string dDataName = "m_";
		dDataName += pDN;
		
		auto pDT = rData.dataType ();
		ss <<"    "<<pDT<<"    m_"<<pDN;

		auto& rStMap = tSingleton <structFileMgr>::single ().structS ();
		auto len = rData.dataLength ();
		if (len) {
			ss<<" ["<<len<<"]";
			auto  bString = true;
			procDataCheck (pStructName, dDataName.c_str(), true);
			if (0 == strcmp (pDT, "char")) {
				protoStr<<"	string "<<pDN<<" = "<<nIndex<<";"<<std::endl;
			} else {
				if (0 == strcmp (pDT, "ubyte")) {
					protoStr<<"	bytes "<<pDN<<" = "<<nIndex<<";"<<std::endl;
				} else {
					std::string strDT = pDT;
					auto it = rPTS.find (pDT);
					if (it == rPTS.end ()) {
						strDT += "Proto";
					} else {
						strDT = it->second;
					}
					protoStr<<"	repeated "<<strDT<<" "<<pDN<<" = "<<nIndex<<";"<<std::endl;
					bString = false;
				}
			}
			if (bString) {
				if (haveSize) {
					fromOs<<"	pU->"<<numName<<" = (decltype(pU->"<<numName<<"))(rPb."<<pDN<<"().size())"<<std::endl;
				}
				auto zeroEnd = rData.zeroEnd ();
				if (zeroEnd) {
					fromOs<<"	strNCpy (pU->"<<strDN<<", sizeof (pU->"<<strDN<<"), rPb."<<pDN<<"().c_str())"<<std::endl;
					toOs<<"	msgPb.set_"<<pDN<<" ((char*)(pU->"<<strDN<<"));"<<std::endl;
				} else {
					fromOs<<"	memcpy (pU->"<<strDN<<",  msgPb."<<pDN<<".data(), rPb."<<pDN<<"().size());"<<std::endl;
					toOs<<"	msgPb.set_"<<pDN<<" ((char*)(pU->"<<strDN<<"), sizeof(pU->"<<strDN<<"));"<<std::endl;
				}
			} else {
				if (haveSize) {
					fromOs<<"	pU->"<<numName<<" = (decltype(pU->"<<numName<<"))(rPb."<<pDN<<"_size())"<<std::endl;
				}

				std::string strDT = pDT;
				auto it = rPTS.find (pDT);
				if (it == rPTS.end ()) {
					strDT += "Proto";
					auto ite = rStMap.find (pDT);
					myAssert (ite != rStMap.end ());
					auto subForFuN = ite->second->fromPbFuName ();
					auto subToFuN = ite->second->toPbFuName ();

					fromOs<<R"(
		auto    nSize = rPb.)"<<pDN<<R"((_size ());
		myAssert (nSize <= )"<<len<<R"();
		for (int i = 0; i < nSize; i++) {
			)"<<subForFuN<<"(pU->"<<strDN<<"[i], rPb."<<pDN<<R"((i)));
		}
		)";
					toOs<<R"(
		for (int i = 0; i < )"<<len<<R"(; i++) {
			)"<<subToFuN<<"(pU->"<<strDN<<"[i], *rPb.add_"<<pDN<<R"(());
		}
		)";
				} else {
					strDT = it->second;
					fromOs<<R"(
		auto    nSize = rPb.)"<<pDN<<R"((_size ());
		myAssert (nSize <= )"<<len<<R"();
		for (int i = 0; i < nSize; i++) {
			pU->)"<<strDN<<"[i] = rPb."<<pDN<<R"((i);
			)
		}
		)";
					toOs<<R"(
		for (int i = 0; i < )"<<len<<R"(; i++) {
			rPb.add_)"<<pDN<<R"((pU->)"<<strDN<<R"([i]);
		}
		)";

				} //else
			} //else
		} else {  // !if(len)
			procDataCheck (pStructName, dDataName.c_str(), false);
			std::string strDT = pDT;
			auto it = rPTS.find (pDT);
			if (rPTS.end () == it) {
				strDT += "Proto";
					auto subForFuN = rS.fromPbFuName ();
					auto subToFuN = rS.toPbFuName ();
					fromOs<<"	"<<subForFuN<<"(pU->"<<strDN<<", rPb."<<pDN<<"());"<<std::endl;
					toOs<<"	"<<subToFuN<<"(pU->"<<strDN<<", *rPb.mutable_"<<pDN<<"());"<<std::endl;
			} else {
				strDT = it->second;
				fromOs<<"	pU->"<<strDN<<" = rPb."<<pDN<<"();"<<std::endl;
				toOs<<"	rPb.set_"<<pDN<<" (pU->"<<strDN<<");"<<std::endl;
			}
			protoStr<<"	"<<strDT<<" "<<pDN<<" = "<<nIndex<<";"<<std::endl;
		}
		ss<<";";
		auto pCom = rData.commit ();
		if (pCom) {
			ss<<R"(/* )"<<pCom<<" */";
		}
		ss<<std::endl;
		strOut = ss.str();
    } while (0);
    return nRet;
}

std::stringstream&  msgGen:: checkOs ()
{
    return m_checkOs;
}

int   msgGen:: genOnceMsgClassH (msgFile& rMsg, std::string& strOut)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		std::stringstream ss;
		structFile* pSA = (structFile *)&rMsg;
		std::string strOutS;
		std::string&  strSerializeOut = this->strSerializeOut ();
		auto nR = genOnceStruct (*pSA, strOutS, m_protoOs, strSerializeOut);
		myAssert (0 == nR);
		// auto& rPbc = pbcOs ();
		// rPbc<<strPbChangeOut;
		if (!strOutS.empty ()) {
			ss<<strOutS;
		}
		auto strN = rMsg.structName ();
		ss<<R"(class )"<<strN<<R"(Msg :public CMsgBase
{
public:
	)"<<strN<<R"(Msg ();
	)"<<strN<<R"(Msg (packetHead* p);
	)";
		if (!strOutS.empty ()) {
			
			ss<<strN<<R"(* pack ();
    )";
			auto &rDv = rMsg.dataOrder ();
			if (!rDv.empty ()) {
				auto enStructBadyType = rConfig.structBadyType ();
				bool stPowerCom = rMsg.powerCom ();
				bool bWCom = false;
				if (structBadyTime_com == enStructBadyType) {
					bWCom = true;
				} if (structBadyTime_proto == enStructBadyType) {
					if (stPowerCom) {
						bWCom = true;
					} else {
						ss<<R"(packetHead*   toPack() override;
						bool fromPack(packetHead* p) override;
	)";

					}
				}
				if (bWCom) {
					auto& rData = *(rDv.rbegin ()->get ());
					auto len = rData.dataLength ();
					if (len) {
						auto haveSize = rData.haveSize ();
						if (haveSize) {
							ss<<R"(packetHead*   toPack() override;)";
						}
					}
				}
			}
		}
		ss<<R"(
};
)";
	strOut = ss.str ();
    } while (0);
    return nRet;
}

int   msgGen:: genOnceRpcH (msgGroupFile& rG)
{
    int   nRet = 0;
    do {
		auto msgGroupName = rG.msgGroupName ();
		std::string strIDFull = msgGroupName;
		strIDFull += "Rpc";
		std::string strFile = srcDir ();
		strFile += "/";
		strFile += strIDFull;
		strFile += ".h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#ifndef _)"<<strIDFull<<R"(h__
#define _)"<<strIDFull<<R"(h__
#include "msgStruct.h"
)";
	auto& rPmp = m_rPmp;
	auto& rNameS =  rG.rpcNameS ();
	auto& rMsgMgr = rPmp.msgFileS ();
	for (auto it = rNameS.begin (); rNameS.end () != it; ++it) {
		std::string strN = *it;
		strN += "Ask";
		auto pAsk = rMsgMgr.findMsg (strN.c_str ());
		myAssert (pAsk);
		std::string strOut;
		auto nR = genOnceMsgClassH (*pAsk, strOut);	
		myAssert (0 == nR);
		os<<strOut;
		strN = *it;
		strN += "Ret";
		auto pRet = rMsgMgr.findMsg (strN.c_str ());
		if (pRet) {
			std::string strOut;
			auto nR = genOnceMsgClassH (*pRet, strOut);	
			myAssert (0 == nR);
			os<<strOut;
		}
	}
os<<R"(#endif
)";

    } while (0);
    return nRet;
}

const char*  msgGen:: protoSerFileName ()
{
    return m_protoSerFileName.get ();
}

void  msgGen:: setProtoSerFileName (const char* v)
{
    strCpy (v, m_protoSerFileName);
}

const char*  msgGen:: comSerFileName ()
{
    return m_comSerFileName.get ();
}

void  msgGen:: setComSerFileName (const char* v)
{
    strCpy (v, m_comSerFileName);
}

int   msgGen:: genOnceRpcCpp (msgGroupFile& rG)
{
    int   nRet = 0;
    do {
		auto msgGroupName = rG.msgGroupName ();
		std::string strIDFull = msgGroupName;
		strIDFull += "Rpc";
		std::string strFile = srcDir ();
		strFile += "/";
		strFile += strIDFull;
		strFile += ".cpp";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}

		auto& rConfig = tSingleton<configMgr>::single ();

	auto enStructBadyType = rConfig.structBadyType ();
	bool bWProto = (enStructBadyType == structBadyTime_proto);
		os<<R"(#include "myAssert.h"
#include "msgGroupId.h"
)";

	
	os<<R"(
#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
)";

	auto& rPmp = m_rPmp;
	auto& rNameS =  rG.rpcNameS ();
	auto& rMsgMgr = rPmp.msgFileS ();
	auto& rSerOut = strSerializeOut ();
	for (auto it = rNameS.begin (); rNameS.end () != it; ++it) {
		std::string strN = *it;
		strN += "Ask";
		auto pAsk = rMsgMgr.findMsg (strN.c_str ());
		myAssert (pAsk);
		std::string strOut;
		auto nR = genOnceMsgClassCpp (rG, *pAsk, false,strOut, rSerOut);	
		myAssert (0 == nR);
		os<<strOut;
		strN = *it;
		strN += "Ret";
		auto pRet = rMsgMgr.findMsg (strN.c_str ());
		if (pRet) {
			std::string strOut;
			auto nR = genOnceMsgClassCpp (rG, *pRet, true, strOut, rSerOut);	
			myAssert (0 == nR);
			os<<strOut;
		}
	}

	if (enStructBadyType == structBadyTime_com) {
		auto strSerFile = comSerFileName ();
		std::ofstream ofSer (strSerFile);
		myAssert (ofSer);
		ofSer<<R"(
#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
)";
		ofSer<<rSerOut;
	} else if (enStructBadyType == structBadyTime_proto) {
		auto strSerFile = protoSerFileName ();
		std::ofstream ofSer (strSerFile);
		myAssert (ofSer);
		auto& rPmp = m_rPmp;
		auto pPName = rPmp.pmpName ();
		ofSer<<R"(#include ")"<<pPName<<R"(.pb.h"

#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
)";
		ofSer<<rSerOut;
	} else {
		myAssert (0);
	}
    } while (0);
    return nRet;
}

int  msgGen:: rpcHGen ()
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGroup = rPmp.msgGroupFileS ().msgGroupS ();
		for (auto it = rGroup.begin (); rGroup.end () != it; ++it) {
			auto& rG = *(it->second.get());
			auto nR = genOnceRpcH (rG);
			if (nR) {
				nRet = 1;
				break;
			}
		}
    } while (0);
    return nRet;
}

int  msgGen:: rpcCppGen ()
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGroup = rPmp.msgGroupFileS ().msgGroupS ();;
		for (auto it = rGroup.begin (); rGroup.end () != it; ++it) {
			auto& rG = *(it->second.get());
			auto nR = genOnceRpcCpp (rG);
			if (nR) {
				nRet = 1;
				break;
			}
		}
    } while (0);
    return nRet;
}

int  msgGen:: msgStructGen ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/msgStruct.h";
		std::ofstream os(strFile);
		os<<R"(#ifndef _msgStruct_h__
#define _msgStruct_h__
#include "msg.h"
)";
		auto& rPmp = m_rPmp;
		auto& rSMgr = rPmp.structFileS ();
		auto& rSS = rSMgr.structOrder ();
		for (auto it = rSS.begin (); rSS.end () != it; ++it)
		{
			auto& rS = *(it->get ());
			std::string strS;
			std::string protoStr;
			std::string&  strSerializeOut = this->strSerializeOut ();
			genOnceStruct (rS, strS, m_protoOs, strSerializeOut);
			// auto& rPbc = pbcOs ();
			// rPbc<<strPbChangeOut;
			os << strS;
		}
		os<<std::endl<<"#endif"<<std::endl;
    } while (0);
    return nRet;
}

int  msgGen:: CMakeListGen ()
{
    int  nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto& rPmp = m_rPmp;
		auto& rConfig = tSingleton <configMgr>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		auto pPmpName = rPmp.pmpName ();
		strFilename += "/";
		strFilename += pPmpName;
		strFilename += "/CMakeLists.txt";
		std::ofstream os(strFilename);
		auto& rTool = rGlobal;
		// auto outPutPath = rTool.outPutPath ();
		auto depIncludeHome = rTool.depIncludeHome ();
		auto depLibHome = rTool.depLibHome ();
		// auto frameHome = rTool.frameHome ();
		auto frameLibPath = rTool.frameLibPath ();
		std::string framePath = rTool.frameInstallPath ();
		os<<"SET(prjName defMsg)"<<std::endl
			<<"SET(depIncludeHome "<<depIncludeHome<<")"<<std::endl
			<<"SET(depLibHome "<<depLibHome<<")"<<std::endl
			// <<"SET(frameHome "<<frameHome<<")"<<std::endl
			<<"SET(frameLibPath "<<frameLibPath<<")"<<std::endl
			// <<"SET(outPutPath "<<outPutPath<<")"<<std::endl
			<<R"(
set(srcS)
set(genSrcS)
set(serFile)
)";

		auto bt = rConfig.structBadyType ();
		if (bt == structBadyTime_com) {
			auto pFile = comSerFileName ();
			os<<R"(list(APPEND serFile )"<<pFile<<R"()
			)";
		} else if (bt == structBadyTime_proto) {
			auto pFile = protoSerFileName ();
			os<<R"(list(APPEND serFile )"<<pFile<<R"( src/protobuf/*.cc)
			)";
		} else {
			myAssert (0);
		}
os<<R"(
file(GLOB srcS src/*.cpp)
set(defS)
set(srcOS)
if (UNIX)
    MESSAGE(STATUS "unix")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/W1)
	file(GLOB defS src/gen/*.def)
	file(GLOB genSrcS src/gen/*.cpp)
	file(GLOB srcOS src/win/*.cpp)
	include_directories(
		${depIncludeHome}
		)
	link_directories(
		${depLibHome}
		)
endif ()
add_library(${prjName} ${genSrcS} ${srcS} ${srcOS} ${defS} ${serFile})
target_include_directories(${prjName} PUBLIC 
							src
							)";
							os<<framePath<<"include/appFrame"<<std::endl;

	bool bWProto = ((structBadyTime_proto == bt));
	if (bWProto) {
		os<<R"(src/protobuf
							)";
	}
                           os<<R"(
						   )
target_link_libraries(${prjName} PUBLIC  common logicCommon)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
)";

    } while (0);
    return nRet;
}

int  msgGen:: msgGroupIdGen ()
{
    int  nRet = 0;
	do {
		std::string strFile = srcDir ();
		strFile += "/msgGroupId.h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
	os<<R"(#ifndef _msgGroupId_h__
#define  _msgGroupId_h__

enum msgGroupId
{
)";
	std::stringstream s2Full;
	auto& rPmpMgr = m_rPmp;
	auto& rMsgGroupS = rPmpMgr.msgGroupFileS ().msgGroupS ();
	// std::remove_reference<decltype (rMsgGroupS)>::type tempMap;
	std::map<int, decltype (rMsgGroupS.begin()->second)> tempMap;
	// using mapT = std::decay_t<decltype (rMsgGroupS)>;
	// std::map<int, std::decltype(std::declval(mapT)().begin())> tempMap;
	for (auto it = rMsgGroupS.begin (); rMsgGroupS.end () != it; ++it) {
		auto hasOrder = it->second->hasOrder ();
		if (hasOrder) {
			auto groupOrder = it->second->groupOrder ();
			auto inRet = tempMap.insert(std::make_pair(groupOrder, it->second));
			myAssert (inRet.second);
		}
	}
	int cur = 0;
	for (auto it = rMsgGroupS.begin (); rMsgGroupS.end () != it; ++it) {
		auto hasOrder = it->second->hasOrder ();
		if (hasOrder) {
			continue;
		}
		bool bIn = false;
		while (!bIn){
			auto inRet = tempMap.insert(std::make_pair(cur++, it->second));
			bIn = inRet.second;
		}
	}
	for (auto it = tempMap.begin(); tempMap.end() != it; ++it) {
	//for (auto it = rMsgGroupS.begin (); rMsgGroupS.end () != it; ++it) {
		auto pG = it->second.get();
		auto pName = pG->msgGroupName ();
		std::string enName = "msgGroupId_";
		enName += pName;
		os<<"    "<<enName.c_str ()<<" = "<<it->first<<","<<std::endl;
		s2Full<<"#define "<<pName<<"2FullMsg(p) ((uword)(("<<enName.c_str ()<<" * 100)+p))"<<std::endl;
		auto nR = msgIdGen(*(pG));
		if (nR) {
			nRet = 4;
			break;
		}
	}
	os<<"};"<<std::endl
	<<std::endl
	<<s2Full.str () <<std::endl
	<<"#endif";
} while (0);
    return nRet;
}

int  msgGen:: mkDir ()
{
    int  nRet = 0;
	do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		auto& rPmp = m_rPmp;
		auto pmpName = rPmp.pmpName ();
		std::string strFilename = projectHome;
			strFilename += "/";
			strFilename += pmpName;
			strFilename += "/src";
		setSrcDir (strFilename.c_str ());
		myMkdir (strFilename.c_str ());
	} while (0);
    return nRet;
}

const char*  msgGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  msgGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

int  msgGen:: msgIdGen (msgGroupFile& rGroup)
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto msgGroupName = rGroup.msgGroupName ();
		std::string strIDFull = msgGroupName;
		strIDFull += "MsgId";
		std::string strFile = srcDir ();
		strFile += "/";
		strFile += strIDFull;
		strFile += ".h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os<<R"(#ifndef _)"<<strIDFull<<R"(_h__
#define )"<<strIDFull<<R"(_h__
enum )"<<strIDFull<<R"(
{
)";
		auto& rpcNameS = rGroup. rpcNameS ();
		int index = 0;
		auto& rMsgMgr = rPmp.msgFileS ();
		for (auto it = rpcNameS.begin (); rpcNameS.end () != it; ++it) {
			std::string strAsk = *it;
			strAsk += "Ask";
			auto pAsk = rMsgMgr.findMsg (strAsk.c_str ());
			myAssert (pAsk);
			os<<"    "<<strIDFull<<"_"<<strAsk<<" = "<<index++<<","<<std::endl;
			std::string strRet = *it;
			strRet += "Ret";
			auto  pRet = rMsgMgr.findMsg (strRet.c_str ());
			if (pRet) {
				os<<"    "<<strIDFull.c_str()
					<<"_"<<strRet<<" = "<<index++<<","<<std::endl;
			}
		}
		os<<R"(};
#endif)";
    } while (0);
    return nRet;
}

std::stringstream&  msgGen:: protoOs ()
{
    return m_protoOs;
}

const char*  msgGen:: protoDir ()
{
    return m_protoDir.get ();
}

void  msgGen:: setProtoDir (const char* v)
{
    strCpy (v, m_protoDir);
}

std::stringstream&  msgGen:: pbcOs ()
{
    return m_pbcOs;
}

std::string&  msgGen:: strSerializeOut ()
{
    return m_strSerializeOut;
}

