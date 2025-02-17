#include "taskGateConTh.h"

int taskGateConTh::onWorkerInitGen(ForLogicFun* pForLogic)
{

	;
	
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	
	return nRet;
}

