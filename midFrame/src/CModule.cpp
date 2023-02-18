#include"CModule.h"
#include <stdlib.h>
#include <string.h>
#include "strFun.h"

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

int CModule::load(ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto pBuff = m_name.get ();
	const int c_BuffNum = 3;
	char* buff[c_BuffNum];
	auto nR = strR(pBuff, '+', buff, c_BuffNum);
	auto szName = buff[0];
	//pForLogic->szServerTxt = buff[1];
	nRet = load_os (szName, pForLogic);
	return nRet;
}

