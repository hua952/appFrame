#include "taskGateWorkerMgr.h"
#include "logicFrameConfig.h"
#include "tSingleton.h"


int taskGateWorkerMgr::initLogicGen (int cArg, char** argS, ForLogicFun* pForLogic, int cDefArg, char** defArgS)
{
	auto& rConfig = tSingleton<logicFrameConfig>::single ();
	int nRet = 0;
	do {
		auto serverGroupNum = rConfig.serverGroupNum ();
		auto serverGroups = rConfig.serverGroups ();
		for (decltype (serverGroupNum) i = 0; i < serverGroupNum; i++) {
			if (i == 0) {
				m_taskGateConTh.first = std::make_unique<taskGateConTh[]>(serverGroups[i].runNum);
				for (decltype (serverGroups[i].runNum) j = 0; j < serverGroups[i].runNum; j++) {
					m_allServers [serverGroups[i].beginId + j] = m_taskGateConTh.first.get() + j;
				}
			} else if (i == 1) {
				m_taskGateNetTh.first = std::make_unique<taskGateNetTh[]>(serverGroups[i].runNum);
				for (decltype (serverGroups[i].runNum) j = 0; j < serverGroups[i].runNum; j++) {
					m_allServers [serverGroups[i].beginId + j] = m_taskGateNetTh.first.get() + j;
				}
			} 
		}
		
		
	} while (0);
	return nRet;
}

