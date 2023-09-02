#include "tSingleton.h"
#include "mainLoop.h"
#include "impMainLoop.h"
#include "comMsgMsgId.h"
#include "comMsgRpc.h"
#include "mLog.h"
#include "impLoop.h"

/*
static int onAddChannelAsk(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead(pAsk);
	addChannelRetMsg ret;
	auto pRU = ret.pack ();
	pRet = ret.pop();
	auto pRN = P2NHead (pRet);
	pRU->m_result = 0;
	memcpy (pRN + 1, pN + 1, 16);
    addChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);

	auto& rServerMgr = tSingleton<loopMgr>::single ();
	auto pServer = rServerMgr.getLoop (pN->ubyDesServId);
	myAssert (pServer);
	do {
		if (!pServer) {
			pRU->m_result = 1;
			mError (" can not find server pAsk = "<<*pAsk);
			break;
		}
		auto& rChS = pServer->channelS ();
		auto pCh = (impLoop::channelKey*)(pN + 1);
		impLoop::channelValue sidS;
		auto inRet = rChS.insert (std::make_pair(*pCh, sidS));
		if (!inRet.second) {
			mError(" insert channel error ch = "<<std::hex
					<<pCh->key<<" "<<pCh->value<<std::dec<<*pAsk);
		}
	} while (0);
	ask.pop ();
	return nRet;
}

static int onAddChannelRet(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
	auto pN = P2NHead (pAsk);
	auto pRN = P2NHead (pRet);
    addChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	addChannelRetMsg ret (nullptr);
	ret.fromPack(pRet);
	auto pRU = ret.pack ();
	ret.pop ();
	ask.pop ();
	return nRet;
}

static int onDelChannelAsk(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    delChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	delChannelRetMsg ret;

	pRet = ret.pop();
	
	ask.pop ();
	return nRet;
}

static int onDelChannelRet(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    delChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	delChannelRetMsg ret (nullptr);
	ret.fromPack(pRet);
	ret.pop ();

	ask.pop ();
	return nRet;
}

static int onListenChannelAsk(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    listenChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	listenChannelRetMsg ret;

	pRet = ret.pop();
	
	ask.pop ();
	return nRet;
}

static int onListenChannelRet(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    listenChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	listenChannelRetMsg ret (nullptr);
	ret.fromPack(pRet);
	ret.pop ();

	ask.pop ();
	return nRet;
}

static int onQuitChannelAsk(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    quitChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	quitChannelRetMsg ret;

	pRet = ret.pop();
		
	ask.pop ();
	return nRet;
}

static int onQuitChannelRet(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    quitChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	quitChannelRetMsg ret (nullptr);
	ret.fromPack(pRet);
	ret.pop ();

	ask.pop ();
	return nRet;
}

static int onSendToChannelAsk(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    sendToChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	sendToChannelRetMsg ret;

	pRet = ret.pop();
		
	ask.pop ();
	return nRet;
}

static int onSendToChannelRet(packetHead* pAsk, pPacketHead& pRet, procPacketArg* pArg)
{
	int nRet = procPacketFunRetType_del;
    sendToChannelAskMsg ask (nullptr);
    ask.fromPack(pAsk);
	sendToChannelRetMsg ret (nullptr);
	ret.fromPack(pRet);
	ret.pop ();

	ask.pop ();
	return nRet;
}
*/
int regSysProcPacketFun (regMsgFT fnRegMsg, serverIdType handle)
{
	int nRet = 0;
	/*
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_addChannelAsk), onAddChannelAsk);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_addChannelRet), onAddChannelRet);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_delChannelAsk), onDelChannelAsk);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_delChannelRet), onDelChannelRet);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_listenChannelAsk), onListenChannelAsk);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_listenChannelRet), onListenChannelRet);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_quitChannelAsk), onQuitChannelAsk);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_quitChannelRet), onQuitChannelRet);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_sendToChannelAsk), onSendToChannelAsk);
    fnRegMsg (handle, comMsg2FullMsg(comMsgMsgId_sendToChannelRet), onSendToChannelRet);
	*/
    return nRet;
}
