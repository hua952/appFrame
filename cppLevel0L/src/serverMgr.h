#ifndef serverMgr_h__
#define serverMgr_h__
#include "server.h"

typedef server* pserver;
class serverMgr
{
public:
	serverMgr();
	~serverMgr();
	int				initFun (int cArg, const char* argS[]);
	serverIdType	getServerNum();
	pserver*		getServerS();
	server*         getServer(loopHandleType handle);
	PhyCallback&    getPhyCallback();
	loopHandleType	procId();
	loopHandleType	gropId();
	void			setProcId(loopHandleType proc);
	void			setGropId(loopHandleType grop);
    ubyte			upNum ();
    void			setUpNum (ubyte va);
    createTcpServerFT    createTcpServerFn ();
    void  setCreateTcpServerFn (createTcpServerFT va);
    delTcpServerFT    delTcpServerFn ();
    void  setDelTcpServerFn (delTcpServerFT va);
	int   initNetServer (const char* szLibname);
	server*         getOutServer();
    udword    packSendInfoTime ();
    void  setPackSendInfoTime (udword va);
private:
    udword  m_packSendInfoTime;
    delTcpServerFT  m_delTcpServerFn;
    createTcpServerFT  m_createTcpServerFn;
    ubyte  m_outNum;
	int procArgS(int nArgC, const char* argS[]);
	loopHandleType	m_procId;
	loopHandleType	m_gropId;
	std::unique_ptr<pserver[]>	 g_serverS;
	std::unique_ptr<char[]>      m_netLibName;
	uword			g_ServerNum;
	PhyCallback m_PhyCallback;
};
#endif
