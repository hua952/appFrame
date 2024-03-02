#include "moduleCMakeListsGen.h"
#include "strFun.h"
#include "tSingleton.h"
#include "fromFileData/globalFile.h"
#include "fromFileData/serverFile.h"
#include "fromFileData/moduleFile.h"
#include "fromFileData/rpcFile.h"
#include "fromFileData/rpcFileMgr.h"
#include "fromFileData/msgPmpFile.h"
#include "moduleGen.h"
#include <string>
#include "rLog.h"
#include <fstream>
#include <set>

moduleCMakeListsGen:: moduleCMakeListsGen ()
{
}

moduleCMakeListsGen:: ~moduleCMakeListsGen ()
{
}

int   moduleCMakeListsGen:: startGen (moduleGen& rModel)
{
	int   nRet = 0;
	do {
		auto& rMod = rModel.moduleData ();
		auto moduleName = rMod.moduleName ();
		auto& rGlobalFile = tSingleton<globalFile>::single ();
		auto appName = rMod.appName ();

		std::string strFile = rGlobalFile.projectHome ();
		strFile += appName;
		strFile += "/";
		strFile += moduleName;
		strFile += "/CMakeLists.txt";
		auto bE = isPathExit (strFile.c_str());
		if (bE) {
			nRet = 0;
			break;
		}
		std::ofstream os(strFile.c_str ());
		if (!os) {
			nRet = 1;
			rError ("create file error file name is : "<<strFile.c_str ());
			break;
		}
		os<<"SET(prjName "<<moduleName<<")"<<std::endl
			<<R"(set(serSrcS))"<<std::endl;

		std::string strPer = " src/procMsg/";
		auto& rData = rMod;
		auto& rSS = rData.orderS ();
		std::stringstream incS;
		std::stringstream cppS;

		using groupSet = std::set<std::string>;
		for (auto it = rSS.begin (); rSS.end () != it; ++it) {
			auto pName = it->get()->serverName ();
			incS
				<<"src/"<<pName<<"/gen"<<std::endl
				<<"src/"<<pName<<"/userLogic"<<std::endl;
			cppS<<"src/"<<pName<<"/gen/*.cpp"<<" "<<"src/"<<pName<<"/proc/*.cpp"<<" "<<"src/"<<pName<<"/userLogic/*.cpp ";
			groupSet  groupS;
			auto serverName = pName;
			auto pServerF = rData.findServer (pName);
			myAssert (pServerF);
			auto& rMap = pServerF->procMsgS ();
			auto pPmp = rGlobalFile.findMsgPmp ("defMsg");
			myAssert (pPmp);
			for (auto ite = rMap.begin(); rMap.end() != ite; ++ite) {
				auto& rMgr = pPmp->rpcFileS ();
				auto pRpc = rMgr.findRpc (ite->rpcName.c_str ());
				myAssert (pRpc);
				auto pGName = pRpc->groupName ();
				groupS.insert (pGName);
			}
			for (auto ite = groupS.begin(); groupS.end() != ite; ++ite) {
				cppS<<"src/"<<serverName<<"/proc/"<<*ite<<"/*.cpp"<<std::endl;
			}
		}
		os<<R"(set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp src/userLogic/*.cpp )"<<cppS.str()<<R"()
set(defS)
set(libPath)
set(libDep)
if (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB defS src/gen/win/*.def)

	include_directories( )";
	auto depInc = rGlobalFile.depIncludeHome ();
	os<<depInc<<")"<<std::endl;
	auto depLib = rGlobalFile.depLibHome ();

	os<<"list(APPEND libDep "<<depLib<<")"<<std::endl;
	const char* szC2 = R"(endif ()
	include_directories(
	src/gen)";
	auto frameInPath = rGlobalFile.frameInstallPath ();
	// auto szGenFrame  = rModel.frameFunDir ();

	std::string incPath = rGlobalFile.frameIncPath ();
	incPath += "/appFrame";
	os<<szC2<<std::endl
	<<"    ${CMAKE_SOURCE_DIR}/defMsg/src"<<std::endl
	<<"src/userLogic"<<std::endl
	<<incPath<<std::endl
	<<incS.str();
	auto prjName = rGlobalFile.projectName ();
	os<<frameInPath<<"include/"<<prjName<<std::endl;
	
	// os<<"    "<<szGenFrame<<std::endl;
	os<<")"<<std::endl;

	auto libPath = rGlobalFile.frameLibPath ();

	os<<"list(APPEND libPath "<<libPath<<")"<<std::endl
	<<R"(link_directories(${libPath} ${libDep})
	add_library(${prjName} SHARED ${genSrcS} ${defS})
target_link_libraries(${prjName} PUBLIC
	common
	logicCommon
	cLog
)";
	auto bH = rGlobalFile.haveServer ();
	if (bH) {
		os<<R"(defMsg
	)";
	}
os<<R"(
	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	)";
	} while (0);
	return nRet;
	}

