#include "gloableData.h"
#include "strFun.h"

gloableData:: gloableData ()
{

	muServerNumS m_muServerPairS[c_serverLevelNum];
	for (uword i = 0; i < c_serverLevelNum; i++) {
		uword num = c_onceServerLevelNum / c_levelMaxOpenNum[i];
		m_muServerPairS[i] = std::make_unique<loopHandleType[]>(num);
		for (decltype (num) j = 0; j < num; j++) {
			m_muServerPairS[i][i] = 0;
		}
	}
}

gloableData:: ~gloableData ()
{
}

