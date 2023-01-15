#include<string>
#include<fstream>
#include"appFileCreator.h"
#include"strFun.h"
#include "comFun.h"

//#include <unistd.h>
//#include <fcntl.h>

appFileCreator::appFileCreator(const char* szSrcDir, const char* szAppName)
{
	strNCpy(m_szSrcDir, DirSize, szSrcDir);
	strNCpy(m_szAppName, NameSize, szAppName);
}

dword appFileCreator::writeH()
{
	std::string strFile = m_szSrcDir;
	//strFile += m_szAppName;
	strFile += "logicServer.h";
	//strFile += ".h";
	bool ifs = isPathExit (strFile.c_str());
	if (ifs) {
		return 0;
	}
	std::ofstream of(strFile.c_str());
/*
	of<<"#ifndef _"<<m_szAppName<<"_h_"<<std::endl
		<<"#define _"<<m_szAppName<<"_h_"<<std::endl
		<<"#include \"comLogic.h\""<<std::endl
		<<std::endl
		<<"class "<<m_szAppName<<": public comLogic"<<std::endl
		<<"{"<<std::endl
		<<"public:"<<std::endl

		<<"	virtual dword  init(int argc, char* arg[], iNet* pNet, iFrame* pF);"<<std::endl
		<<"	virtual void onSignal(int nSig);"<<std::endl
		<<"	virtual void onExit();"<<std::endl
		<<"};"<<std::endl
		<<std::endl
		<<"#endif"<<std::endl;
		*/
	const char* szCon = R"(#ifndef _logicServer_h_
#define _logicServer_h_
#include <memory>
#include "mainLoop.h"
class logicServerMgr
{
public:
	void  afterLoad(const ForLogicFun* pForLogic);
private:
};
#endif)";
	of<<szCon;
	return 0;
}

dword appFileCreator::writeCpp()
{
	/*
	std::string strFile = m_szSrcDir;
	strFile += m_szAppName;
	strFile += ".cpp";
	bool ifs = isPathExit (strFile.c_str());
	if (ifs) {
		return 0;
	}
	std::ofstream of(strFile.c_str());
	of<<"#include<signal.h>"<<std::endl
		<<"#include \""<<m_szAppName<<".h\""<<std::endl
		<<"#include <unistd.h>"<<std::endl
		<<"#include <fcntl.h>"<<std::endl
		<<"#include\"cTimer.h\""<<std::endl
		<<"#include \"strFun.h\""<<std::endl
		<<"#include\"rlog.h\""<<std::endl
		<<"#include<string.h>"<<std::endl
		<<"#include\"funLog.h\""<<std::endl
		<<"int initMsg();"<<std::endl
		<<"int regMsgPmp();"<<std::endl
		<<"dword "<<m_szAppName<<":: init(int argc, char* arg[], iNet* pNet, iFrame* pF)"<<std::endl
		<<"{"<<std::endl
		<<"LOG_FUN_CALL"<<std::endl
		<<"	dword dwRet = comLogic::init(argc, arg, pNet, pF);"<<std::endl
		<<"	if(0 != dwRet)"<<std::endl
		<<"	{"<<std::endl
		<<"		return 1;"<<std::endl
		<<"	}"<<std::endl
		<<"	dwRet = initMsg();"<<std::endl
		<<"	dwRet = regMsgPmp();"<<std::endl
		<<"	return 0;"<<std::endl
		<<"}"<<std::endl
		<<std::endl
		<<"void "<<m_szAppName<<"::onExit()"<<std::endl
		<<"{"<<std::endl
		<<"	comLogic::onExit();"<<std::endl
		<<"}"<<std::endl
		<<"void "<<m_szAppName<<"::onSignal(int nSig)"<<std::endl
		<<"{"<<std::endl
		<<"}"<<std::endl
		<<std::endl
		<<"extern\"C\""<<std::endl
		<<"{"<<std::endl
		<<"	iLogic* getReLogic();"<<std::endl
		<<"}"<<std::endl
		<<"iLogic* getReLogic()"<<std::endl
		<<"{"<<std::endl
		<<"	static "<<m_szAppName<<"	s_ser;"<<std::endl
		<<"	return &s_ser;"<<std::endl
		<<"}"<<std::endl;
		*/
	std::string strFile = m_szSrcDir;
	strFile += "logicServer.cpp";
	bool ifs = isPathExit (strFile.c_str());
	if (ifs) {
		return 0;
	}
	std::ofstream of(strFile.c_str());
	const char* szCon = R"(#include <iostream>
#include "msgPmpID.h"
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
#include "gen/loopHandleS.h"

static int OnFrameCli(void* pArgS)
{
	return procPacketFunRetType_del;
}

void logicServerMgr::afterLoad(const ForLogicFun* pForLogic)
{
	auto& rFunS = getForMsgModuleFunS();
	rFunS.fnSendPackToLoop = pForLogic->fnSendPackToLoop;
	rFunS.fnAllocPack = pForLogic->fnAllocPack;
	rFunS.fnFreePack = pForLogic->fnFreePack;
	rFunS.fnLogMsg = pForLogic->fnLogMsg;
	gDebug ("In logicServerMgr::afterLoad testLogic");
	auto fnCreateLoop = pForLogic->fnCreateLoop;
	auto fnRegMsg = pForLogic->fnRegMsg;
	auto fnAddComTimer = pForLogic->fnAddComTimer;
	gInfo (" before fnAddComTimer testLogic");
	gInfo (" At then end of afterLoad testLogic");
})";
	of<<szCon;
		return 0;
}
