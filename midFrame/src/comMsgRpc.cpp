#include "myAssert.h"
#include "comMsgMsgId.h"
#include "comMsgRpc.h"
/*
packetHead* allocPacket(udword udwS);
packetHead* allocPacketExt(udword udwS, udword ExtNum);
addChannelAskMsg:: addChannelAskMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(0, 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_addChannelAsk);
	
}
addChannelAskMsg:: addChannelAskMsg (packetHead* p):CMsgBase(p)
{
}

addChannelRetMsg:: addChannelRetMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (addChannelRet), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_addChannelRet);
	NSetRet(pN);
	
}
addChannelRetMsg:: addChannelRetMsg (packetHead* p):CMsgBase(p)
{
}
addChannelRet* addChannelRetMsg:: pack()
{
    return ((addChannelRet*)(P2User(m_pPacket)));
}

delChannelAskMsg:: delChannelAskMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(0, 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_delChannelAsk);
	
}
delChannelAskMsg:: delChannelAskMsg (packetHead* p):CMsgBase(p)
{
}

delChannelRetMsg:: delChannelRetMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (delChannelRet), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_delChannelRet);
	NSetRet(pN);
	
}
delChannelRetMsg:: delChannelRetMsg (packetHead* p):CMsgBase(p)
{
}
delChannelRet* delChannelRetMsg:: pack()
{
    return ((delChannelRet*)(P2User(m_pPacket)));
}

listenChannelAskMsg:: listenChannelAskMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(0, 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_listenChannelAsk);
	
}
listenChannelAskMsg:: listenChannelAskMsg (packetHead* p):CMsgBase(p)
{
}

listenChannelRetMsg:: listenChannelRetMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (listenChannelRet), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_listenChannelRet);
	NSetRet(pN);
	
}
listenChannelRetMsg:: listenChannelRetMsg (packetHead* p):CMsgBase(p)
{
}
listenChannelRet* listenChannelRetMsg:: pack()
{
    return ((listenChannelRet*)(P2User(m_pPacket)));
}

quitChannelAskMsg:: quitChannelAskMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(0, 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_quitChannelAsk);
	
}
quitChannelAskMsg:: quitChannelAskMsg (packetHead* p):CMsgBase(p)
{
}

quitChannelRetMsg:: quitChannelRetMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (quitChannelRet), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_quitChannelRet);
	NSetRet(pN);
	
}
quitChannelRetMsg:: quitChannelRetMsg (packetHead* p):CMsgBase(p)
{
}
quitChannelRet* quitChannelRetMsg:: pack()
{
    return ((quitChannelRet*)(P2User(m_pPacket)));
}

sendToChannelAskMsg:: sendToChannelAskMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (sendToChannelAsk), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_sendToChannelAsk);
	auto p = ((sendToChannelAsk*)(N2User(pN)));
	p->m_packNum = 0;

}
sendToChannelAskMsg:: sendToChannelAskMsg (packetHead* p):CMsgBase(p)
{
}
sendToChannelAsk* sendToChannelAskMsg:: pack()
{
    return ((sendToChannelAsk*)(P2User(m_pPacket)));
}
packetHead* sendToChannelAskMsg:: toPack()
{
	netPacketHead* pN = P2NHead(m_pPacket);
    sendToChannelAsk* p = ((sendToChannelAsk*)(N2User(pN)));
	myAssert (p->m_packNum <= 1100000);
	pN->udwLength = sizeof(sendToChannelAsk) - sizeof(p->m_pack[0]) * (1100000 - p->m_packNum);
	return m_pPacket;
}

sendToChannelRetMsg:: sendToChannelRetMsg ()
{
	m_pPacket = (packetHead*)allocPacketExt(sizeof (sendToChannelRet), 1);
	netPacketHead* pN = P2NHead(m_pPacket);
	NSetAddCh(pN);
	pN->uwMsgID = comMsg2FullMsg(comMsgMsgId_sendToChannelRet);
	NSetRet(pN);
	
}
sendToChannelRetMsg:: sendToChannelRetMsg (packetHead* p):CMsgBase(p)
{
}
sendToChannelRet* sendToChannelRetMsg:: pack()
{
    return ((sendToChannelRet*)(P2User(m_pPacket)));
}
*/
