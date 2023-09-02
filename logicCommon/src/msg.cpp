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

packetHead*  CMsgBase::toPack()
{
    return m_pPacket;
}

bool CMsgBase::fromPack(packetHead* p)
{
	releasePack(m_pPacket);
	m_pPacket = p;
	return true;
}

packetHead*  CMsgBase::getPack()const
{
	return m_pPacket;
}

packetHead* CMsgBase::pop()
{
	packetHead* p = toPack();
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
	auto pRet =  allocPacket (udwS + 16 * ExtNum);
	if (ExtNum) {
		pNetPacketHead pN = P2NHead(pRet);
		NSetExtPH(pN);
	}
	return pRet;
}

static ForMsgModuleFunS* g_ForMsgModuleFunS = nullptr;
void              setForMsgModuleFunS (ForMsgModuleFunS* pFunS)
{
	g_ForMsgModuleFunS = pFunS;
}

ForMsgModuleFunS& getForMsgModuleFunS()
{
	static ForMsgModuleFunS s_ForMsgModuleFunS;
	return  s_ForMsgModuleFunS;
	//return *g_ForMsgModuleFunS;
}
