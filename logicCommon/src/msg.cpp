#include<myAssert.h>
#include<stdlib.h>
#include<string.h>
#include<sstream>
#include"msg.h"
#include"packet.h"

CMsgBase::~CMsgBase()
{
	//SBD(m_pPacket)
	releasePack(m_pPacket);
}

CMsgBase::CMsgBase():m_pPacket(NULL)
{
}

CMsgBase::CMsgBase(packetHead* p):m_pPacket(p)
{
}
/*
void CMsgBase:: unZip()
{
    do {
		auto pN = P2NHead(m_pPacket);
		if (NIsZip(pN)) {
			fromPack ();
			pN = P2NHead(m_pPacket);
			NSetUnZip(pN);
		}
    } while (0);
}

void  CMsgBase:: zip()
{
	auto pN = P2NHead(m_pPacket);
    do {
		if (!NIsZip(pN)) {
			toPack();
			pN = P2NHead(m_pPacket);
			NSetZip(pN);
		}
    } while (0);
}
int CMsgBase::toPack()
{
    return 0;
}

int CMsgBase::fromPack()
{
	return 0;
}
*/
packetHead*  CMsgBase::getPack()const
{
	return m_pPacket;
}

packetHead* CMsgBase::pop()
{
	packetHead* p = m_pPacket; //toPack();
    m_pPacket = NULL;
	return p;
}

MsgRpcType CAskMsgBase::RpcType()
{
    return eMsgRpc_Ask;
}

CRetMsgBase::CRetMsgBase()
{
}

MsgRpcType CRetMsgBase::RpcType()
{
    return eMsgRpc_Ret;
}

void releasePack(packetHead* p)
{
	if(NULL == p)
	{
		return;
	}
	auto & mgr = getForMsgModuleFunS();
	mgr.fnFreePack(p);
}

packetHead* allocPacket(udword udwS)
{
	auto & mgr = getForMsgModuleFunS();
	packetHead* pRet = (packetHead*)(mgr.fnAllocPack(AllPacketHeadSize + udwS));
	pNetPacketHead pN = P2NHead(pRet);
	memset(pN, 0, NetHeadSize);
	pN->udwLength = udwS;
	pRet->pAsk = 0;
	return pRet;
}

packetHead* allocPacketExt(udword udwS, udword ExtNum)
{
	auto pRet =  allocPacket (udwS + sizeof(packetHead) * ExtNum);
	if (ExtNum) {
		pNetPacketHead pN = P2NHead(pRet);
		NSetExtPH(pN);
	}
	return pRet;
}

packetHead* clonePack(packetHead* p)
{
	auto pN = P2NHead(p);
	udword extNum = NIsExtPH(pN)?1:0;
	auto pRet = allocPacketExt (pN->udwLength, extNum);
	auto pRN = P2NHead(pRet);
	auto udwLength = pN->udwLength;
	*pRN++ = *pN++;
	if (udwLength ) {
		memcpy (pRN, pN, udwLength);
	}
	return pRet;
}

/*
static ForMsgModuleFunS* g_ForMsgModuleFunS = nullptr;
void              setForMsgModuleFunS (ForMsgModuleFunS* pFunS)
{
	g_ForMsgModuleFunS = pFunS;
}
*/
ForMsgModuleFunS* s_pForMsgModuleFunS = nullptr;

void setForMsgModuleFunS(ForMsgModuleFunS* pF)
{
	s_pForMsgModuleFunS = pF;
}

ForMsgModuleFunS& getForMsgModuleFunS()
{
	return *s_pForMsgModuleFunS;
	// static ForMsgModuleFunS s_ForMsgModuleFunS;
	// return  s_ForMsgModuleFunS;
	//return *g_ForMsgModuleFunS;
}
