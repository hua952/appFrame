#include "taskGateNetTh.h"

int taskGateNetTh::onWorkerInitGen(ForLogicFun* pForLogic)
{

	;
	
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	
	return nRet;
}

