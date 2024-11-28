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
    os<<"pAsk = "<<pack.packArg<<" sessionID = " <<pack.sessionID<<" loopId = "<<(int)(pack.loopId)<< " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pack = "<<pp;
    return os;
}

std::ostream& operator << (std::ostream& os, const netPacketHead& pack)
{
	auto pp = &pack;
	auto& p = pack;
    os << " udwLength = " << p.udwLength<< "  dwToKen = " << p.dwToKen
		<<" ubySrcServId = "<<(int)(p.ubySrcServId) <<" ubyDesServId = "<<(int)(p.ubyDesServId)
		<<" uwMsgID = "<<p.uwMsgID<<" uwTag = "<<std::hex<<(int)(p.uwTag)<<std::dec<<"pN = "<<pp;
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

bool  upDir (char* szPath)
{
	bool bRet = false;
	int len = (int)(strlen (szPath));
	decltype (len) i = len - 1; 
	for (; i >= 0 ; i--) {
		if ('/' != szPath[i] && '\\' != szPath[i]) {
			break;
		}
	}

	for (; i >= 0 ; i--) {
		if ('/' == szPath[i] || '\\' == szPath[i]) {
			szPath[i] = 0;
			bRet = true;
			break;
		}
	}
	return bRet;
}

extern serverIdType  c_levelMaxOpenNum[] = {1, 8, 16, 64};

void getLvevlFromSerId (loopHandleType	serId, loopHandleType& level, loopHandleType& onceLv, loopHandleType& onceIndex)
{
	auto ubySerTmp = (ubyte)(serId);
	level = ubySerTmp/c_onceServerLevelNum;
	auto ubyOnce = ubySerTmp%c_onceServerLevelNum;
	onceLv = ubyOnce/c_levelMaxOpenNum[level];
	onceIndex = ubyOnce%c_levelMaxOpenNum[level];
}

std::string  getDllPath (const char* szDllName)
{
	std::string strRet;
	strRet += dllInstallDir ();
	strRet += "/";
	strRet += dllPerName ();
	strRet += szDllName;
	strRet += dllExtName ();
	return strRet;
}

