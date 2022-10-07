#include"libeventConnector.h"
#include"libeventNet.h"

libeventConnector::libeventConnector()
{
}

libeventConnector::~libeventConnector()
{
}

int  libeventConnector::init(const char* ip, uword port, udword token)
{
	strNCpy (m_ip, sizeof(m_ip), ip);
	m_port = port;
	m_token = token;
}


