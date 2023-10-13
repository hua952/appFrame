#include "libeventSessionCom.h"
#include "libeventConnector.h"
#include "myAssert.h"
#include "comFun.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "libeventServerCom.h"
#include "nLog.h"
//#include "framMsgId.h"

libeventSessionCom::libeventSessionCom()
{
	pack = nullptr;
	curIndexRead = 0;
	bev = nullptr;
	m_serverCom = nullptr;
	m_state = SessionState_Unknow;

	m_writeH.pPer = &m_writeH;
	m_writeH.pNext = &m_writeH;
}

libeventSessionCom::~libeventSessionCom()
{
	cleanWritePack ();
	myAssert (nullptr == bev);
}

void libeventSessionCom::cleanWritePack ()
{
	auto pH = &m_writeH;
	auto pCur = pH->pNext;

	auto freeFun = serverCom()->freePackFun ();
	while (pCur != pH) {
		auto pD = pCur;
		pCur= pCur->pNext;
		freeFun (pD);
	}
}

SessionState libeventSessionCom::state()
{
	return m_state;
}

void  libeventSessionCom::setState (SessionState s)
{
	m_state = s;
}

void*	libeventSessionCom:: userData ()
{
	return m_pUserData.get ();
}

void	libeventSessionCom::setUserData(void* pData, int len)
{
	if (nullptr == pData || 0 == len) {
		m_pUserData.reset ();
	} else {
		m_pUserData = std::make_unique<char []> (len);
		memcpy (m_pUserData.get(), pData, len);
	}
}

struct bufferevent*  libeventSessionCom::getBev ()
{
	return bev;
}

void  libeventSessionCom::setBev (struct bufferevent* bev)
{
	this->bev = bev;
}

ubyte*  libeventSessionCom:: getBuff ()
{
	ubyte* pRet = nullptr;
	if (nullptr == pack) {
		myAssert (curIndexRead < sizeof (netPacketHead));
		pRet = (ubyte*)&netHead;
		pRet += curIndexRead;
		//nTrace (__FUNCTION__<<" 000 pRet = "<<pRet);
	} else {
		auto pN = P2NHead (pack);
		myAssert (curIndexRead < pN->udwLength);
		pRet = (ubyte*)pN;
		pRet += sizeof (netPacketHead);
		pRet += curIndexRead;
		//nTrace (__FUNCTION__<<" 111 pRet = "<<pRet);
	}
	return pRet;
}

udword   libeventSessionCom::getNeet ()
{
	udword nRet = 0;
	if (nullptr == pack) {
		if (curIndexRead < sizeof (netPacketHead)) {
			nRet = sizeof (netPacketHead) - curIndexRead;
			//nTrace (__FUNCTION__<<" 0000 nRet = "<<nRet);
		}
	} else {
		auto pN = P2NHead (pack);
		if (curIndexRead < pN->udwLength ) {
			nRet = pN->udwLength - curIndexRead;
			//nTrace (__FUNCTION__<<" 1111 nRet = "<<nRet);
		}
	}
	return nRet;
}

packetHead*  libeventSessionCom:: afterReadBuff(udword nS)
{
	packetHead* pRet = nullptr;
	curIndexRead += nS;
	if (nullptr == pack) {
		myAssert (curIndexRead <= sizeof (netPacketHead));	
		if (curIndexRead == sizeof (netPacketHead)) {
			auto allFun = serverCom()->allocPackFun ();
			pack = allFun(netHead.udwLength);
			auto pN = P2NHead(pack);
			*pN = netHead;
			//nTrace (__FUNCTION__<<" read head msgId = "<<pN->uwMsgID<<" length = "<<pN->udwLength);
			if (0 == netHead.udwLength) {
				pRet = pack;
				pack = nullptr;
				//nTrace (__FUNCTION__<<" 000 msgId = "<<pN->uwMsgID);
			}
			curIndexRead = 0;
		}
	} else {
		auto pN = P2NHead (pack);
		myAssert (curIndexRead <= pN->udwLength);	
		if (curIndexRead == pN->udwLength ) {
			pRet = pack;
			pack = nullptr;
			curIndexRead = 0;
			/*
			nTrace (__FUNCTION__<<" read pack msgId = "<<pN->uwMsgID<<" length = "<<pN->udwLength
					<<" pack = "<<pRet);
					*/
			/*
			auto pN = P2NHead (pRet);
			if (pN->uwMsgID == toFramMsgId (enFramMsgId_regMyHandleRet)) {
				auto pU = (regMyHandleRetPack*)(N2User(pN));
				nTrace ("rec result = "<<pU->result<<"  myHandle = "<<(int)pU->myHandle
						<<" length = "<<pN->udwLength<< "pU = "<<pU );
			}
			*/
		}
	}
	return pRet;
}

void libeventSessionCom::trySend ()
{
	auto stateS = state ();
	auto bevL = getBev ();
	nTrace (__FUNCTION__<<" state = "<<stateS);
	if (SessionState_Online == stateS && bevL) {
		auto freeFun = serverCom()->freePackFun ();
		auto pH = &m_writeH;
		auto pCur = pH->pNext;
		while (pCur != pH) {
			auto d = pCur;
			auto pN = P2NHead(d);
			auto nS = sizeof(netPacketHead) + pN->udwLength;
			auto nRet = bufferevent_write(bevL, pN, nS);
			if (0 == nRet) {
				nTrace (__FUNCTION__<<" msgId = "<<pN->uwMsgID<<" length = "<<pN->udwLength<<" nS = "<<nS);
				
				pCur = pCur->pNext;
				d->pPer->pNext = pCur;
				pCur->pPer = d->pPer;
				auto pServer = serverCom ();
				auto pWF = pServer->onWritePackFun ();
				if (pWF) {
					pWF (this, d);
				}
				//freeFun (d);
			} else {
				nWarn (" bufferevent_write fail ");
				break;
			}
		}
	}
}

ITcpServer* libeventSessionCom::getServer ()
{
	return m_serverCom;
}

libeventServerCom*   libeventSessionCom:: serverCom ()
{
    return m_serverCom;
}

void libeventSessionCom:: setServerCom (libeventServerCom*  va)
{
    m_serverCom = va;
}

int 	libeventSessionCom:: send(packetHead* pack)
{
	int nRet = 0;
	auto pH = &m_writeH;
	pack->pPer = pH->pPer;
	pH->pPer->pNext = pack;
	pack->pNext = pH;
	pH->pPer = pack;
	trySend ();
	return nRet;
}

SessionIDType libeventSessionCom:: id ()
{
    return m_id;
}

void  libeventSessionCom::setId (SessionIDType va)
{
    m_id = va;
}
