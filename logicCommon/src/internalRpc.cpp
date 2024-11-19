#include "internalRpc.h"
#include "internalMsgGroup.h"
#include "internalMsgId.h"
#include "logicWorkerMgr.h"

#define IMP_MSG_PART1(msgName, msgSize) msgName##Msg::msgName##Msg() {\
	auto &smgr = logicWorkerMgr::getMgr ();\
	m_pPacket = (packetHead*)smgr.forLogicFun ()->fnAllocPack (msgSize);\
	netPacketHead* pN = P2NHead(m_pPacket);\
	NSetAddSer(pN);\
	pN->uwMsgID = internal2FullMsg(internalMsgId_##msgName);

#define IMP_MSG_PART2(msgName) }\
msgName##Msg::msgName##Msg(packetHead* p):CMsgBase(p){\
}

#define IMP_ASK_MSG(msgName) IMP_MSG_PART1(msgName, sizeof(msgName))\
IMP_MSG_PART2(msgName)


#define IMP_RET_MSG(msgName) IMP_MSG_PART1(msgName, sizeof(msgName))\
	NSetRet(pN);\
IMP_MSG_PART2(msgName)

#define IMP_ASK_MSG_Z(msgName) IMP_MSG_PART1(msgName, 0)\
IMP_MSG_PART2(msgName)


#define IMP_RET_MSG_Z(msgName) IMP_MSG_PART1(msgName, 0)\
	NSetRet(pN);\
IMP_MSG_PART2(msgName)

IMP_MSG_PART1(sendPackToRemoteAsk, sizeof(sendPackToRemoteAsk))\
	auto pU = (sendPackToRemoteAsk*)(N2User(pN));
	pU->objSessionId = EmptySessionID;
IMP_MSG_PART2(sendPackToRemoteAsk)
IMP_RET_MSG(sendPackToRemoteRet)

IMP_ASK_MSG_Z(recRemotePackForYouAsk)

IMP_ASK_MSG_Z(heartbeatAsk)
IMP_ASK_MSG_Z(heartbeatRet)

IMP_ASK_MSG(regAppRouteAsk)
IMP_RET_MSG(regAppRouteRet)

IMP_ASK_MSG(createChannelAsk)
IMP_RET_MSG(createChannelRet)

IMP_ASK_MSG(deleteChannelAsk)
IMP_RET_MSG(deleteChannelRet)

IMP_ASK_MSG(subscribeChannelAsk)
IMP_RET_MSG(subscribeChannelRet)

IMP_ASK_MSG(sayToChannelAsk)
IMP_RET_MSG(sayToChannelRet)

IMP_ASK_MSG(leaveChannelAsk)
IMP_RET_MSG(leaveChannelRet)

IMP_ASK_MSG(broadcastPacketNtf)

IMP_ASK_MSG_Z(sendToAllGateAsk)
IMP_RET_MSG(sendToAllGateRet)

IMP_MSG_PART1(exitAppNtfByNet, sizeof(exitAppNtfByNet))\
	auto pU = (exitAppNtfByNet*)(N2User(pN));
	pU->resultNum = 1;
	pU->result[0]= 0;
IMP_MSG_PART2(exitAppNtfByNet)

IMP_ASK_MSG(exitAppNtf)
