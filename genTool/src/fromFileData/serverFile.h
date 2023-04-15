#ifndef _serverFile_h__
#define _serverFile_h__
#include <memory>

class serverFile
{
public:
    serverFile ();
    ~serverFile ();
	const char*  serverName ();
	void  setServerName (const char* v);
private:
	std::unique_ptr <char[]>  m_serverName;
};
#endif
