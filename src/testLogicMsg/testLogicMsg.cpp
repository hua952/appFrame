#include "testLogicMsg.h"

static const ForMsgModuleFunS* s_g_ForMsgModuleFunS = NULL;
int  initTestLogicMsg(const ForMsgModuleFunS* pFunS)
{
	int nRet = 0;
	s_g_ForMsgModuleFunS = pFunS;
	return nRet;
}

const ForMsgModuleFunS*  getForMsgModuleFunS()
{
	return s_g_ForMsgModuleFunS;
}
