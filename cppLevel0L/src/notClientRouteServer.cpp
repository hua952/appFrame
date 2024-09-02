#include "notClientRouteServer.h"
#include "strFun.h"
#include "myAssert.h"

notClientRouteServer:: notClientRouteServer ()
{
}

notClientRouteServer:: ~notClientRouteServer ()
{
}

int  notClientRouteServer:: procToOtherAppServerPack(packetHead* pPack)
{
	/*    通过本线程从网络接下的Ask包交给其它线程处理后, 对应的ret包再通过本线程从网络发出(gate 和 server 都有可能)          */
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead(pPack);
	auto myHandle = id ();
	do {
		bool bIsRet = NIsRet(pN);
		myAssert (bIsRet && EmptySessionID != pPack->sessionID && myHandle == pPack->loopId);  /*   目前server 和gate 发出的 ask 包只能通过广播发,不会到这里      */

		auto pS = getSession (pPack->sessionID);
		myAssert (pS);
		if (pS) {
			packetHead* pNew = nullptr;
			toNetPack (pN, pNew);
			if (pNew) {
				pS->send(pNew);
			} else {
				pS->send(pPack);
				nRet = procPacketFunRetType_doNotDel;
			}
		} else {
			mError(" ret pack can not find sesssion pack: "<<*pPack);
		}
	} while (0);
	return nRet;
}

