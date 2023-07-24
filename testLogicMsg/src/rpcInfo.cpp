#include "CChessMsgID.h"
#include "msgPmpID.h"
#include "msg.h"
#include "loopHandleS.h"

void regRpcS (const ForMsgModuleFunS* pForLogic)
{
	auto regRpc = pForLogic->fnRegRpc;
	regRpc (CChess2FullMsg(CChessMsgID_manualListAsk), CChess2FullMsg(CChessMsgID_manualListRet),
		ThreadServerHandle, ThreadClientHandle);
	regRpc (CChess2FullMsg(CChessMsgID_regretAsk), CChess2FullMsg(CChessMsgID_regretRet),
		TestServerHandle, ThreadClientHandle);
	regRpc (CChess2FullMsg(CChessMsgID_moveAsk), CChess2FullMsg(CChessMsgID_moveRet),
		AppTestServerHandle, ThreadClientHandle);
}


