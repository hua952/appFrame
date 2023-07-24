#include <memory>
#include "comFun.h"
#include "mainLoop.h"
#include "depOSFun.h"
#include "strFun.h"

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

int  getDirFromFile (const char* szFile, std::unique_ptr<char[]>& pathBuf)
{
	int nRet = 0;
	do {
		if (!szFile) {
			nRet = 1;
			break;
		}
		int len = (int)(strlen(szFile));
		auto resLen = len;
		bool bFind = false;
		for (int i = len - 1; i > 0; i--) {
			auto bIs = isPathInterval (szFile [i]);
			if (bIs) {
				resLen = i + 1;
				bFind = true;
				break;
			}
		}
		if (!bFind) {
			nRet = 2;
			break;
		}
		pathBuf = std::make_unique <char[]> (resLen);
		strNCpy (pathBuf.get (), resLen, szFile);
	} while (0);
	return nRet;
}
