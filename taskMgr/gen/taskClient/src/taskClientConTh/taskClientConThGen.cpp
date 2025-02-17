#include "taskClientConTh.h"

int taskClientConTh::onWorkerInitGen(ForLogicFun* pForLogic)
{

	;
	
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	
	return nRet;
}

