#ifndef exportFun_h__
#define exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(const ForLogicFun* pForLogic);
	void  beforeUnload();
}

#endif
