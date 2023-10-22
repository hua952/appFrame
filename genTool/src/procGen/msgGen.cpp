#include "msgGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
#include <type_traits>
#include <utility>
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

int   msgGen:: genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool bRet, std::string& strOut)
{
    int   nRet = 0;
    do {
		std::stringstream ss;
		auto strN = rMsg.structName ();
		std::string strClass =rMsg.msgName (); // strN;
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
	auto& rData = *(rDv.rbegin ()->get ());
	auto len = rData.dataLength ();
	// if (len > 1) {
	if (len) {
		auto rDN = rData.dataName ();
		auto zeroEnd = rData.zeroEnd ();
		auto haveSize = rData.haveSize ();
		if (haveSize) {
			auto len = rData.dataLength ();
			ss<<R"(packetHead* )"<<strFunWon<<R"(toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
    )";
	
	ss<<strN<<R"(* p = (()"<<strN<<R"++(*)(N2User(pN)));
	)++";
	ss<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	bool bWZ = false;
	)";
	if (zeroEnd	) {
		ss<<R"(if(p->m_)"<<rDN<<R"(Num) {
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
	ss<<R"(myAssert (p->m_)"<<rDN<<R"(Num <= )"<<len<<R"();
	pN->udwLength = sizeof()"<<strN<<R"() - sizeof(p->m_)"
	<<rDN<<R"([0]) * ()"<<len<<R"( - p->m_)"<<rDN<<R"(Num);
	)";
	if (zeroEnd) {
		ss<<R"(if(bWZ) { 
			p->m_)"<<rDN<<R"([p->m_)"<<rDN<<R"(Num - 1] = 0;
		}
	)";
	}
	ss<<R"(return m_pPacket;
}
)";
		}
	} // if (len > 1)
}
ss<<std::endl;
	strOut = ss.str ();
    } while (0);
    return nRet;
}

int   msgGen:: genOnceStruct (structFile& rS, std::string& strOut)
{
    int   nRet = 0;
    do {
		std::stringstream os;
		auto sName = rS.structName ();
		os<<R"(struct )"<<sName<<R"(
{
)";
		auto& rDv = rS.dataOrder ();
		if (rDv.empty ()) {
			break;
		}
		for (auto iter = rDv.begin(); rDv.end() != iter; ++iter) {
			auto& rData = *(iter->get());
			std::string strD;
			genOnceData (rS, rData, strD);
			os<<strD;
		}
		os<<R"(};
)";
		strOut = os.str();
    } while (0);
    return nRet;
}


std::stringstream&  msgGen:: loadFileOs ()
{
    return m_loadFileOs;
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

int   msgGen:: genOnceData (structFile& rS, dataFile& rData, std::string& strOut)
{
    int   nRet = 0;
    do {
		auto pStructName = rS.structName ();
		auto pDN = rData.dataName ();

		std::stringstream ss;
		bool  haveSize  = rData.haveSize ();
		if (haveSize) {
			std::string strST = "udword";
			bool wordSize = rData.wordSize ();
			if (wordSize) {
				strST = "uword";
			}
			std::string sizeName = "m_";
			sizeName += pDN;
			sizeName += "Num";
			ss<<"    " << strST<<"    "<<sizeName<<";"<<std::endl;
			
			procDataCheck (pStructName, sizeName.c_str(), false);
		}
		std::string dDataName = "m_";
		dDataName += pDN;
		
		auto pDT = rData.dataType ();
		ss <<"    "<<pDT<<"    m_"<<pDN;
		auto len = rData.dataLength ();
		if (len) {
			ss<<" ["<<len<<"]";
			procDataCheck (pStructName, dDataName.c_str(), true);
		} else {
			procDataCheck (pStructName, dDataName.c_str(), false);
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
		std::stringstream ss;
		structFile* pSA = (structFile *)&rMsg;
		std::string strOutS;
		auto nR = genOnceStruct (*pSA, strOutS);
		myAssert (0 == nR);
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
		os<<R"(#include "myAssert.h"
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
		auto nR = genOnceMsgClassCpp (rG, *pAsk, false,strOut);	
		myAssert (0 == nR);
		os<<strOut;
		strN = *it;
		strN += "Ret";
		auto pRet = rMsgMgr.findMsg (strN.c_str ());
		if (pRet) {
			std::string strOut;
			auto nR = genOnceMsgClassCpp (rG, *pRet, true, strOut);	
			myAssert (0 == nR);
			os<<strOut;
		}
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
			genOnceStruct (rS, strS);
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
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		auto pPmpName = rPmp.pmpName ();
		strFilename += "/";
		strFilename += pPmpName;
		strFilename += "/CMakeLists.txt";
		std::ofstream os(strFilename);
		auto& rTool = rGlobal;
		auto outPutPath = rTool.outPutPath ();
		auto depIncludeHome = rTool.depIncludeHome ();
		auto depLibHome = rTool.depLibHome ();
		auto frameHome = rTool.frameHome ();
		auto frameLibPath = rTool.frameLibPath ();
		os<<"SET(prjName defMsg)"<<std::endl
			<<"SET(depIncludeHome "<<depIncludeHome<<")"<<std::endl
			<<"SET(depLibHome "<<depLibHome<<")"<<std::endl
			<<"SET(frameHome "<<frameHome<<")"<<std::endl
			<<"SET(frameLibPath "<<frameLibPath<<")"<<std::endl
			<<"SET(outPutPath "<<outPutPath<<")"<<std::endl
			<<R"(
set(srcS)
set(genSrcS)
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
add_library(${prjName} ${genSrcS} ${srcS} ${srcOS} ${defS})
target_include_directories(${prjName} PUBLIC 
							src
							${frameHome}/common/src
							${frameHome}/logicCommon/src
							${frameHome}/cLog/src
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

