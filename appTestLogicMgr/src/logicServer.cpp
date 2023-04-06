#include <iostream>
#include <string>
#include <vector>
#include "logicServer.h"
#include <cstring>
#include "msg.h"
#include "gLog.h"
#include "myAssert.h"
#include "loopHandleS.h"
#include "strFun.h"
#include "comFun.h"
#include "rpcInfo.h"
#include "modelLoder.h"

logicServer::logicServer()
{
}

logicServer::~logicServer()
{
}

int logicServer::init(const char* name, ServerIDType id)
{
	auto nameL = strlen(name);
	m_name = std::make_unique<char[]>(nameL + 1);
	strcpy(m_name.get(), name);
	m_id = id;
	return 0;
}

ServerIDType logicServer::id()
{
	return m_id;
}

const char*  logicServer::name()
{
	return m_name.get();
}

int logicServer::OnServerFrame()
{
	return 0;
}

void getModelS (int nArgC, const char* argS[], std::vector<std::string>& vModelS)
{
	char* pRetBuf[3];
	for (int i = 1; i < nArgC; i++) {
		auto pa = argS [i];
		auto nL = strlen (pa);
		auto pB = std::make_unique<char[]> (nL + 1);
		auto pBuf = pB.get ();
		strNCpy (pBuf, nL + 1, pa);
		auto nNum = strR(pBuf, '=', pRetBuf, 3);
		if (2 != nNum) {
			gWarn("arg num error"<< "2 != nNum = " << nNum);
			continue;
		}
		if (strcmp (pRetBuf[0], "logicModel") == 0) {
			vModelS.push_back (pRetBuf[1]);
		}
	}
}

void logicServerMgr::afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	m_ForLogicFun = *pForLogic;
	auto pForMsg = &m_ForLogicFun;
	auto& rFunS = getForMsgModuleFunS();
	rFunS = *pForLogic;
	m_ForLogicFun = *pForLogic;
	regRpcS (&rFunS);
	std::unique_ptr<char[]> myPath;
	auto nGet = getCurModelPath (myPath);
	std::string strBase = myPath.get();
	std::vector<std::string> vModelS;
	getModelS (nArgC, argS, vModelS);
	do {
		for (auto it = vModelS.begin (); vModelS.end () != it; ++it) {
			std::string strServer = strBase;
			strServer += *it;
			auto hDllServer = loadDll (strServer.c_str());
			if (!hDllServer) {
				gError ("load dll fail fileName = "<<strServer.c_str());
				break;
			}
			auto onLoad = (afterLoadFunT)(getFun(hDllServer, "afterLoad"));
			if (!onLoad) {
				gError ("get fun afterLoad fail");
				break;
			}
			onLoad (nArgC, argS, pForLogic);
			gTrace ("after call onLoad");
		}
	} while (0);
}
