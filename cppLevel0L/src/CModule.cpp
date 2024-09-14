#include"CModule.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include "strFun.h"
#include "modelLoder.h"
#include "mLog.h"
#include "tSingleton.h"
#include "argConfig.h"
#include "serverMgr.h"

CModule::CModule()
{
	m_handle = nullptr;
	m_onUnloadFun = nullptr;
	m_fnOnLoopBegin = nullptr;
	m_fnOnLoopEnd = nullptr;
	m_fnLogicOnConnect = nullptr;
	m_fnLogicOnAccept = nullptr;
	m_fnAfterLoad = nullptr;
}

CModule::~CModule()
{
	//unload();
}

const char* CModule::name()
{
	return m_name.get();
}

int CModule::init(const char* szName)
{
	auto nL = strlen(szName);
	m_name= std::make_unique<char[]>(nL + 1);
	auto p = m_name.get();
	strcpy(p, szName);
	return 0;
}

int CModule::load(int nArgC, char** argS, ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto pBuff = m_name.get ();
	const int c_BuffNum = 3;
	char* buff[c_BuffNum];
	auto nR = strR(pBuff, '+', buff, c_BuffNum);
	auto szName = buff[0];
	auto& rServerMgr = tSingleton<serverMgr>::single ();
	std::string strPath;
	auto& rConfig = tSingleton<argConfig>::single ();
	auto workDir = rServerMgr.homeDir (); // rConfig.workDir();
	if (workDir) {
		strPath = workDir;
	} else {
		std::unique_ptr<char[]> t;
		getCurModelPath (t);
		strPath = t.get();
	}
	strPath += "/bin/";
	strPath += szName;
	strPath += dllExtName();
	auto hdll = loadDll (strPath.c_str());
	do {
		if (!hdll) {
			mWarn ("loadDll error szName = "<<szName);
			nRet = 1;
			break;
		}
		auto funOnLoad = (afterLoadFunT)(getFun (hdll, "afterLoad"));
		if(!funOnLoad) {
			mWarn ("funOnLoad empty error is");
			nRet = 2;
			break;
		}
		m_fnAfterLoad = funOnLoad;
		m_fnLogicOnConnect = (logicOnConnectFT)(getFun (hdll, "logicOnConnect"));
		if (!m_fnLogicOnConnect) {
			mWarn ("fun logicOnConnect empty error is");
			nRet = 3;
			break;
		}
		m_fnLogicOnAccept = (logicOnConnectFT)(getFun (hdll, "logicOnAccept"));
		if (!m_fnLogicOnAccept) {
			mWarn ("fun logicOnAccept empty error is");
			nRet = 4;
			break;
		}
		m_onUnloadFun = (beforeUnloadFunT)(getFun(hdll, "beforeUnload"));
		if (!m_onUnloadFun) {
			mWarn ("fun beforeUnload empty error is");
			nRet = 5;
			break;
		}
		m_fnOnLoopBegin = (onLoopBeginFT)(getFun(hdll, "onLoopBegin"));
		if (!m_fnOnLoopBegin) {
			mWarn ("fun onLoopBegin empty error is");
			nRet = 6;
			break;
		}
		m_fnOnFrameLogic = (onFrameLagicFT)(getFun(hdll, "onFrameLogic"));
		if (!m_fnOnFrameLogic) {
			mWarn ("fun onLoopBegin empty error is");
			nRet = 6;
			break;
		}
		m_fnOnLoopEnd = (onLoopEndFT)(getFun(hdll, "onLoopEnd"));
		if (!m_fnOnLoopEnd) {
			mWarn ("fun fnOnFrameLogic empty error is");
			nRet = 6;
			break;
		}
		m_handle = hdll;
		try {
			auto nR = funOnLoad(nArgC, argS, pForLogic, 0, nullptr);
			if (nR) {
				mError ("funOnLoad ret error nR = "<<nR);
			}
		} catch (...) {
			mTrace ("catch error");
		}
	} while (0);
	return nRet;

}

onLoopEndFT  CModule:: fnOnLoopEnd ()
{
    return m_fnOnLoopEnd;
}

onLoopBeginFT  CModule:: fnOnLoopBegin ()
{
    return m_fnOnLoopBegin;
}

logicOnAcceptFT  CModule:: fnLogicOnAccept ()
{
    return m_fnLogicOnAccept;
}

logicOnConnectFT  CModule:: fnLogicOnConnect ()
{
    return m_fnLogicOnConnect;
}

afterLoadFunT  CModule:: fnAfterLoad ()
{
    return m_fnAfterLoad;
}

onFrameLagicFT  CModule::fnOnFrameLogic ()
{
	return m_fnOnFrameLogic;
}

