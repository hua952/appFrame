#include <windows.h>
#include <iostream>
#include "tSingleton.h"
#include "CModule.h"
//#include <Windows.h>
#include "myAssert.h"
#include "impMainLoop.h"

int CModule::load(const ForLogicFun* pForLogic)
{
	int nRet = 0;
	auto szName = name();

	//HINSTANCE hdll;
	auto hdll = LoadLibraryA(szName);
	myAssert(hdll);
	do {
		if(!hdll){
			nRet = 1;
			break;
		}
		
		auto& rloopMgr = tSingleton<loopMgr>::single();
		//auto funOnLoad = (afterLoadFunT)(dlsym(m_handle, "afterLoad"));
		auto funOnLoad = (afterLoadFunT)(GetProcAddress(hdll, "afterLoad"));
		myAssert(funOnLoad);
		if(!funOnLoad)
		{
			std::cout<<"funOnLoad empty error is"<<std::endl;
			nRet = 3;
			break;
		}
		//m_onUnloadFun = (beforeUnloadFunT)(dlsym(m_handle, "beforeUnload"));
		m_onUnloadFun = (beforeUnloadFunT)(GetProcAddress(hdll, "beforeUnload"));
		m_handle = hdll;
		funOnLoad(pForLogic);
	}while(0);
	return nRet;
}

int CModule::unload()
{
	if(m_handle) {
		if(m_onUnloadFun) {
			m_onUnloadFun();
			m_onUnloadFun = nullptr;
		}

		FreeLibrary ((HMODULE)m_handle);
		//dlclose(m_handle);
		m_handle = nullptr;
	}
	return 0;
}

