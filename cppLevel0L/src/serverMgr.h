#ifndef serverMgr_h__
#define serverMgr_h__
#include "server.h"
#include "msgMgr.h"
#include "CModule.h"

typedef server* pserver;
packetHead* allocPack(udword udwSize);// Thread safety
void	freePack(packetHead* pack);// Thread safety
class impLoop;

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
	// loopMgr&  loopS ();

	impLoop*   getLoop(loopHandleType id);
	impLoop*   getLoopByIndex(uword index);
	int createServer(const char* szName, loopHandleType serId,
	serverNode* pNode, frameFunType funFrame, void* arg);
	int             init(int nArgC, char** argS, PhyCallback& info);
	int   initNetServer ();
	int procArgSMid(int nArgC, char** argS);
	// loopHandleType  procId();
	// loopHandleType  gropId();
	// void                    setProcId(loopHandleType proc);
	// void                    setGropId(loopHandleType grop);
	int             createServerS();
	int getAllLoopAndStart(serverNode* pBuff, int nBuffNum);
	msgMgr& defMsgInfoMgr (); // Thread safety
	// PhyCallback& getPhyCallback();
	ForLogicFun&  getForLogicFun();
	uword  canRouteNum ();
	void onLoopBegin(ServerIDType fId);
	void onLoopEnd(ServerIDType fId);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	void logicOnAccept(serverIdType fId, SessionIDType sessionId, uqword userData);
	serializePackFunType  fromNetPack ();
	serializePackFunType  toNetPack ();
	uword  canUpRouteServerNum ();
	void  setCanUpRouteServerNum (uword v);
	uword  canDownRouteServerNum ();
	void  setCanDownRouteServerNum (uword v);
	loopHandleType   getOnceUpServer ();
	loopHandleType   getOnceDownServer ();
	loopHandleType   getOnceUpOrDownServer ();
	uword  getAllCanRouteServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanUpServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanDownServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	bool   isRootApp ();
private:
	// loopMgr  m_loopS;
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

	uword  m_canDownRouteServerNum;
	uword  m_canUpRouteServerNum;
	serializePackFunType  m_toNetPack;
	serializePackFunType  m_fromNetPack;
	msgMgr		m_defMsgInfoMgr;
	int			m_CurLoopNum;
	// loopHandleType	m_gropId;
	std::unique_ptr<impLoop>	 m_loopS [LoopNum];
	std::unique_ptr<loopHandleType[]>	 m_canRouteServerIdS;
	typedef std::map<loopHandleType, std::string> tempLoopIdMap;
	tempLoopIdMap	m_tempLoopIdMap;
	// PhyCallback  m_callbackS;
	ForLogicFun m_forLogic;
	std::unique_ptr<CModule[]>	 m_ModuleS;
	uword			m_ModuleNum;
};

packetHead* allocPack(udword udwSize);
void	freePack(packetHead* pack);

void         lv0PushToCallStack (const char* szTxt);
void         lv0PopFromCallStack ();
void         lv0LogCallStack (int nL);

#endif
