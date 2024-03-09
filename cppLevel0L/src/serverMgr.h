#ifndef serverMgr_h__
#define serverMgr_h__
#include "server.h"
#include "impMainLoop.h"

typedef server* pserver;
packetHead* allocPack(udword udwSize);// Thread safety
void	freePack(packetHead* pack);// Thread safety
class serverMgr
{
public:
	serverMgr();
	~serverMgr();
	int				initFun (int cArg, char** argS);
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
	// int   initNetServer (const char* szLibname);
	server*         getOutServer();
    udword    packSendInfoTime ();
    void  setPackSendInfoTime (udword va);
    udword    delSendPackTime ();
    void  setDelSendPackTime (udword va);

	int pushPackToLoop (loopHandleType pThis, packetHead* pack);// Thread safety
	loopMgr&  loopS ();
private:
	loopMgr  m_loopS;
    udword  m_delSendPackTime;
    udword  m_packSendInfoTime;
    delTcpServerFT  m_delTcpServerFn;
    createTcpServerFT  m_createTcpServerFn;
    ubyte  m_outNum;
	int procArgS(int nArgC, char** argS);
	loopHandleType	m_procId;
	loopHandleType	m_gropId;
	std::unique_ptr<pserver[]>	 g_serverS;
	std::unique_ptr<server[]>	m_pServerImpS;
	std::unique_ptr<char[]>      m_netLibName;
	uword			g_ServerNum;
	PhyCallback m_PhyCallback;
};

packetHead* allocPack(udword udwSize);
void	freePack(packetHead* pack);

void         lv0PushToCallStack (const char* szTxt);
void         lv0PopFromCallStack ();
void         lv0LogCallStack (int nL);

#endif
