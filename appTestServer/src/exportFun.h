#ifndef exportFun_h__
#define exportFun_h__
#include "mainLoop.h"

extern "C"
{
	void  afterLoad(int nArgC, const char* argS[], ForLogicFun* pForLogic);
	void  beforeUnload();
}

#endif
