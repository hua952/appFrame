#ifndef _cppLevel0L_h__
#define  _cppLevel0L_h__

#include "mainLoop.h"
extern "C"
{
	int initFun (int cArg, char** argS, int cDefArg, char** defArgS);
	void afterAllLoopEndBeforeExitApp ();
	int getServerGroupInfo(uword serverG, ubyte* beginIndex, ubyte* runNum);
}

#endif
