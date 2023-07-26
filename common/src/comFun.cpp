#include <memory>
#include <chrono>
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

void second2Str (uqword uqwCur, std::unique_ptr<char[]>& strTime)
{
	auto tSub = uqwCur;
	const auto c_day = 24 * 3600;
	auto nDay = tSub / c_day;
	tSub %= c_day;
	auto nHour = tSub / 3600;
	tSub %= 3600;
	auto nMin = tSub / 60;
	tSub %= 60;
	std::stringstream ss;
	ss<<nDay<<"-"<<nHour<<":"<<nMin<<":"<<tSub;
	strCpy (ss.str().c_str(), strTime);
}
uqword curNanosecond ()
{
	auto curT = std::chrono::system_clock::now();
	auto tSub = std::chrono::duration_cast<std::chrono::nanoseconds>(curT.time_since_epoch());
	// auto dT = double(tSub.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	auto uqwRet = (uqword)(tSub.count());
	return uqwRet;
}
