#ifndef serverMgr_h__
#define serverMgr_h__
#include "server.h"
#include "msgMgr.h"
#include "CModule.h"
#include <map>

typedef server* pserver;
packetHead* allocPack(udword udwSize);// Thread safety
void	freePack(packetHead* pack);// Thread safety

int fromNetPack (netPacketHead* pN, pPacketHead& pNew);// Thread safety
int toNetPack (netPacketHead* pN, pPacketHead& pNew);// Thread safety
class serverMgr
{
public:
	using  serverPair = std::pair<std::unique_ptr<server[]>, loopHandleType>;
	using  muServerPairS = std::pair<std::unique_ptr<serverPair[]>, loopHandleType>;
	serverMgr();
	~serverMgr();
	int				initFun (int cArg, char** argS);
	server*         getServer(loopHandleType handle);
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
    udword    packSendInfoTime ();
    void  setPackSendInfoTime (udword va);
    udword    delSendPackTime ();
    void  setDelSendPackTime (udword va);

	int pushPackToLoop (loopHandleType pThis, packetHead* pack);// Thread safety

	server*   getLoop(loopHandleType id);
	int             init(int nArgC, char** argS/*, PhyCallback& info*/);
	int   initNetServer ();
	
	int             createServerS();
	int getAllLoopAndStart(serverNode* pBuff, int nBuffNum);
	msgMgr& defMsgInfoMgr (); // Thread safety
	ForLogicFun&  getForLogicFun();
	uword  canRouteNum ();
	void onLoopBegin(ServerIDType fId);
	void onLoopEnd(ServerIDType fId);
	void logicOnConnect(serverIdType fId, SessionIDType sessionId, uqword userData);
	void logicOnAccept(serverIdType fId, SessionIDType sessionId, uqword userData);
	// serializePackFunType  fromNetPack ();
	// serializePackFunType  toNetPack ();
	uword  canUpRouteServerNum ();
	void  setCanUpRouteServerNum (uword v);
	uword  canDownRouteServerNum ();
	void  setCanDownRouteServerNum (uword v);
	// loopHandleType   getOnceUpServer ();
	// loopHandleType   getOnceDownServer ();
	// loopHandleType   getOnceUpOrDownServer ();
	uword  getAllCanRouteServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanUpServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	uword  getAllCanDownServerS (loopHandleType* pBuff, uword buffNum); // Thread safety
	bool   isRootApp ();
	CModule&  ModuleMgr ();
	muServerPairS*  muServerPairSPtr ();
	serverIdType  netServerTmp ();
	void          setNetServerTmp (serverIdType serverId);
	server*       getNetServerS (uword& num);
	server*       getOnceNetServer ();
private:
	serverIdType  m_netServerTmp;
	muServerPairS  m_muServerPairS[c_serverLevelNum];
	CModule  m_ModuleMgr;
	uword  m_serverNum;
    udword  m_delSendPackTime;
    udword  m_packSendInfoTime;
    delTcpServerFT  m_delTcpServerFn;
    createTcpServerFT  m_createTcpServerFn;
    ubyte  m_outNum;
	int procArgS(int nArgC, char** argS);
	loopHandleType	m_procId;
	loopHandleType	m_gropId;
	std::unique_ptr<char[]>      m_netLibName;

	uword  m_canDownRouteServerNum;
	uword  m_canUpRouteServerNum;
	// serializePackFunType  m_toNetPack;
	// serializePackFunType  m_fromNetPack;
	msgMgr		m_defMsgInfoMgr;
	std::unique_ptr<loopHandleType[]>	 m_canRouteServerIdS;
	ForLogicFun m_forLogic;
};

packetHead* allocPack(udword udwSize);
void	freePack(packetHead* pack);

void         lv0PushToCallStack (const char* szTxt);
void         lv0PopFromCallStack ();
void         lv0LogCallStack (int nL);

#endif
