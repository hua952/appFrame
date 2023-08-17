#include <windows.h>
#include "tSingleton.h"
#include "cppLevel0LCom.h"
#include "serverMgr.h"
// #pragma comment(lib, "ws2_32.lib")

int  serverMgr:: initNetServer (const char* szLibname)
{
	int nRet = 0;
	auto szName = szLibname;
	// rTrace ("At Then begin of loadNetServer libName = "<<szName);
	//HINSTANCE hdll;

	do {
		/*
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			rError ("WSAStartup failed with error: "<< err);
			nRet = 1;
		}
		*/
		auto hdll = LoadLibraryA(szName);
		// rTrace (" After LoadLibraryA hdll = "<<hdll);
		if (!hdll) {
			nRet = 2;
			break;
		}
		m_createTcpServerFn = (createTcpServerFT)(GetProcAddress(hdll, "createTcpServer"));
		if (!m_createTcpServerFn) {
			rWarn ("GetProcAddress createTcpServer");
			nRet = 3;
			break;
		}
		m_delTcpServerFn = (delTcpServerFT)(GetProcAddress(hdll, "delTcpServer"));
		if (!m_delTcpServerFn) {
			rWarn ("GetProcAddress delTcpServer");
			nRet = 4;
			break;
		}
		typedef int (*initGlobalFT) (allocPackFT  allocPackFun, freePackFT  freePackFun, logMsgFT logMsgFun);
		auto initFn = (initGlobalFT)(GetProcAddress(hdll, "initGlobal"));
		if (!initFn) {
			rWarn ("GetProcAddress initGlobal");
			nRet = 5;
			break;
		}

		auto& rC = getPhyCallback();
		auto nInit = initFn (rC.fnAllocPack , rC.fnFreePack, rC.fnLogMsg);

		if (0 != nInit) {
			rWarn ("call initFn Error");
			nRet = 6;
			break;
		}
	} while (0);
	return nRet;
}
