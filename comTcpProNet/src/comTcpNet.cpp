#include "comTcpNet.h"
#include "strFun.h"
#include "modelLoder.h"
#include "loop.h"

static	void*  s_hDll = nullptr;
static delTcpServerFT  s_delTcpServerFun = nullptr;
static createTcpServerFT s_creTcpServerFun = nullptr;
comTcpNet:: comTcpNet ()
{
	m_tcpServer = nullptr;
	/*
	m_hDll = nullptr;
	m_delTcpServerFun = nullptr;
	*/
}

comTcpNet:: ~comTcpNet ()
{
	cleanNet ();
}

static int sProcessNetPackFun(ISession* session, packetHead* pack)
{
	int nRet = procPacketFunRetType_del;
	do {
		auto pITcp = session->getServer ();
		auto pU = (comTcpNet*)(*(PVoid*)(pITcp->userData()));
		nRet = pU->processNetPackFun (session, pack);
	} while (0);
	return nRet;
}

static void sOnAcceptSession(ISession* session, uqword userData)
{
	auto pKV = (comTcpNet::usrDataType*)userData;
	auto pNet = pKV->first;
	pNet->onAcceptSession (session, pKV->second);
}

static void sOnConnect(ISession* session, uqword userData)
{
	auto pKV = (comTcpNet::usrDataType*)userData;
	auto pNet = pKV->first;
	pNet->onConnect (session, pKV->second);
}

static void sOnClose(ISession* session)
{
	auto pITcp = session->getServer ();
	auto pU = (comTcpNet*)(*(PVoid*)(pITcp->userData()));
	pU->onClose (session);
}

static void sOnWritePack(ISession* session, packetHead* pack)
{
	auto pITcp = session->getServer ();
	auto pU = (comTcpNet*)(*(PVoid*)(pITcp->userData()));
	pU->onWritePack (session, pack);
}

int   comTcpNet:: initNet (endPoint* pLister,
		udword listerNum, endPoint* pConnector, udword conNum,
		sigInfo* pInfo, udword sigNum)
{
    int   nRet = 0;
    do {
		auto creF = s_creTcpServerFun;
		if (!creF) {
			nRet = 1;
			break;
		}
		callbackS cb;
		cb.procPackfun = sProcessNetPackFun;
		cb.acceptFun = sOnAcceptSession;
		cb.connectFun = sOnConnect;
		cb.closeFun = sOnClose;
		cb.onWritePackFun = sOnWritePack;
		endPoint* pEDS = nullptr;
		auto endPointNum = listerNum + conNum;

		if (!endPointNum) {
			nRet = 2;
			break;
		}
		auto endPS = std::make_unique<endPoint[]>(endPointNum);
		pEDS = endPS.get();
		m_usrDataS = std::make_unique<usrDataType[]>(endPointNum);
		for (decltype (listerNum) i = 0; i < listerNum; i++) {
			auto&re = m_usrDataS[i];
			auto&le = pLister[i];
			re.second = le.userData;
			endPS[i] = le;
		}
		for (decltype (conNum) i = 0; i < conNum; i++) {
			auto&re = m_usrDataS[i + listerNum];
			auto&co = pConnector[i];
			re.second = co.userData;
			endPS[i + listerNum] = co;
		}
		for (decltype (endPointNum) i = 0; i < endPointNum; i++) {
			m_usrDataS[i].first = this;
			endPS[i].userData = (uqword)(&m_usrDataS[i]);
		}

		auto pNet = creF(&cb, pEDS, listerNum, pEDS + listerNum,
			conNum, pInfo, sigNum);
		if (!pNet) {
			nRet = 4;
			break;
		}
		auto pt = this;
		pNet->setUserData (&pt, sizeof(pt));
		m_tcpServer = pNet;
    } while (0);
    return nRet;
}

int comTcpNet::processNetPackFun(ISession* session, packetHead* pack)
{
	int nRet = 0;
	do {
		
	} while (0);
	return nRet;
}

void comTcpNet::onAcceptSession(ISession* session, uqword userData)
{
}
void comTcpNet::onConnect(ISession* session, uqword userData)
{
}
void comTcpNet::onClose(ISession* session)
{
}
void comTcpNet::onWritePack(ISession* session, packetHead* pack)
{
}

ITcpServer*  comTcpNet:: tcpServer ()
{
    return m_tcpServer;
}

void  comTcpNet:: setTcpServer (ITcpServer* v)
{
    m_tcpServer = v;
}

void  comTcpNet:: cleanNet ()
{
	auto pDel = s_delTcpServerFun;
	if (pDel && m_tcpServer) {
		pDel (m_tcpServer);
		m_tcpServer = nullptr;
	}
}

int initComTcpNet (const char* szDllFile, allocPackFT  allocPackFun,
		freePackFT	freePackFun, logMsgFT logMsgFun)
{
	int nRet = 0;
	do {
		auto hDll = loadDll (szDllFile);
		if (!hDll) {
			nRet = 1;
			break;
		}
		auto pDelF = (delTcpServerFT)(getFun (hDll, "delTcpServer"));
		if (!pDelF) {
			nRet = 2;
			break;
		}
		auto creF = (createTcpServerFT)(getFun (hDll, "createTcpServer"));
		if (!creF) {
			unloadDll (hDll);
			nRet = 3;
			break;
		}
		s_hDll = hDll;
		s_creTcpServerFun = creF;
		s_delTcpServerFun = pDelF;
	} while (0);
	return nRet;
}

void cleanComTcpNet ()
{
	auto hDll = s_hDll;
	if (hDll) {
		unloadDll (hDll);
		s_hDll = nullptr;
		s_creTcpServerFun = nullptr;
		s_delTcpServerFun = nullptr;
	}
}

ISession*  comTcpNet:: getSession (SessionIDType id)
{
    ISession*  nRet = 0;
    do {
		auto pSer = tcpServer ();
		return pSer->getSession (id);
    } while (0);
    return nRet;
}

