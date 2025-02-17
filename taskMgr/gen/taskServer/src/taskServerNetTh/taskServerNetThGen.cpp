#include "taskServerNetTh.h"

int taskServerNetTh::onWorkerInitGen(ForLogicFun* pForLogic)
{

	;
	
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	
	return nRet;
}

