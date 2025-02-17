#include "taskClientNetTh.h"

int taskClientNetTh::onWorkerInitGen(ForLogicFun* pForLogic)
{

	;
	
	auto setAttrFun = pForLogic->fnSetAttr;
	auto nRet = onWorkerInit(pForLogic);
	
	return nRet;
}

