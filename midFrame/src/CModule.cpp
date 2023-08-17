#include"CModule.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include "strFun.h"
#include "modelLoder.h"
#include "mLog.h"
#include "impMainLoop.h"
#include "tSingleton.h"

CModule::CModule()
{
	m_handle = nullptr;
	m_onUnloadFun = nullptr;
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

int CModule::load(int nArgC, const char* argS[], ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto pBuff = m_name.get ();
	const int c_BuffNum = 3;
	char* buff[c_BuffNum];
	auto nR = strR(pBuff, '+', buff, c_BuffNum);
	auto szName = buff[0];
	auto& rPho =  tSingleton<loopMgr>::single ();
	std::string strPath;
	strPath = szName;
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
		m_onUnloadFun = (beforeUnloadFunT)(getFun(hdll, "beforeUnload"));
		m_handle = hdll;
		try {
			funOnLoad(nArgC, argS, pForLogic);
		} catch (...) {
			mTrace ("catch error");
		}
	} while (0);
	return nRet;

}

