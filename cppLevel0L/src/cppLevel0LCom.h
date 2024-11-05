#ifndef cppLevel0LCom_h__
#define cppLevel0LCom_h__
#include <sstream>
#include <cLog.h>

#define  LOG_MSG(nLevel,...) {std::stringstream ss;ss<<__VA_ARGS__<<" "<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__;logMsg("appFrame",ss.str().c_str(),nLevel);}
#define rTrace(...) LOG_MSG(0, __VA_ARGS__)
#define rDebug(...) LOG_MSG(1, __VA_ARGS__)
#define rInfo(...) LOG_MSG(2, __VA_ARGS__)
#define rWarn(...) LOG_MSG(3, __VA_ARGS__)
#define rError(...) LOG_MSG(4, __VA_ARGS__)

// #define PUSH_FUN_CALL {std::stringstream ss;ss<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__;lv0PushToCallStack(ss.str().c_str());}
// #define POP_FUN_CALL {lv0PopFromCallStack();}

 #define PUSH_FUN_CALL
 #define POP_FUN_CALL

#endif
