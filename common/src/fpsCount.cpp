#include "fpsCount.h"

fpsCount::fpsCount ()
{
	m_uqwNum = 0;
	m_beginTime = std::chrono::system_clock::now();
	// m_sysBeginTime = std::chrono::system_clock::now();
}

fpsCount::~fpsCount ()
{
}
/*
void fpsCount::sysTimeStr (std::unique_ptr<char[]>& pStr)
{
	auto curT = std::chrono::system_clock::now();
	auto tSub = std::chrono::duration_cast<std::chrono::seconds>(curT -  m_sysBeginTime);
	second2Str (tSub.count(), pStr);
}
*/
double  fpsCount::update (uqword uqwNum)
{
	auto nSub = uqwNum - m_uqwNum;
	double dRet = (double)nSub;
	auto curT = std::chrono::system_clock::now();
	auto tSub = std::chrono::duration_cast<std::chrono::microseconds>(curT - m_beginTime);
	auto dT = double(tSub.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	dRet /= dT;
	m_uqwNum = uqwNum;
	m_beginTime = std::chrono::system_clock::now();
	return dRet;
}
