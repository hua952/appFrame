#include "gateRouteServer.h"
#include "strFun.h"
#include "myAssert.h"

gateRouteServer:: gateRouteServer ()
{
}

gateRouteServer:: ~gateRouteServer ()
{
}

int  gateRouteServer:: processToGatePack(packetHead* pPack)
{
    int  nRet = 0;
    do {
		/*  其它gate route 线程广播给本 线程的    */
		nRet = processGateRecPack(nullptr, pPack);
    } while (0);
    return nRet;
}

int  gateRouteServer:: procProx(packetHead* pPack)
{
    int  nRet = 0;
    do {
		myAssert (0);  /*  出错, 不应该调用到这里   */
    } while (0);
    return nRet;
}


int  gateRouteServer:: processBroadcastPack (packetHead* pPack)
{
	int  nRet = procPacketFunRetType_del;
    do {
		auto pN = P2NHead (pPack);
		auto bIsRet = NIsRet (pN);
		/*    目前gate只接收 ask 包      */
		myAssert(!bIsRet);
		auto pF = findMsg(pN->uwMsgID);
		auto myHandle = id();
		if(!pF) {
			mWarn ("can not find proc function token: "
					<<pN->dwToKen<<" msgId = "<<pN->uwMsgID
					<<" length = "<<pN->udwLength
					<<"myHandle = "<<(int)myHandle);
			break;
		}
		pPacketHead  pRet = nullptr;
		nRet = pF(pPack, pRet, &argP);
		if (pRet) {
			/* 这个包是其它gate route 线程广播给本线程的,不需要回复(只有发起广播的那个 gate route 线程需要回复), 此处做了无用的分配和释放,需要再优化一下 */
			freePack (pRet);
			pRet = nullptr;
		}
    } while (0);
    return nRet;
}

