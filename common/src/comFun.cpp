#include <memory>
#include "comFun.h"
#include "mainLoop.h"

loopHandleType  toHandle (loopHandleType  p, loopHandleType l)
{
	loopHandleType nRet = p;
	nRet <<= LoopNumBitLen;
	nRet |= l;
	return nRet;
}

bool            fromHandle (loopHandleType handle, loopHandleType &p, loopHandleType& l)
{
	l = (loopHandleType)(handle & LoopMark);
	auto temp = handle;
	temp >>= LoopNumBitLen;
	temp &= ProcMark;
	p = (loopHandleType)(temp);
	return true;
}

