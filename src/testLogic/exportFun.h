#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(const ForLogicFun* pForLogic);
	//int   getServerS (const char** pBuff, int nBuffNum);
	void  beforeUnload();
}
#endif
