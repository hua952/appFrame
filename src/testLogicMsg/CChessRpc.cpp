#include <myAssert.h>
#include <stdlib.h>
#include "CChessRpc.h"
#include "msgPmpID.h"
#include "CChessMsgID.h"


manualListAsk::manualListAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_manualListAsk);
}
manualListAsk::manualListAsk(packetHead* p):CMsgBase(p)
{
}

manualListRet::manualListRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(manualListRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_manualListRet);
	manualListRetPack* p = ((manualListRetPack*)(N2User(pN)));
	p->m_nodeSNum = 0;
	pN->udwLength = sizeof(manualListRetPack) - sizeof(p->m_nodeS);
}
manualListRet::manualListRet(packetHead* p):CMsgBase(p)
{
}
packetHead* manualListRet::  toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
	manualListRetPack* p = ((manualListRetPack*)(N2User(pN)));
	assert(p->m_nodeSNum < 1024);
	pN->udwLength = sizeof(manualListRetPack) - sizeof(p->m_nodeS[0]) * (1024 - p->m_nodeSNum);
	return m_pPacket;
}
manualListRetPack* manualListRet::	pack()
{
	return ((manualListRetPack*)(P2User(m_pPacket)));
}

getOnceManualAsk::getOnceManualAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_getOnceManualAsk);
}
getOnceManualAsk::getOnceManualAsk(packetHead* p):CMsgBase(p)
{
}

getOnceManualRet::getOnceManualRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(getOnceManualRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_getOnceManualRet);
	getOnceManualRetPack* p = ((getOnceManualRetPack*)(N2User(pN)));
	p->m_trackNum = 0;
	pN->udwLength = sizeof(getOnceManualRetPack) - sizeof(p->m_track);
}
getOnceManualRet::getOnceManualRet(packetHead* p):CMsgBase(p)
{
}
packetHead* getOnceManualRet::  toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
	getOnceManualRetPack* p = ((getOnceManualRetPack*)(N2User(pN)));
	assert(p->m_trackNum < 1024);	pN->udwLength = sizeof(getOnceManualRetPack) - sizeof(p->m_track[0]) * (1024 - p->m_trackNum);
	return m_pPacket;
}
getOnceManualRetPack* getOnceManualRet::	pack()
{
	return ((getOnceManualRetPack*)(P2User(m_pPacket)));
}

enterServerAsk::enterServerAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(enterServerAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_enterServerAsk);
}
enterServerAsk::enterServerAsk(packetHead* p):CMsgBase(p)
{
}
enterServerAskPack* enterServerAsk::	pack()
{
	return ((enterServerAskPack*)(P2User(m_pPacket)));
}

enterServerRet::enterServerRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(enterServerRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_enterServerRet);
}
enterServerRet::enterServerRet(packetHead* p):CMsgBase(p)
{
}
enterServerRetPack* enterServerRet::	pack()
{
	return ((enterServerRetPack*)(P2User(m_pPacket)));
}

wateForGameAsk::wateForGameAsk()
{

	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_wateForGameAsk);
}
wateForGameAsk::wateForGameAsk(packetHead* p):CMsgBase(p)
{
}

wateForGameRet::wateForGameRet()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(wateForGameRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_wateForGameRet);
}
wateForGameRet::wateForGameRet(packetHead* p):CMsgBase(p)
{
}
wateForGameRetPack* wateForGameRet::	pack()
{
	return ((wateForGameRetPack*)(P2User(m_pPacket)));
}

gameEndAsk::gameEndAsk()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(gameEndAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_gameEndAsk);
}
gameEndAsk::gameEndAsk(packetHead* p):CMsgBase(p)
{
}
gameEndAskPack* gameEndAsk::	pack()
{
	return ((gameEndAskPack*)(P2User(m_pPacket)));
}

giveUpAsk::giveUpAsk()
{

	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_giveUpAsk);
}
giveUpAsk::giveUpAsk(packetHead* p):CMsgBase(p)
{
}

giveUpRet::giveUpRet()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(giveUpRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_giveUpRet);
}
giveUpRet::giveUpRet(packetHead* p):CMsgBase(p)
{
}
giveUpRetPack* giveUpRet::	pack()
{
	return ((giveUpRetPack*)(P2User(m_pPacket)));
}

regretAsk::regretAsk()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(regretAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_regretAsk);
}
regretAsk::regretAsk(packetHead* p):CMsgBase(p)
{
}
regretAskPack* regretAsk::	pack()
{
	return ((regretAskPack*)(P2User(m_pPacket)));
}

regretRet::regretRet()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(regretRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_regretRet);
}
regretRet::regretRet(packetHead* p):CMsgBase(p)
{
}
regretRetPack* regretRet::	pack()
{
	return ((regretRetPack*)(P2User(m_pPacket)));
}

acceptRegretAsk::acceptRegretAsk()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(acceptRegretAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_acceptRegretAsk);
}
acceptRegretAsk::acceptRegretAsk(packetHead* p):CMsgBase(p)
{
}
acceptRegretAskPack* acceptRegretAsk::	pack()
{
	return ((acceptRegretAskPack*)(P2User(m_pPacket)));
}

acceptRegretRet::acceptRegretRet()
{

	m_pPacket = (packetHead*)allocPacket(sizeof(acceptRegretRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_acceptRegretRet);
}
acceptRegretRet::acceptRegretRet(packetHead* p):CMsgBase(p)
{
}
acceptRegretRetPack* acceptRegretRet::	pack()
{
	return ((acceptRegretRetPack*)(P2User(m_pPacket)));
}

ntfReqRegretAsk::ntfReqRegretAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_ntfReqRegretAsk);
}
ntfReqRegretAsk::ntfReqRegretAsk(packetHead* p):CMsgBase(p)
{
}

ntfRefuseRegretAsk::ntfRefuseRegretAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_ntfRefuseRegretAsk);
}
ntfRefuseRegretAsk::ntfRefuseRegretAsk(packetHead* p):CMsgBase(p)
{
}

ntfRegretAsk::ntfRegretAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_ntfRegretAsk);
}
ntfRegretAsk::ntfRegretAsk(packetHead* p):CMsgBase(p)
{
}

peaceAsk::peaceAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_peaceAsk);
}
peaceAsk::peaceAsk(packetHead* p):CMsgBase(p)
{
}

peaceRet::peaceRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(peaceRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_peaceRet);
}

peaceRet::peaceRet(packetHead* p):CMsgBase(p)
{
}

peaceRetPack* peaceRet::pack()
{
	return ((peaceRetPack*)(P2User(m_pPacket)));
}

acceptPeaceAsk::acceptPeaceAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(acceptPeaceAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_acceptPeaceAsk);
}

acceptPeaceAsk::acceptPeaceAsk(packetHead* p):CMsgBase(p)
{
}

acceptPeaceAskPack* acceptPeaceAsk::	pack()
{
	return ((acceptPeaceAskPack*)(P2User(m_pPacket)));
}

acceptPeaceRet::acceptPeaceRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(acceptPeaceRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_acceptPeaceRet);
}

acceptPeaceRet::acceptPeaceRet(packetHead* p):CMsgBase(p)
{
}

acceptPeaceRetPack* acceptPeaceRet::	pack()
{
	return ((acceptPeaceRetPack*)(P2User(m_pPacket)));
}

ntfReqPeaceAsk::ntfReqPeaceAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_ntfReqPeaceAsk);
}
ntfReqPeaceAsk::ntfReqPeaceAsk(packetHead* p):CMsgBase(p)
{
}

ntfRefusePeaceAsk::ntfRefusePeaceAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_ntfRefusePeaceAsk);
}
ntfRefusePeaceAsk::ntfRefusePeaceAsk(packetHead* p):CMsgBase(p)
{
}

readyAsk::readyAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_readyAsk);
}
readyAsk::readyAsk(packetHead* p):CMsgBase(p)
{
}

readyRet::readyRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(readyRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_readyRet);
}
readyRet::readyRet(packetHead* p):CMsgBase(p)
{
}
readyRetPack* readyRet::	pack()
{
	return ((readyRetPack*)(P2User(m_pPacket)));
}

moveAsk::moveAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(moveAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_moveAsk);
}
moveAsk::moveAsk(packetHead* p):CMsgBase(p)
{
}
moveAskPack* moveAsk::	pack()
{
	return ((moveAskPack*)(P2User(m_pPacket)));
}

moveRet::moveRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(moveRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_moveRet);
}
moveRet::moveRet(packetHead* p):CMsgBase(p)
{
}
moveRetPack* moveRet::	pack()
{
	return ((moveRetPack*)(P2User(m_pPacket)));
}

moveNtfAsk::moveNtfAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(moveNtfAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_moveNtfAsk);
}
moveNtfAsk::moveNtfAsk(packetHead* p):CMsgBase(p)
{
}
moveNtfAskPack* moveNtfAsk::	pack()
{
	return ((moveNtfAskPack*)(P2User(m_pPacket)));
}

createGameAsk::createGameAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(createGameAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_createGameAsk);
}
createGameAsk::createGameAsk(packetHead* p):CMsgBase(p)
{
}
createGameAskPack* createGameAsk::	pack()
{
	return ((createGameAskPack*)(P2User(m_pPacket)));
}

watchPlayerAsk::watchPlayerAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(watchPlayerAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_watchPlayerAsk);
}
watchPlayerAsk::watchPlayerAsk(packetHead* p):CMsgBase(p)
{
}
watchPlayerAskPack* watchPlayerAsk::	pack()
{
	return ((watchPlayerAskPack*)(P2User(m_pPacket)));
}

watchPlayerRet::watchPlayerRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(watchPlayerRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_watchPlayerRet);
}
watchPlayerRet::watchPlayerRet(packetHead* p):CMsgBase(p)
{
}
watchPlayerRetPack* watchPlayerRet::	pack()
{
	return ((watchPlayerRetPack*)(P2User(m_pPacket)));
}

playingGameInfoAsk::playingGameInfoAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_playingGameInfoAsk);
}
playingGameInfoAsk::playingGameInfoAsk(packetHead* p):CMsgBase(p)
{
}

playingGameInfoRet::playingGameInfoRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(playingGameInfoRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_playingGameInfoRet);
	playingGameInfoRetPack* p = ((playingGameInfoRetPack*)(N2User(pN)));
	p->m_memberNum = 0;
	pN->udwLength = sizeof(playingGameInfoRetPack) - sizeof(p->m_member);
}
playingGameInfoRet::playingGameInfoRet(packetHead* p):CMsgBase(p)
{
}
packetHead* playingGameInfoRet::  toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
	playingGameInfoRetPack* p = ((playingGameInfoRetPack*)(N2User(pN)));
	assert(p->m_memberNum < 256);	pN->udwLength = sizeof(playingGameInfoRetPack) - sizeof(p->m_member[0]) * (256 - p->m_memberNum);
	return m_pPacket;
}
playingGameInfoRetPack* playingGameInfoRet::	pack()
{
	return ((playingGameInfoRetPack*)(P2User(m_pPacket)));
}

playerEnterAsk::playerEnterAsk()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(playerEnterAskPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_playerEnterAsk);
}
playerEnterAsk::playerEnterAsk(packetHead* p):CMsgBase(p)
{
}
playerEnterAskPack* playerEnterAsk::	pack()
{
	return ((playerEnterAskPack*)(P2User(m_pPacket)));
}

roomDisAsk::roomDisAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_roomDisAsk);
}
roomDisAsk::roomDisAsk(packetHead* p):CMsgBase(p)
{
}

manSAsk::manSAsk()
{
	m_pPacket = (packetHead*)allocPacket(0);
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_manSAsk);
}
manSAsk::manSAsk(packetHead* p):CMsgBase(p)
{
}

manSRet::manSRet()
{
	m_pPacket = (packetHead*)allocPacket(sizeof(manSRetPack));
	netPacketHead* pN = P2NHead(m_pPacket);
	pN->uwMsgID = CChess2FullMsg(CChessMsgID_manSRet);
	manSRetPack* p = ((manSRetPack*)(N2User(pN)));
	p->m_trackNum = 0;
	pN->udwLength = sizeof(manSRetPack) - sizeof(p->m_track);
}

manSRet::manSRet(packetHead* p):CMsgBase(p)
{
}

packetHead* manSRet::  toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
	manSRetPack* p = ((manSRetPack*)(N2User(pN)));
	assert(p->m_trackNum < 512);	pN->udwLength = sizeof(manSRetPack) - sizeof(p->m_track[0]) * (512 - p->m_trackNum);
	return m_pPacket;
}

manSRetPack* manSRet::	pack()
{
	return ((manSRetPack*)(P2User(m_pPacket)));
}

static void s_RegRPC(uword uwAsk, uword uwRet, udword askMaxSize, udword retMaxSize)
{
}
static void s_RegMsg(uword uwAsk, udword askMaxSize)
{
}
int CChessMsgInfoReg()
{
	return 0;
}
