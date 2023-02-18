#ifndef _logicServer_h_
#define _logicServer_h_
#include <memory>
#include "mainLoop.h"

class logicServer
{
public:
	logicServer();
	~logicServer();
	int init(const char* name, ServerIDType id);
	ServerIDType id();
	const char*  name();
	int OnServerFrame();
private:
	std::unique_ptr<char[]>	 m_name;
	ServerIDType  m_id;
};


class logicServerMgr
{
public:
	enum serverIdS
	{
		serverIdS_TestServer,
		serverIdS_Num
	};
	/*
	static const  ServerIDType s_SerId = 0;
	static const  ServerIDType s_CliId = 1;
	static const  ServerIDType s_SerB = 2;
	*/
	void  afterLoad(ForLogicFun* pForLogic);
private:
};
#endif
