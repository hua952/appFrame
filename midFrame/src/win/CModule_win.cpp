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
	auto hdll = LoadLibraryA(szName);
	do {
		if(!hdll){
			mWarn (" LoadLibraryA Error szName = "<<szName);
			nRet = 1;
			break;
		}
		
		auto& rloopMgr = tSingleton<loopMgr>::single();
		auto funOnLoad = (afterLoadFunT)(GetProcAddress(hdll, "afterLoad"));
		if(!funOnLoad)
		{
			mWarn ("funOnLoad empty error is");
			nRet = 3;
			break;
		}
		m_onUnloadFun = (beforeUnloadFunT)(GetProcAddress(hdll, "beforeUnload"));
		m_handle = hdll;
		try {
			funOnLoad(pForLogic);
		} catch (...) {
			mTrace ("catch error");
		}
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

