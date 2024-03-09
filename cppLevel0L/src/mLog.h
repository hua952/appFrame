#ifndef _mLog_h__
#define _mLog_h__
#include "mainLoop.h"
#include "cppLevel0LCom.h"
/*
logMsgFT getLogMsgFun ();
#include <sstream>
#define  LOG_MSG(nLevel,...) {std::stringstream ss;ss<<__FILE__<<":"<<__LINE__<<"   "<<__FUNCTION__<<__VA_ARGS__;getLogMsgFun()("appFrame",ss.str().c_str(),nLevel);}
#define mTrace(...) LOG_MSG(0, __VA_ARGS__)
#define mDebug(...) LOG_MSG(1, __VA_ARGS__)
#define mInfo(...) LOG_MSG(2, __VA_ARGS__)
#define mWarn(...) LOG_MSG(3, __VA_ARGS__)
#define mError(...) LOG_MSG(4, __VA_ARGS__)
*/
#define mTrace rTrace
#define mDebug rDebug
#define mInfo rInfo
#define mWarn rWarn
#define mError rError
#endif
