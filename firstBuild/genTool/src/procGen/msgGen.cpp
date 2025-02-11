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
#include <vector>
#include<filesystem>
using namespace std::filesystem;

msgGen::msgGen (msgPmpFile& rPmp):m_rPmp(rPmp)
{
}

msgGen:: ~msgGen ()
{
}

int  msgGen::startGen ()
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		auto pPmpName = rPmp.pmpName ();
		strFilename += "/gen/";
		strFilename += pPmpName;
		strFilename += "/src";
		myMkdir (strFilename.c_str ());
		
		auto& proOs = protoOs ();
		
		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto& rApps = rAppMgr.appS ();
		if (rApps.size() > 1) {
			auto strPro = strFilename;
			strPro += "/protobuf/";
			myMkdir (strPro.c_str());
			setProtoDir (strPro.c_str());

			std::string strDir = projectHome;
			strDir += "/gen/protobufSer/src";
			myMkdir (strDir.c_str());
			setProtobufSerSrcDir (strDir.c_str());
		}

		proOs<<R"(syntax = "proto3";
)";
		int nR = 0;
		nR = mkDir ();
		if (nR) {
			nRet = 1;
			break;
		}
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

		std::stringstream  strSerializeOut;
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
		nR = msgStructGen (strSerializeOut);
		if (nR) {
			nRet = 4;
			break;
		}

		strVec serFunStuOs;
		auto serializePackFunStName = rPmp.serializePackFunStName ();

		nR = rpcHGen (strSerializeOut, serFunStuOs);
		if (nR) {
			nRet = 5;
			break;
		}
		std::stringstream serArrayOs;
		std::stringstream    strToPack;
		nR = rpcCppGen (strToPack, strSerializeOut, serArrayOs);
		if (nR) {
			nRet = 6;
			break;
		}
		
		nR = rpcInfoHGen();
		if (nR) {
			nRet = 8;
			break;
		}

		if (rApps.size() > 1) {
			std::string aProtoFile = protobufSerSrcDir ();
			std::string strProtoFile = aProtoFile;
			create_directories (strProtoFile);
			strProtoFile += "/";
			strProtoFile += pPmpName;
			strProtoFile += ".proto"; 

			std::ofstream protoOsF (strProtoFile.c_str());
			if (!protoOsF) {
				nRet = 9;
				break;
			}

			protoOsF<<proOs.str();
			protoOsF.close ();
			std::stringstream protocOs;
			auto vcpkg_tool = std::getenv("VCPKG_HOME");
			auto vcpkg_dll = std::getenv("VCPKG_OS");
			protocOs<<vcpkg_tool<<"/packages/protobuf_"<<vcpkg_dll<<"/tools/protobuf/protoc --cpp_out="<<aProtoFile<<" --proto_path="<<aProtoFile<<" "<<pPmpName<<".proto";
			rInfo(" will exc : "<<protocOs.str());
			system (protocOs.str().c_str());
		}

		auto& rConfig = tSingleton<configMgr>::single ();
		auto enStructBadyType = rConfig.structBadyType ();
		std::stringstream incOs;
		std::stringstream incToPackOs;

		incOs<<R"(#include "myAssert.h"
#include "strFun.h"
#include "rpcInfo.h"
#include <memory>
)";
		incToPackOs<<incOs.str();
		if (enStructBadyType == structBadyTime_proto) {
			incOs<<R"(#include ")"<<pPmpName<<R"(.pb.h"
)";
			incToPackOs<<R"(#include ")"<<serializePackFunStName<<R"(.h"
)";
		}
		std::string strSerWFile;
		std::stringstream ssInc;
		auto& rGroup = rPmp.msgGroupFileS ().msgGroupS ();
		for (auto it = rGroup.begin (); rGroup.end () != it; ++it) {
			auto& rG = *(it->second.get());
			auto msgGroupName = rG.msgGroupName ();
			
			
			incOs<<R"(#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

)";

ssInc<<R"(#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

)";

			incToPackOs<<R"(#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

)";

		}
		if (enStructBadyType == structBadyTime_com) {
			strSerWFile = comSerFileName ();
		} else if (enStructBadyType == structBadyTime_proto) {
			strSerWFile = protoSerFileName ();
		} else {
			myAssert (0);
		}

		auto nMsgNum = rPmp.msgFileS().msgS ().size ();
		std::ofstream ofSer (strSerWFile.c_str());
		ofSer<<incToPackOs.str()<<R"(packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
)"<<strToPack.str();


		if (rApps.size() <= 1) {
			break;
		}
			std::string aProtoFile = protobufSerSrcDir ();
			std::string strStFunS = aProtoFile;
			strStFunS += "/";
			strStFunS += serializePackFunStName;
			strStFunS += ".h";

			std::ofstream ofStH (strStFunS.c_str());
			ofStH<<R"(#include  "msg.h"
#include  "msgGroupId.h"
)"<<ssInc.str()<<std::endl
<<R"(struct )"<<serializePackFunStName <<R"(
{
)";
			for (auto it = serFunStuOs.begin (); it != serFunStuOs.end (); it++) {
				ofStH<<"	serializePackFunType   "<<it->strMsgId<<";"<<std::endl
				<<"	serializePackFunType   "<<it->second.first<<";"<<std::endl
				<<"	serializePackFunType   "<<it->second.second<<";"<<std::endl;
			}
			ofStH<<R"(};
)";
			std::string strProtoSFile = aProtoFile;
			strProtoSFile += "/";
			strProtoSFile += pPmpName;
			strProtoSFile += "Ser.cpp"; 
			std::ofstream ofPro (strProtoSFile.c_str());
			ofPro<<incOs.str()<<R"(#include ")"<<serializePackFunStName<<R"(.h"

)"<<strSerializeOut.str()<<R"(
extern "C"
{
int  getSerializeFunS (serializePackFunType* pFunS, int maxNum, ForLogicFun* pForLogic)
{
	int nRet = 0;
	myAssert ((maxNum % 3) == 0);
	setForMsgModuleFunS (pForLogic);
	do {
)";
	for (auto it = serFunStuOs.begin (); it != serFunStuOs.end (); it++) {
		ofPro<<R"(	pFunS[nRet++] = (serializePackFunType)()"<<it->first<<R"();
)"<<R"(	pFunS[nRet++] = )";

	if (it->nCom == structBadyTime_com ) {
		ofPro<<"nullptr";
	} else if (it->nCom == structBadyTime_proto ) {
		ofPro<<it->second.first;
	} else {
		myAssert (0);
	}
	ofPro<<R"(;
)"<<R"(	pFunS[nRet++] = )"<<it->second.second<<R"(;
	if (nRet >= maxNum) {
		break;
	}
)";
	}
	ofPro<<R"(	} while (0);
	return nRet;
}
}
)";
		// } // if
    } while (0);
    return nRet;
}

int  msgGen:: rpcInfoCppGen ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/rpcInfo.cpp";
		std::ofstream infoFileOs(strFile);
		if (!infoFileOs) {
			nRet = 1;
			break;
		}
		// std::ofstream os(strFile);
		std::stringstream os;
		std::stringstream defProcOs;
		
		std::stringstream incOs;
		incOs<<R"(#include "msgGroupId.h"
#include "msg.h"
#include <fstream>
#include <map>
#include "rpcInfo.h"
#include "myAssert.h"
#include "loopHandleS.h"
)";
		auto& rPmp = m_rPmp;
		auto& rMap = rPmp.msgGroupFileS ().msgGroupS ();
		for (auto it = rMap.begin ();rMap.end () != it; ++it) {
			auto& strName = it->first;
			incOs<<R"(#include ")"<<strName<<R"(MsgId.h")"<<std::endl;
			incOs<<R"(#include ")"<<strName<<R"(Rpc.h")"<<std::endl;
		}
		incOs<<R"(
)";
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

)";
	defProcOs<<R"(int getDefProc (loopHandleType* pBuff, int buffNum)
{
	int nRet = 0;
	do {
		if (nRet >= buffNum) {
			break;
		}
		)";

		
	auto regOsP = std::make_unique<std::stringstream>();
	auto& regOs = *(regOsP.get()); 
	regOs<<incOs.str();
	regOs<<R"(
extern "C"
{
int getMsgPairS (uword* pBuff, int buffNum)
{
	myAssert(buffNum%2==0);
	int nRet = 0;
	do {
		if (nRet >= buffNum) {
			break;
		}
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

		auto pFull = rG.fullChangName ();
		std::string askFullId = pFull;
		askFullId += R"(()";
		askFullId += pAsk->strMsgId ();
		askFullId += ")";
		auto pAskDefPro = pAsk->defProServerTmpId (); // defProServerId ();
		
		if (pAskDefPro) {
			defProcOs<<R"(pBuff[nRet++] = )"<<askFullId <<R"(;
		if (nRet >= buffNum) {
			break;
		}
		pBuff[nRet++] = )"<<pAskDefPro<<R"(;
		if (nRet >= buffNum) {
			break;
		}
		)";
		}
		const char* pRetDefPro = "c_emptyLoopHandle";
		auto pRet = rMsgMgr.findMsg (trySId.c_str ());
		
		if (pRet) {
			pRetDefPro = pRet->defProServerTmpId (); //defProServerId ();
			
			retId = pFull;
			retId += R"(()";
			retId += pRet->strMsgId ();
			retId += ")";
			if (pRetDefPro) {
		defProcOs<<R"(pBuff[nRet++] = )"<<retId<<R"(;
		if (nRet >= buffNum) {
			break;
		}
		pBuff[nRet++] = )"<<pRetDefPro<<R"(;
		if (nRet >= buffNum) {
			break;
		}
		)";
			}
			
		} 
	
		regOs<<R"(    pBuff[nRet++] = )"<<askFullId<<R"(;
		pBuff[nRet++] = )"<<retId<<R"(;
		if (nRet >= buffNum) {
			break;
		}
		)";
	}
	} // for (G
	
	regOs<<R"(
	} while (0);
	return nRet;
}
})";

defProcOs<<R"(
	} while (0);
	return nRet;
})";

	infoFileOs<<incOs.str()<<std::endl<<os.str()<<defProcOs.str();

		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto& rApps = rAppMgr.appS ();
		if (rApps.size() > 1) {
			std::string regFile = protobufSerSrcDir ();
			regFile += "/regRpcPair.cpp";
			std::ofstream regOsRe (regFile.c_str());
			if (!regOsRe) {
				nRet = 2;
				break;
			}
			regOs<<os.str();
			regOsRe<<regOs.str();
		}
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
#include "arrayMap.h"

void dumpStructS ();
int  checkStructS (const char* szFile);
// void regRpcS (const ForMsgModuleFunS* pForLogic);
int getDefProc (uword* pBuff, int buffNum);

#endif)";
    } while (0);
    return nRet;
}

int   msgGen:: genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool bRet, std::string& strOut, std::stringstream& strToPack, std::stringstream&  strSerializeOut, std::stringstream& serArrayOs)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		std::stringstream ss;
		std::stringstream& ssP = strSerializeOut;
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

	std::vector<std::string> zerV;
	if (!rDv.empty ()) {
		for (auto it = rDv.begin (); it != rDv.end (); it++) {
			auto& rD = *(it->get ());
			auto l = rD.dataLength ();
			auto z = rD.zeroEnd ();
			if (l && z) {
				zerV.push_back(rD.dataName());
			}
		}
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

		auto serFunStuFromName = rMsg.serFunStuFromName ();
		auto serFunStuToName = rMsg.serFunStuToName ();
		std::string serPackToFunName = serFunStuToName;

		if (structBadyTime_com != bt) {
			serPackToFunName = serPackToFunName + "Before";
		}
		std::stringstream ssBeforSerTo;
		if (len) {
			if (haveSize) {

				ssBeforSerTo<<"static int "<<serPackToFunName<<R"( (netPacketHead* pN, pPacketHead& pNew)
{
    )";

				ssBeforSerTo<<strN<<R"(* p = (()"<<strN<<R"++(*)(N2User(pN)));
	)++";
				ssBeforSerTo<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	bool bWZ = false;
	)";
				if (zeroEnd	) {
					ssBeforSerTo<<R"(if(p->m_)"<<rDN<<R"(Num) {
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
				ssBeforSerTo<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	pN->udwLength = sizeof()"<<strN<<R"() - sizeof(p->m_)"
				<<rDN<<R"([0]) * ()"<<len<<R"( - p->m_)"<<rDN<<R"(Num);
	)";
				if (zeroEnd) {
					ssBeforSerTo<<R"(if(bWZ) { 
			p->m_)"<<rDN<<R"([p->m_)"<<rDN<<R"(Num - 1] = 0;
				}
				)";
				}

				ssBeforSerTo<<R"( return 0;
}
)";
				}
}
		if (structBadyTime_com == bt) {
			if (!ssBeforSerTo.str().empty()) {
				ssP<<ssBeforSerTo.str();
			}
		} else if (structBadyTime_proto == bt) {
			auto toPbFu = rMsg.toPbFuName ();
			auto fromFuName = rMsg.fromPbFuName ();

			std::stringstream  fromSer;
			std::stringstream  toSer;
			fromSer<<"static int "<<serFunStuFromName<<R"( (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
	)"<<msgName<<R"(	newC;
	)"<<strN<<R"(Proto  msgPb;
	auto pOU = ()"<<strN<<R"(*)(N2User(pN));
	auto bRet = msgPb.ParseFromArray (pOU, pN->udwLength);
	myAssert (bRet);
	auto pU = newC.pack ();
	pNew = newC.pop ();
	auto pNN = P2NHead(pNew);
	// auto newLen = pNN->udwLength;
	*pNN = *pN;
	// pNN->udwLength = newLen;
)";
	fromSer<<R"(
	)"<<fromFuName<<R"((*pU, msgPb);
)";
	if (haveSize) {
		fromSer<<R"(	pNN->udwLength = sizeof()"<<strN<<R"() - sizeof(pU->m_)"
				<<rDN<<R"([0]) * ()"<<len<<R"( - pU->m_)"<<rDN<<R"(Num);)";
	}
fromSer<<R"(
	return nRet;
}
)";
	if (!ssBeforSerTo.str().empty()) {
		toSer<<ssBeforSerTo.str();
	}
	toSer<<"static int "<<serFunStuToName<<R"( (netPacketHead* pN, pPacketHead& pNew)
{
	int nRet = 0;
)";
	if (!ssBeforSerTo.str().empty()) {
		toSer<<serPackToFunName<<R"((pN, pNew);)";
	}
	toSer<<R"(
	auto pU = ()"<<strN<<R"(*)(N2User(pN));
	)"<<strN<<R"(Proto  msgPb;
	)"<<toPbFu<<R"((*pU, msgPb);
	auto newLen = msgPb.ByteSizeLong();
	pNew = (packetHead*)allocPacket(newLen);
	auto pNN = P2NHead(pNew);
	auto pNU = ()"<<strN<<R"(*)(N2User(pNN));
	msgPb.SerializeToArray (pNU, newLen);
	*pNN = *pN;
	pNN->udwLength = newLen;
	return nRet;
}
)";
	strSerializeOut << fromSer.str();
	strSerializeOut << toSer.str();

	serArrayOs<<R"(pSerFun->fromFun =  s_pFunS->)"<<serFunStuFromName<<R"(;
	pSerFun->toFun =  s_pFunS->)"<<serFunStuToName<<R"(;
	pSerFun->msgId = )"<<strMsgId <<R"(;
	pSerFun++;
		)";
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
		}
	}
	if (!zerV.empty()) {
		ss<<strN<<R"(* p2 = (()"<<strN<<R"++(*)(N2User(pN)));
	)++";
				for (auto it = zerV.begin (); it != zerV.end (); it++) {
					
				ss<<R"(p2->m_)"<<*it<<R"([0] = 0;
	)";

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

int   msgGen:: genOnceStruct (structFile& rS, std::stringstream& strOut, std::stringstream& protoStr, std::stringstream& strSerializeOut)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		auto& rDv = rS.dataOrder ();
		if (rDv.empty ()) {
			break;
		}
		std::stringstream& os = strOut;
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
			osFrom<<pFromFull<<std::endl<<"{"<<std::endl;
			auto pToFull = rS.toPbFuFullName ();
			osTo<<pToFull<<std::endl<<"{"<<std::endl;
		}
		
		int nIndex = 1;
		for (auto iter = rDv.begin(); rDv.end() != iter; ++iter) {
			auto& rData = *(iter->get());
			// std::string strD;
			genOnceData (rS, rData, /*strD*/ os, protoStr, nIndex++, osFrom, osTo); // os<<strD;
		}
		os<<R"(};
)";

protoStr<<R"(}

)";

	if (structBadyTime_proto == bt) {
		osFrom<<"}"<<std::endl<<std::endl;
		osTo<<"}"<<std::endl<<std::endl;
		strSerializeOut << osFrom.str();
		strSerializeOut << osTo.str();
	}
		// strOut = os.str();
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

int   msgGen:: genOnceData (structFile& rS, dataFile& rData, std::stringstream& strOut, std::stringstream& protoStr, int nIndex, std::stringstream& fromOs, std::stringstream& toOs)
{
    int   nRet = 0;
    do {
		msgPmpFile& rPmp = m_rPmp;
		auto& rPTS = tSingleton <globalFile>::single ().protoDataTypeS ();
		auto pStructName = rS.structName ();
		auto pDN = rData.dataName ();
		auto strDN = rData.memberName ();
		std::stringstream &ss = strOut;
		std::string sizeName = "m_";
		sizeName += pDN;
		sizeName += "Num";
		auto& numName= sizeName;
		auto& dataS = rS.dataS ();
		bool lastData = dataS.rbegin()->first == pDN;
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
		auto lowerName = rData.lowerName ();
		auto& rStMap = rPmp.structFileS ().structS ();
		auto len = rData.dataLength ();
		if (len) {
			fromOs<<"auto	"<<strDN<<"MaxLen = sizeof (rData."<<strDN<<")/sizeof (rData."<<strDN<<"[0]);"<<std::endl;
			auto zeroEnd = rData.zeroEnd ();
			if (zeroEnd) {
				fromOs<<"	"<<strDN<<"MaxLen--;"<<std::endl;
			}
			ss<<" ["<<len<<"]";
			auto  bString = true;
			procDataCheck (pStructName, dDataName.c_str(), true);
			if (0 == strcmp (pDT, "char")) {
				// protoStr<<"	string "<<pDN<<" = "<<nIndex<<";"<<std::endl;
				protoStr<<"	bytes "<<pDN<<" = "<<nIndex<<";"<<std::endl;
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
				fromOs<<"	myAssert(rPb."<<lowerName<<"().size() <="<<strDN<<"MaxLen);"<<std::endl;
				if (haveSize) {
					fromOs<<"	rData."<<numName<<" = (decltype(rData."<<numName<<"))(rPb."<<lowerName<<"().size());"<<std::endl;
				}
				if (zeroEnd) {
					if (haveSize) {
						fromOs<<"	rData."<<numName<<"++;"<<std::endl;
					}
					fromOs<<"	strNCpy (rData."<<strDN<<", sizeof (rData."<<strDN<<"), rPb."<<lowerName<<"().c_str());"<<std::endl;
					toOs<<"	auto "<<strDN<<"StrLen = strlen("<<"rData."<<strDN<<");"<<std::endl;
					toOs<<"	rPb.set_"<<lowerName<<" ((char*)(rData."<<strDN<<"), "<<strDN<<"StrLen);"<<std::endl;
				} else {
					
					fromOs<<"	memcpy (rData."<<strDN<<",  rPb."<<lowerName<<"().data(), rPb."<<lowerName<<"().size());"<<std::endl;
					if (haveSize && lastData ) {
						toOs<<"	rPb.set_"<<lowerName<<" ((char*)(rData."<<strDN<<"), rData."<<numName<<");"<<std::endl;
					} else {
						toOs<<"	rPb.set_"<<lowerName<<" ((char*)(rData."<<strDN<<"), sizeof(rData."<<strDN<<"));"<<std::endl;
					}
				}
			} else {
				fromOs<<"	myAssert(rPb."<<lowerName<<"_size() <="<<strDN<<"MaxLen);"<<std::endl;
				if (haveSize) {
					fromOs<<"	rData."<<numName<<" = (decltype(rData."<<numName<<"))(rPb."<<lowerName<<"_size());"<<std::endl;
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
		auto    )"<<lowerName<<R"(MySize = rPb.)"<<lowerName<<R"(_size ();
		myAssert ()"<<lowerName<<R"(MySize <= )"<<len<<R"();
		for (int i = 0; i < )"<<lowerName<<R"(MySize; i++) {
			)"<<subForFuN<<"(rData."<<strDN<<"[i], rPb."<<lowerName<<R"((i));
	}
		)";
					toOs<<R"(
		for (int i = 0; i < )"<<len<<R"(; i++) {
			)"<<subToFuN<<"(rData."<<strDN<<"[i], *rPb.add_"<<lowerName<<R"(());
		}
		)";
				} else {
					strDT = it->second;
					fromOs<<R"(
		auto    )"<<lowerName<<R"(MySize = rPb.)"<<lowerName<<R"(_size ();
		myAssert ()"<<lowerName<<R"(MySize <= )"<<len<<R"();
		for (int i = 0; i < )"<<lowerName<<R"(MySize; i++) {
			rData.)"<<strDN<<"[i] = rPb."<<lowerName<<R"((i);
	}
		)";
					toOs<<R"(
		for (int i = 0; i < )"<<len<<R"(; i++) {
			rPb.add_)"<<lowerName<<R"((rData.)"<<strDN<<R"([i]);
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
					auto ite = rStMap.find (pDT);
					myAssert (ite != rStMap.end ());
					auto subForFuN = ite->second->fromPbFuName ();
					auto subToFuN = ite->second->toPbFuName ();
					// auto subForFuN = rS.fromPbFuName ();
					// auto subToFuN = rS.toPbFuName ();
					fromOs<<"	"<<subForFuN<<"(rData."<<strDN<<", rPb."<<lowerName<<"());"<<std::endl;
					toOs<<"	"<<subToFuN<<"(rData."<<strDN<<", *rPb.mutable_"<<lowerName<<"());"<<std::endl;
			} else {
				strDT = it->second;
				fromOs<<"	rData."<<strDN<<" = rPb."<<lowerName<<"();"<<std::endl;
				toOs<<"	rPb.set_"<<lowerName<<" (rData."<<strDN<<");"<<std::endl;
			}
			protoStr<<"	"<<strDT<<" "<<pDN<<" = "<<nIndex<<";"<<std::endl;
		}
		ss<<";";
		auto pCom = rData.commit ();
		if (pCom) {
			ss<<R"(/* )"<<pCom<<" */";
		}
		ss<<std::endl;
		// strOut = ss.str();
    } while (0);
    return nRet;
}

std::stringstream&  msgGen:: checkOs ()
{
    return m_checkOs;
}

int   msgGen:: genOnceMsgClassH (msgFile& rMsg, std::stringstream& strOut, std::stringstream&  strSerializeOut,  strVec& serFunStuOs)
{
    int   nRet = 0;
    do {
		auto& rConfig = tSingleton<configMgr>::single ();
		std::stringstream& ss = strOut;
		structFile* pSA = (structFile *)&rMsg;
		std::stringstream strOutS;
		// std::string&  strSerializeOut = this->strSerializeOut ();
		auto nR = genOnceStruct (*pSA, strOutS, m_protoOs, strSerializeOut);
		myAssert (0 == nR);
		// auto& rPbc = pbcOs ();
		// rPbc<<strPbChangeOut;
		if (!strOutS.str().empty ()) {
			ss<<strOutS.str();
		}
		auto strN = rMsg.structName ();
		ss<<R"(class )"<<strN<<R"(Msg :public CMsgBase
{
public:
	)"<<strN<<R"(Msg ();
	)"<<strN<<R"(Msg (packetHead* p);
	)";
		if (!strOutS.str().empty ()) {
			
			ss<<strN<<R"(* pack ();
    )";
			auto &rDv = rMsg.dataOrder ();
			if (!rDv.empty ()) {
				auto enStructBadyType = rConfig.structBadyType ();
				bool stPowerCom = rMsg.powerCom ();
				bool bWCom = false;

				strVecV node;
				auto serFunStuFromName = rMsg.serFunStuFromName ();
				auto serFunStuToName = rMsg.serFunStuToName ();
				node.strMsgId = rMsg.strMsgId ();
				node.first = rMsg.msgFullName ();
				if (structBadyTime_com == enStructBadyType) {
					node.nCom = structBadyTime_com;
					bWCom = true;
				} else if (structBadyTime_proto == enStructBadyType) {
					if (stPowerCom) {
						node.nCom = structBadyTime_com;
						bWCom = true;
					} else {
						node.nCom = structBadyTime_proto;
					}
				} else {
					myAssert (0);
				}

				if (structBadyTime_com == node.nCom) {
					auto& rData = *(rDv.rbegin ()->get ());
					auto len = rData.dataLength ();
					if (len) {
						auto haveSize = rData.haveSize ();
						if (haveSize) {
							node.second.first = serFunStuFromName;
							node.second.second = serFunStuToName;
							serFunStuOs.push_back (node);
						}
					}
				} else {
					node.second.first = serFunStuFromName;
					node.second.second = serFunStuToName;
					serFunStuOs.push_back (node);
				}
			}
		}
		ss<<R"(
};
)";
	// strOut = ss.str ();
    } while (0);
    return nRet;
}

int   msgGen:: genOnceRpcH (msgGroupFile& rG, std::stringstream&  strSerializeOut,  strVec& serFunStuOs)
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
		std::stringstream strOut;
		auto nR = genOnceMsgClassH (*pAsk, strOut, strSerializeOut, serFunStuOs);	
		myAssert (0 == nR);
		os<<strOut.str();
		strN = *it;
		strN += "Ret";
		auto pRet = rMsgMgr.findMsg (strN.c_str ());
		if (pRet) {
			std::stringstream strOut;
			auto nR = genOnceMsgClassH (*pRet, strOut, strSerializeOut, serFunStuOs);	
			myAssert (0 == nR);
			os<<strOut.str();
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

int   msgGen:: genOnceRpcCpp (msgGroupFile& rG, std::stringstream&  strToPack, std::stringstream&  strSerializeOut, std::stringstream& serArrayOs)
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
	
	os<<R"(
#include "msgGroupId.h"
#include ")"<<msgGroupName<<R"(MsgId.h"
#include ")"<<msgGroupName<<R"(Rpc.h"

packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
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
		auto nR = genOnceMsgClassCpp (rG, *pAsk, false,strOut, strToPack, strSerializeOut, serArrayOs);	
		myAssert (0 == nR);
		os<<strOut;
		strN = *it;
		strN += "Ret";
		auto pRet = rMsgMgr.findMsg (strN.c_str ());
		if (pRet) {
			std::string strOut;
			auto nR = genOnceMsgClassCpp (rG, *pRet, true, strOut, strToPack, strSerializeOut, serArrayOs);	
			myAssert (0 == nR);
			os<<strOut;
		}
	}
    } while (0);
    return nRet;
}

int  msgGen:: rpcHGen (std::stringstream&  strSerializeOut,  strVec& serFunStuOs)
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGroup = rPmp.msgGroupFileS ().msgGroupS ();
		for (auto it = rGroup.begin (); rGroup.end () != it; ++it) {
			auto& rG = *(it->second.get());
			auto nR = genOnceRpcH (rG, strSerializeOut, serFunStuOs);
			if (nR) {
				nRet = 1;
				break;
			}
		}
    } while (0);
    return nRet;
}

int  msgGen:: rpcCppGen (std::stringstream&  strToPack, std::stringstream& strSerializeOut, std::stringstream& serArrayOs)
{
    int  nRet = 0;
    do {
		auto& rPmp = m_rPmp;
		auto& rGroup = rPmp.msgGroupFileS ().msgGroupS ();;
		for (auto it = rGroup.begin (); rGroup.end () != it; ++it) {
			auto& rG = *(it->second.get());
			auto nR = genOnceRpcCpp (rG, strToPack, strSerializeOut, serArrayOs);
			if (nR) {
				nRet = 1;
				break;
			}
		}
    } while (0);
    return nRet;
}

int  msgGen:: msgStructGen (std::stringstream&  strSerializeOut)
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
		std::stringstream strS;
		for (auto it = rSS.begin (); rSS.end () != it; ++it)
		{
			auto& rS = *(it->get ());
			std::string protoStr;
			// std::string&  strSerializeOut = this->strSerializeOut ();
			genOnceStruct (rS, strS, m_protoOs, strSerializeOut);
			// auto& rPbc = pbcOs ();
			// rPbc<<strPbChangeOut;
			//os << strS;
		}
		os << strS.str();
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
		strFilename += "/gen/";
		strFilename += pPmpName;
		strFilename += "/CMakeLists.txt";
		std::ofstream os(strFilename);
		auto& rTool = rGlobal;
		// auto outPutPath = rTool.outPutPath ();
		// auto depIncludeHome = rTool.depIncludeHome ();
		// auto depLibHome = rTool.depLibHome ();
		// auto frameHome = rTool.frameHome ();
		auto frameLibPath = rTool.frameLibPath ();
		std::string framePath = rTool.frameInstallPath ();
		os<<"SET(prjName defMsg)"<<std::endl
			// <<"SET(depIncludeHome "<<depIncludeHome<<")"<<std::endl
			// <<"SET(depLibHome "<<depLibHome<<")"<<std::endl
			// <<"SET(frameHome "<<frameHome<<")"<<std::endl
			<<"SET(frameLibPath "<<frameLibPath<<")"<<std::endl
			// <<"SET(outPutPath "<<outPutPath<<")"<<std::endl
			<<R"(
set(srcS)
set(genSrcS)
set(serFile)
set(pbFileS)
)";

		auto bt = rConfig.structBadyType ();
		if (bt == structBadyTime_com) {
			auto pFile = comSerFileName ();
			os<<R"(list(APPEND serFile )"<<pFile<<R"()
			)";
		} else if (bt == structBadyTime_proto) {
			auto pFile = protoSerFileName ();
			os<<R"(list(APPEND serFile )"<<pFile<<R"()
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
	include_directories(
		${depIncludeHome}
		)
	link_directories(
		${depLibHome}
		)
endif ()
add_library(${prjName} ${genSrcS} ${srcS} ${srcOS} ${defS} ${pbFileS})
target_include_directories(${prjName} PRIVATE
)";

		auto& rAppMgr = tSingleton<appFileMgr>::single ();
		auto& rApps = rAppMgr.appS ();
		if (rApps.size() > 1) {
			os<<R"(${CMAKE_SOURCE_DIR}/protobufSer/src)";
		}
							os<<R"(src
							)";
							os<<framePath<<"/include/appFrame"<<std::endl;

	bool bWProto = ((structBadyTime_proto == bt));
	if (bWProto) {
		os<<R"(src/protobuf
							)";
	}
                           os<<R"(
						   )

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
		create_directories (strFile);
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
			strFilename += "/gen/";
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
#define _)"<<strIDFull<<R"(_h__
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

const char*  msgGen:: protobufSerSrcDir ()
{
    return m_protobufSerSrcDir.get ();
}

void  msgGen:: setProtobufSerSrcDir (const char* v)
{
    strCpy (v, m_protobufSerSrcDir);
}

