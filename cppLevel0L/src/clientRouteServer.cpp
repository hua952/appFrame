#include "clientRouteServer.h"
#include "strFun.h"

clientRouteServer:: clientRouteServer ()
{
	m_defSession = nullptr;
}

clientRouteServer:: ~clientRouteServer ()
{
}

int  clientRouteServer:: processToGatePack(packetHead* pPack)
{
	return procToOtherAppServerPack (pPack);
}
int  clientRouteServer:: procToOtherAppServerPack (packetHead* pPack)
{
    int  nRet = procPacketFunRetType_doNotDel; /*   该包在处理返回结果时还要用, 不能删   */
    do {
		auto pN = P2NHead (pPack);
		// myAssert(c_emptyLoopHandle != pN->ubyDesServId);

		bool bIsRet = NIsRet (pN);
		myAssert (!bIsRet);

		auto myHandle = id();
		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);

		loopHandleType sPId;
		loopHandleType sSId;
		fromHandle (pN->ubySrcServId, sPId, sSId);

		myAssert (sPId == myPId); /* 该函数只处理首站发出的ask包 */

		ISession* pSe = m_defSession;

		pPack->pAsk = 0;
		if (pSe) {
			pSe->send (pPack);
		} else {
			mError ("procProx pSe null pack = "<<*pPack);
			nRet = procPacketFunRetType_del; /* 无法发送,删除包 */
		}
    } while (0);
    return nRet;
}
