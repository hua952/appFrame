#ifndef _IScession_h__
#define _IScession_h__
#include "typeDef.h"

struct IScession
{
	virtual	dword	write(ubyte* pBuff, udword buffLen) = 0;
	virtual	dword	read(ubyte*  pBuff, udword buffLen) = 0;
};
#endif

