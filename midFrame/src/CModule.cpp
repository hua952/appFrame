#include"CModule.h"
#include <stdlib.h>
#include <string.h>

CModule::CModule()
{
	m_handle = nullptr;
	m_onUnloadFun = nullptr;
}

CModule::~CModule()
{
	unload();
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
