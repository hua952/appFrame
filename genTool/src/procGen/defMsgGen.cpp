#include "defMsgGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/appFileMgr.h"
#include <fstream>
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
#include <sstream>
#include "myAssert.h"
#include "rLog.h"
#include "mainLoop.h"

defMsgGen:: defMsgGen ()
{
}

defMsgGen:: ~defMsgGen ()
{
}

int defMsgGen::loopHandleSGen ()
{
    int  nRet = 0;
    do {
		std::string strFile = srcDir ();
		strFile += "/loopHandleS.h";
		std::ofstream os(strFile);
		if (!os) {
			nRet = 1;
			break;
		}
		os <<R"(#ifndef _loopHandleS_h__
#define _loopHandleS_h__
)";
		
		auto& rAppS = tSingleton<appFileMgr>::single ().appS ();
		auto& rModMgr = tSingleton<moduleFileMgr>::single ();
		int ip = 0;
		for (auto it = rAppS.begin (); rAppS.end () != it; ++it) {
			auto& rApp = *(it->second.get ());
			int is = ip++ * LoopNum;
			rApp.setProcId (is);
			auto& rModNameS = rApp.moduleFileNameS ();
			for (auto ite = rModNameS.begin ();
					rModNameS.end () != ite; ++ite) {
				auto& rMName = *ite;
				auto pMod = rModMgr.findModule (rMName.c_str ());
				myAssert (pMod);

				// auto& rSS = pMod->serverS ();
				auto& rSS = pMod->orderS ();

				for (auto iter = rSS.begin ();
						rSS.end () != iter; ++iter) {
					auto pServer = iter->get (); // iter->second.get ();
					auto pSName = pServer->strHandle ();
					os<<R"(#define  )"<<pSName<<"  "<<is++<<std::endl;
				}
			}
		}
		os<<R"(#endif)";
    } while (0);
    return nRet;
}

int  defMsgGen:: CMakeListGen ()
{
	int  nRet = 0;
	do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		strFilename += "/defMsg/CMakeLists.txt";
		std::ofstream os(strFilename);
		// std::string strMgrFullName = m_pModel->name ();
		// strMgrFullName += "LogicModelMgr";
		auto& rTool = rGlobal;
		auto outPutPath = rTool.outPutPath ();
		// auto installPath = rTool.installPath ();
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
			// <<"SET(installPath"<<installPath<<")"<<std::endl
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

int  defMsgGen:: startGen ()
{
    int  nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		strFilename += "/defMsg/src";
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
		nR = loopHandleSGen ();
		if (nR) {
			nRet = 7;
			break;
		}
		nR = rpcInfoHGen();
		if (nR) {
			nRet = 8;
			break;
		}
		nR = rpcInfoCppGen();
		if (nR) {
			nRet = 8;
			break;
		}
    } while (0);
    return nRet;
}

int  defMsgGen:: msgGroupIdGen ()
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
	auto& rMsgGroupS = tSingleton <msgGroupFileMgr>::single ().msgGroupS ();
	for (auto it = rMsgGroupS.begin (); rMsgGroupS.end () != it; ++it) {
		auto pG = it->second.get();
		auto pName = pG->msgGroupName ();
		std::string enName = "msgGroupId_";
		enName += pName;
		os<<"    "<<enName.c_str ()<<","<<std::endl;
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

const char*  defMsgGen:: srcDir ()
{
    return m_srcDir.get ();
}

void  defMsgGen:: setSrcDir (const char* v)
{
    strCpy (v, m_srcDir);
}

int  defMsgGen:: mkDir ()
{
    int  nRet = 0;
    do {
		auto& rGlobal = tSingleton <globalFile>::single ();
		auto projectHome = rGlobal.projectHome ();
		std::string strFilename = projectHome;
		strFilename += "/defMsg/src";
		setSrcDir (strFilename.c_str ());
		myMkdir (strFilename.c_str ());
    } while (0);
    return nRet;
}

int  defMsgGen:: msgIdGen (msgGroupFile& rGroup)
{
    int  nRet = 0;
    do {
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
		auto& rMsgMgr = tSingleton<msgFileMgr>::single ();
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
				os<<"    "<<strIDFull<<"_"<<strRet<<" = "<<index++<<","<<std::endl;
			}
		}
		os<<R"(};
#endif)";
    } while (0);
    return nRet;
}

int   defMsgGen:: genOnceStruct (structFile& rS, std::string& strOut)
{
	int   nRet = 0;
	do {
		std::stringstream os;
		auto sName = rS.structName ();
		// rDebug ("process struct : "<<sName);
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
			genOnceData (rData, strD);
			os<<strD;
		}
		os<<R"(};
)";
		strOut = os.str();
	} while (0);
	return nRet;
}

int defMsgGen::msgStructGen ()
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
		auto& rSMgr = tSingleton <structFileMgr>::single ();
		// auto& rSS = rSMgr.structS ();
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

int   defMsgGen:: genOnceData (dataFile& rData, std::string& strOut)
{
    int   nRet = 0;
    do {
		std::stringstream ss;
		auto pDN = rData.dataName ();
		bool  haveSize  = rData.haveSize ();
		if (haveSize) {
			std::string strST = "udword";
			bool wordSize = rData.wordSize ();
			if (wordSize) {
				strST = "uword";
			}
			ss<<"    " << strST<<"    m_"<<pDN<<"Num;"<<std::endl;
		}
		auto pDT = rData.dataType ();
		ss <<"    "<<pDT<<"    m_"<<pDN;
		auto len = rData.dataLength ();
		if (len > 1) {
			ss<<" ["<<len<<"]";
		}
		ss<<";";
		auto pCom = rData.commit ();
		if (pCom) {
			ss<<R"(// )"<<pCom;
		}
		ss<<std::endl;
		strOut = ss.str();
    } while (0);
    return nRet;
}

int  defMsgGen:: rpcCppGen ()
{
    int  nRet = 0;
    do {
		auto& rGroup = tSingleton <msgGroupFileMgr>::single ().msgGroupS ();
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

int  defMsgGen:: rpcHGen ()
{
    int  nRet = 0;
    do {
		auto& rGroup = tSingleton <msgGroupFileMgr>::single ().msgGroupS ();
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

int   defMsgGen:: genOnceMsgClassH (msgFile& rMsg, std::string& strOut)
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
				if (len > 1) {
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

int   defMsgGen:: genOnceRpcH (msgGroupFile& rG)
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

	auto& rNameS =  rG.rpcNameS ();
	auto& rMsgMgr = tSingleton<msgFileMgr>::single ();
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

int   defMsgGen:: genOnceRpcCpp (msgGroupFile& rG)
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

)";

	auto& rNameS =  rG.rpcNameS ();
	auto& rMsgMgr = tSingleton<msgFileMgr>::single ();
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

int   defMsgGen:: genOnceMsgClassCpp (msgGroupFile& rG, msgFile& rMsg, bool bRet, std::string& strOut)
{
    int   nRet = 0;
    do {
		// auto msgGroupName = rG.msgGroupName ();
		std::stringstream ss;
		auto strN = rMsg.structName ();
		std::string strClass =rMsg.msgName (); // strN;
		// strClass += "Msg";
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
		ss<<strFunWon<<strClass<<R"( ()
{
	m_pPacket = (packetHead*)allocPacket()"<<pSize<<R"();
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = )"<<fullChangName<<R"(()"
	<<strMsgId<<R"();
	)";
	if (bRet) {
		ss<<R"(NSetRet(pN);
	)";
	}
	ss<<R"(
}
)"<<strFunWon<<strClass<<R"( (packetHead* p):CMsgBase(p)
{
}
)";

	auto &rDv = rMsg.dataOrder ();
	if (!rDv.empty ()) {
		ss<<strN<<R"(* )"<<strFunWon<<R"(pack()
{
    return (()"<<strN<<R"(*)(P2User(m_pPacket)));
}
)";
	auto& rData = *(rDv.rbegin ()->get ());
	auto len = rData.dataLength ();
	if (len > 1) {
		auto haveSize = rData.haveSize ();
		if (haveSize) {
			auto rDN = rData.dataName ();
			auto len = rData.dataLength ();
			ss<<R"(packetHead* )"<<strFunWon<<R"(toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
    )"<<strN<<R"(* p = (()"<<strN<<R"++(*)(N2User(pN)));
	myAssert (p->m_)++"<<rDN<<R"(Num < )"<<len<<R"();
	pN->udwLength = sizeof()"<<strN<<R"() - sizeof(p->m_)"
	<<rDN<<R"([0]) * ()"<<len<<R"( - p->m_)"<<rDN<<R"(Num);
	return m_pPacket;
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

int  defMsgGen:: rpcInfoHGen ()
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

void regRpcS (const ForMsgModuleFunS* pForLogic);
#endif)";
    } while (0);
    return nRet;
}

int  defMsgGen:: rpcInfoCppGen ()
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
#include "loopHandleS.h"
)";
		auto& rMap = tSingleton<msgGroupFileMgr>::single ().msgGroupS ();
		for (auto it = rMap.begin ();rMap.end () != it; ++it) {
			auto& strName = it->first;
			os<<R"(#include ")"<<strName<<R"(MsgId.h")"<<std::endl;
		}
		os<<R"(

void regRpcS (const ForMsgModuleFunS* pForLogic)
{
	auto regRpc = pForLogic->fnRegRpc;
)";
	auto& rMsgMgr = tSingleton <msgFileMgr>::single ();
	
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

