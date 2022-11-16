#include <iostream>
#include "tSingleton.h"
#include "CModule.h"
#include <dlfcn.h>
#include "myAssert.h"
#include "impMainLoop.h"

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
		/*
		auto funGetServerS= (getServerSFunT)(dlsym(m_handle, "getServerS"));
		if(!funGetServerS)
		{
			std::cout<<"funGetServerS empty error is"<<dlerror()<<std::endl;
			nRet = 2;
			break;
		}
		std::unique_ptr<const char* []>	 nameS;
		nameS = std::make_unique<const char* []>(LoopNum);
		auto nSerNum = funGetServerS (nameS.get(), LoopNum);
		*/
		auto& rloopMgr = tSingleton<loopMgr>::single();
		auto funOnLoad = (afterLoadFunT)(dlsym(m_handle, "afterLoad"));
		myAssert(funOnLoad);
		if(!funOnLoad)
		{
			std::cout<<"funOnLoad empty error is"<<dlerror()<<std::endl;
			nRet = 3;
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

