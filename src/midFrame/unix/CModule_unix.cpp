#include <iostream>
#include "CModule.h"
#include <dlfcn.h>
#include "myAssert.h"

int CModule::load(const ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto szName = name();
	m_handle = dlopen(szName, RTLD_NOW | RTLD_LOCAL);
	myAssert(m_handle);
	do
	{
		if(!m_handle)
		{
			nRet = 1;
			break;
		}
		auto funOnLoad = (afterLoadFunT)(dlsym(m_handle, "afterLoad"));
		myAssert(funOnLoad);
		if(!funOnLoad)
		{
			std::cout<<"funOnLoad empty error is"<<dlerror()<<std::endl;
			nRet = 2;
			break;
		}
		m_onUnloadFun = (beforeUnloadFunT)(dlsym(m_handle, "beforeUnload"));
		funOnLoad(pForLogic);
	}while(0);
	return nRet;
}

int CModule::unload()
{
	if(m_handle)
	{
		if(m_onUnloadFun)
		{
			m_onUnloadFun();
			m_onUnloadFun = nullptr;
		}
		dlclose(m_handle);
		m_handle = nullptr;
	}
	return 0;
}

