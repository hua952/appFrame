#include<string.h>
#include"libeventNet.h"
#include"libeventSession.h"

libeventSession::libeventSession()
{
	m_pPack = NULL;
	m_udwCurIndex = 0;
	m_pBev = NULL;
}

libeventSession::~libeventSession()
{

	auto fnFree = getCallBackS().fnFreePack();
	fnFree(m_pPack);
	if(NULL != m_pBev)
	{
		bufferevent_free(m_pBev);  		
		m_pBev = NULL;
	}
}

void  libeventSession:: clearBev()
{
	if(m_pBev)
	{
		bufferevent_free(m_pBev); 
		m_pBev = NULL;
	}
}

void libeventSession::close()
{
	clearBev();
	libeventNet::single().onCloseSession(this);
	void delLibeventSession(libeventSession* pS);
	delLibeventSession(this);
}

bool libeventSession:: onlyWritePack(ubyte* pBuff, udword udwSize)
{
	return -1 != bufferevent_write(m_pBev, (char*)pBuff, udwSize);
}

udword onlyReadPack(ubyte* pBuff, udword udwSize)
{
	return bufferevent_read(m_pBev, pBuff, udwSize);
}



bufferevent* libeventSession:: bufEvent()
{
	return m_pBev;
}

void	libeventSession::setBufEvent(bufferevent* pB)
{
	m_pBev = pB;
}

