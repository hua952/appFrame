#ifndef _testLogicMsg_h__
#define _testLogicMsg_h__
#include "mainLoop.h"
#include "msg.h"
enum testLogicMsgIdType
{
	enTestLogicAsk1,
	enTestLogicRet1,
	enTestLogicAsk2,
	enTestLogicRet2,
};

#define testLogicMsgIdBegin 0

//int  initTestLogicMsg(const ForMsgModuleFunS* pFunS);
//const ForMsgModuleFunS*  getForMsgModuleFunSPt();
#endif
