#ifndef _libeventConnector_h_
#define _libeventConnector_h_

class libeventConnector
{
public:
	libeventConnector();
	~libeventConnector();
	int   init(const char* ip, uword port, udword token = 0);
private:
	udword            m_token;
	uword             m_port;
	char              m_ip[16];
	libeventSession   m_session;
};

#endif
