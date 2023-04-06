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

std::ostream& operator << (std::ostream& os, const packetHead& pack)
{
	auto pp = &pack;
	auto& p = *(P2NHead(pp));
    os << " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<p.uwTag<<std::dec<<"pack = "<<pp;
    return os;
}


