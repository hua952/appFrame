#include <iostream>
#include "xmlLoad.h"
#include "configSrcGen.h"
#include "itemMgr.h"

int main (int nC, char* argS [])
{
	int nRet = 0;
	
	do {
		auto& rC = std::cout;
		if (nC != 4) {
			rC<<" arg num error  exe  file includeDir libDir"<<std::endl;
			nRet = 1;
			break;
		}
		
		xmlLoad loader;
		auto nR =  loader.load (argS[1]);
		if (nR) {
			nRet = 2;
			break;
		}
		auto& rMgr = itemMgr::mgr ();
		rMgr.setIncludeDir (argS[2]);
		rMgr.setLibDir (argS[3]);
		auto& rConfig = configSrcGen::mgr ();
		nR = rConfig.startGen ();
		if (nR) {
			nRet = 3;
			break;
		}
	} while (0);
	return nRet;
}
