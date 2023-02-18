#ifndef _exportFun_h__
#define _exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(ForLogicFun* pForLogic);
	void  beforeUnload();
}
#endif
