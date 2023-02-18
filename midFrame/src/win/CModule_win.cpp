#include <windows.h>
#include <iostream>
#include "tSingleton.h"
#include "CModule.h"
//#include <Windows.h>
#include "myAssert.h"
#include "impMainLoop.h"
#include "mLog.h"

int CModule::load_os (const char* szName, ForLogicFun* pForLogic)
{
	int nRet = 0;
	//mTrace ("At Then begin of CModule::load szName = "<<szName);
	//HINSTANCE hdll;
	auto hdll = LoadLibraryA(szName);
	//mTrace (" After LoadLibraryA hdll = "<<hdll);
	//myAssert(hdll);
	do {
		if(!hdll){
			mWarn (" LoadLibraryA Error szName = "<<szName);
			nRet = 1;
			break;
		}
		
		auto& rloopMgr = tSingleton<loopMgr>::single();
		//auto funOnLoad = (afterLoadFunT)(dlsym(m_handle, "afterLoad"));
		auto funOnLoad = (afterLoadFunT)(GetProcAddress(hdll, "afterLoad"));
		//myAssert(funOnLoad);
		//mTrace (" After GetProcAddress afterLoadh funOnLoad = "<<funOnLoad);
		if(!funOnLoad)
		{
			mWarn ("funOnLoad empty error is");
			nRet = 3;
			break;
		}
		//m_onUnloadFun = (beforeUnloadFunT)(dlsym(m_handle, "beforeUnload"));
		m_onUnloadFun = (beforeUnloadFunT)(GetProcAddress(hdll, "beforeUnload"));
		//mTrace (" After GetProcAddress beforeUnload funOnLoad = "<<m_onUnloadFun);
		m_handle = hdll;
		//mTrace (" before call funOnLoad");
		try {
			funOnLoad(pForLogic);
		} catch (...) {
			mTrace ("catch error");
		}
		//mTrace (" after call funOnLoad");
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

