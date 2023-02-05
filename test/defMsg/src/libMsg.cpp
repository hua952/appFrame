#include<stdlib.h>
#include"funLog.h"
int initMsg()
{
LOG_FUN_CALL
	int nRet = 0;
	int CChessMsgInfoReg();
	nRet = CChessMsgInfoReg();
	if(0 != nRet)
	{
		return	1;
	}
	return 0;
}
