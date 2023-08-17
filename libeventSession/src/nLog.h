#ifndef _nLog_h__
#define _nLog_h__
#include <iostream>
// #include <sstream>
#include "mainLoop.h"

// logMsgFT logMsgFun ();

#define NLOG_MSG(nLevel,...) {std::cout<<__VA_ARGS__<<std::endl;}
#define nTrace(...) NLOG_MSG(0, __VA_ARGS__)
#define nDebug(...) NLOG_MSG(1, __VA_ARGS__)
#define nInfo(...) NLOG_MSG(2, __VA_ARGS__)
#define nWarn(...) NLOG_MSG(3, __VA_ARGS__)
#define nError(...) NLOG_MSG(4, __VA_ARGS__)

#endif
