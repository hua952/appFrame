#ifndef _cppLevel0L_h__
#define  _cppLevel0L_h__

#include "server.h"
#include "mainLoop.h"
extern "C"
{
	int initFun (int cArg, const char* argS[]);
}

uword getServerNum();
pserver* getServerS();
PhyCallback&  getPhyCallback();

#endif
