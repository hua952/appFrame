#ifndef gLog_h__
#define gLog_h__

#include "cLog.h"
#include <sstream>
#include "msg.h"

logMsgFT getLogMsgFun ();
#define  GLOG_MSG(nLevel,...) {std::stringstream ss;ss<<__VA_ARGS__<<" "<<__FILE__<<":"<<__LINE__<<"   "<<__FUNCTION__;getLogMsgFun()("appFrame",ss.str().c_str(),nLevel);}
// #define GLOG_MSG(nLevel,...) {std::cout<<__VA_ARGS__<<std::endl;}
#define gTrace(...) GLOG_MSG(0, __VA_ARGS__)
#define gDebug(...) GLOG_MSG(1, __VA_ARGS__)
#define gInfo(...) GLOG_MSG(2, __VA_ARGS__)
#define gWarn(...) GLOG_MSG(3, __VA_ARGS__)
#define gError(...) GLOG_MSG(4, __VA_ARGS__)
#endif
