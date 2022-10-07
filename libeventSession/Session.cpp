#include"myAssert.h"
#include"Session.h"

static SessionIDType g_nextSessionID = 0;
SessionInitData& getCallBackS()
{
	static SessionInitData	s_data;
	return s_data;
}

int initSessionGlobalData(const SessionInitData& initData)
{
	int nRet = 0;
	auto& rData = getCallBackS();
	rData.fnAllocPack = initData.fnAllocPack;
	rData.fnFreePack = initData.fnFreePack;
	return nRet;
}

Session::Session()
{
    m_ID = EmptySessionID;
}

Session::~Session()
{
	auto fnFree = getCallBackS().fnFreePack();
	auto pH = &m_Head;
	auto pD = pH->pNext;
	while(pH->pNext != pH)
	{
		auto pD = pH->pNext;
		auto pN = P2NHead (pD);

		pH->pNext = pD->pNext;
		pD->pNext->pPer = pH;
		fnFree (pD);
	}
}

SessionState Session::state()
{
	return m_state;
}

void Session::send(packetHead* pack)
{
	auto pH = &m_Head;
	if(pH == pH->pNext)
	{
		netPacketHead* pN = P2NHead(pack);
		bool bOK = onlyWritePack((ubyte*)pN, pN->dwLength + NetHeadSize);
		if(bOK)
		{
			afterSend(pack);
		}
		else
		{
			pushPackToSendList(pack);
		}
		return;
	}
	pushPackToSendList(pack);
	processSend();
}

/* tryReadBuff的返回值没什么用*/
bool Session:: tryReadBuff()
{
	bool bC = true;
	char* pBuf = NULL;
	udword udwLefS = 0;
	if(NULL == m_pPack)
	{
		myAssert(m_udwCurIndex < NetHeadSize );
		pBuf = (char*)&m_Head[0];
		pBuf += m_udwCurIndex;
		udwLefS = NetHeadSize - m_udwCurIndex;
	}
	else
	{
		netPacketHead* pNet = P2NHead(m_pPack);
		assert(m_udwCurIndex < pNet->dwLength);
		pBuf = (char*)(N2User(pNet));
		pBuf += m_udwCurIndex;
		udwLefS = pNet->dwLength - m_udwCurIndex;
	}
	size_t len = onlyReadPack(pBuf, udwLefS);
	if (0 == len) {
		return false;
	}
	assert(len <= udwLefS);
	if(len == udwLefS)
	{
		if(NULL == m_pPack)
		{
			netPacketHead* pN = (netPacketHead*)&m_Head[0];
			myAssert(pN->dwLength <= pMsg->maxSize);
			if(pN->dwLength > pMsg->maxSize)
			{
				close(); 
				return false;
			}
			auto fnAllocPack = getCallBackS().fnAllocPack();
			m_pPack = fnAllocPack(pN->dwLength);
			netPacketHead* pNH = P2NHead(m_pPack);
			memcpy(pNH, pN, NetHeadSize);
			if(0 == pN->dwLength)
			{
				procOncePack();
			}
		}
		else
		{
			procOncePack();	
		}
		m_udwCurIndex = 0;
	}
	else
	{
		m_udwCurIndex += len;
	}
	return bC;
}

bool Session::procOncePack()
{
	bool bC = true;
	return bC;
}


void Session::onReadBuff()
{
	bool bC = true;
	do
	{
		bC = tryReadBuff();
	}while(bC/* &&  evbuffer_get_length(bufferevent_get_input(m_pBev)) > 0*/);
}
void Session::processSend()
{
	auto pH = &m_Head;
	auto pD = pH->pNext;
	while(pH->pNext != pH)
	{
		auto pD = pH->pNext;
		auto pN = P2NHead (pD);
		bool bOK = onlyWritePack((ubyte*)pN, pN->dwLength + NetHeadSize);
		if(!bOK)
		{
			break;
		}
		pH->pNext = pD->pNext;
		pD->pNext->pPer = pH;
		afterSend(pNode->pack);	
	}
}

void afterSend(packetHead* pack)
{

	auto fnFree = getCallBackS().fnFreePack();
	fnFree(pack);
}

void libeventSession::pushPackToSendList(packetHead* pack)
{
	auto pH = &m_Head;
	pH->pPer->pNext = pack;
	pack->pPer = pH->pPer;
	pack->pNext = pH;
	pH->pPer = pack;
}

int Session::close()
{
}

void*  Session:: userData()
{
	return m_pUserData;
}

void   Session:: setUserData(void* pData)
{
	m_pUserData = pData;
}

SessionIDType Session::id()
{
    return m_ID;
}

void Session::setId(SessionIDType id)
{
    m_ID = id;
}

