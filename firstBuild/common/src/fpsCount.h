#ifndef fpsCount_h__
#define fpsCount_h__

#include "comFun.h"
#include <ctime>
#include <chrono> 

class fpsCount
{
public:
    fpsCount ();
    ~fpsCount ();
	double  update (uqword uqwNum);
	// void sysTimeStr(std::unique_ptr<char[]>& pStr);
private:
	uqword   m_uqwNum;
	std::chrono::time_point<std::chrono::system_clock>   m_beginTime;
	// std::chrono::time_point<std::chrono::system_clock>   m_sysBeginTime;
};

#endif 
