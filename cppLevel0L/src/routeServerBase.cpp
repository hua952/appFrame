#include "routeServerBase.h"
#include "strFun.h"

routeServerBase:: routeServerBase ()
{
}

routeServerBase:: ~routeServerBase ()
{
}

int routeServerBase::processOncePack(packetHead* pPack)
{
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead(pPack);
	auto myHandle = id ();
	do {
		auto appNetType = rConfig.appNetType ();	
		if (c_emptyLoopHandle == pN->ubyDesServId) {
			/*  本进程的其它线性委托本线程发消息到网关,一般广播    */
			nRet = processToGatePack (pPack);
			break;
		}
		
		loopHandleType desPId;
		loopHandleType desSId;
		fromHandle (pN->ubyDesServId, desPId, desSId);

		loopHandleType srcPId;
		loopHandleType srcSId;
		fromHandle (pN->ubySrcServId, srcPId, srcSId);

		loopHandleType myPId;
		loopHandleType mySId;
		fromHandle (myHandle, myPId, mySId);
		
		if (srcPId != desSId) {
			if (srcPId == myPId) {
				nRet = procToOtherAppServerPack (pPack);
				break;
			} /* else {
				剩下的又分 
				if (desPId == myPId) {
					这种情况以及srcPId == desSId的情况将会在 稍后的 workServer::processOncePack (pPack); 处理 
				} else {
					这种情况不应该在此发生, 这种情况应该在 gate 收到网络包时就转发了 
				}
			}*/
		}
		nRet = workServer::processOncePack (pPack);
	} while (0);
	return nRet;
}

