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
	dword afterLoad(int nArgC, char** argS, ForLogicFun* pForLogic);
private:
	ForLogicFun m_ForLogicFun;
};
#endif
