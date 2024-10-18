#include "internalRpc.h"
#include "internalMsgGroup.h"
#include "internalMsgId.h"
#include "logicWorkerMgr.h"

sendPackToRemoteAskMsg :: sendPackToRemoteAskMsg ()
{
	auto &smgr = logicWorkerMgr::getMgr ();
	m_pPacket = (packetHead*)smgr.forLogicFun ()->fnAllocPack (0);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddSer(pN);
	pN->uwMsgID = internal2FullMsg(internalMsgId_sendPackToRemoteAsk);
	
}

sendPackToRemoteAskMsg :: sendPackToRemoteAskMsg (packetHead* p):CMsgBase(p)
{
}

sendPackToRemoteRetMsg:: sendPackToRemoteRetMsg()
{
	m_pPacket = (packetHead*)allocPacket(sizeof (sendPackToRemoteRet));
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddSer(pN);
	pN->uwMsgID = internal2FullMsg(internalMsgId_sendPackToRemoteRet);
	NSetRet(pN);
	auto p = ((sendPackToRemoteRet*)(N2User(pN)));
	p->m_result= 0;
	
}

sendPackToRemoteRetMsg:: sendPackToRemoteRetMsg(packetHead* p):CMsgBase(p)
{
}


recRemotePackForYouAskMsg ::recRemotePackForYouAskMsg ()
{
	auto &smgr = logicWorkerMgr::getMgr ();
	m_pPacket = (packetHead*)smgr.forLogicFun ()->fnAllocPack (0);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddSer(pN);
	pN->uwMsgID = internal2FullMsg(internalMsgId_recRemotePackForYouAsk);
}

recRemotePackForYouAskMsg ::recRemotePackForYouAskMsg(packetHead* p):CMsgBase(p)
{
}


regAppRouteAskMsg ::regAppRouteAskMsg()
{
}

regAppRouteAskMsg ::regAppRouteAskMsg (packetHead* p)
{
	auto &smgr = logicWorkerMgr::getMgr ();
	m_pPacket = (packetHead*)smgr.forLogicFun ()->fnAllocPack (sizeof(regAppRouteAsk));
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddSer(pN);
	pN->uwMsgID = internal2FullMsg(internalMsgId_regAppRouteAsk);
}

regAppRouteRetMsg ::regAppRouteRetMsg()
{
}

regAppRouteRetMsg ::regAppRouteRetMsg (packetHead* p)
{
	auto &smgr = logicWorkerMgr::getMgr ();
	m_pPacket = (packetHead*)smgr.forLogicFun ()->fnAllocPack (sizeof(regAppRouteRet));
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddSer(pN);
	pN->uwMsgID = internal2FullMsg(internalMsgId_regAppRouteRet);
}
