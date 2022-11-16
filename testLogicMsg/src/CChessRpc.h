#ifndef _CChess_rpc_h__
#define _CChess_rpc_h__
#include"msg.h"
struct playerInfo
{
	udword		m_accID;
	udword		m_score;
	udword		m_head;
	uword		m_lv;
	ubyte		m_side;
	char		m_name[32];//昵称
};
struct trackNode
{
	ubyte		m_from;
	ubyte		m_to;
	ubyte		m_fromManType;
	ubyte		m_toManType;
};
struct manualListNode
{
	udword		m_id;
	udword		m_time;//比赛时间
	ubyte		m_endState;//比赛结果
	ubyte		m_winSide;//胜方
	playerInfo		m_firstPlayer;
	playerInfo		m_secondPlayer;
};

class manualListAsk :public CMsgBase
{
public:
	manualListAsk();
	manualListAsk(packetHead*p);
};

struct manualListRetPack
{
	udword		m_id;
	udword	m_nodeSNum;
	manualListNode		m_nodeS[1024];//昵称
};
class manualListRet :public CMsgBase
{
public:
	manualListRet();
	manualListRet(packetHead*p);
	virtual packetHead*   toPack();
	manualListRetPack*	pack();
};

class getOnceManualAsk :public CMsgBase
{
public:
	getOnceManualAsk();
	getOnceManualAsk(packetHead*p);
};

struct getOnceManualRetPack
{
	udword		m_id;
	uword		m_manS[16];//棋子
	udword	m_trackNum;
	trackNode		m_track[1024];//棋谱
};
class getOnceManualRet :public CMsgBase
{
public:
	getOnceManualRet();
	getOnceManualRet(packetHead*p);
	virtual packetHead*   toPack();
	getOnceManualRetPack*	pack();
};

struct enterServerAskPack
{
	char		m_accName[32];
	uword		m_head;
	char		m_sinCode[257];
};
class enterServerAsk :public CMsgBase
{
public:
	enterServerAsk();
	enterServerAsk(packetHead*p);
	enterServerAskPack*	pack();
};

struct enterServerRetPack
{
	udword		m_result;
	udword		m_exp;
	uword		m_lv;
	uword		m_trackLen;
	ubyte		m_state;
	ubyte		m_inRoomState;
};
class enterServerRet :public CMsgBase
{
public:
	enterServerRet();
	enterServerRet(packetHead*p);
	enterServerRetPack*	pack();
};

class wateForGameAsk :public CMsgBase
{
public:
	wateForGameAsk();
	wateForGameAsk(packetHead*p);
};

struct wateForGameRetPack
{
	udword		m_result;
};
class wateForGameRet :public CMsgBase
{
public:
	wateForGameRet();
	wateForGameRet(packetHead*p);
	wateForGameRetPack*	pack();
};

struct gameEndAskPack
{
	udword		m_newExp[2];//最新成绩
	ubyte		m_endState;
	ubyte		m_winSide;
};
class gameEndAsk :public CMsgBase
{
public:
	gameEndAsk();
	gameEndAsk(packetHead*p);
	gameEndAskPack*	pack();
};

class giveUpAsk :public CMsgBase
{
public:
	giveUpAsk();
	giveUpAsk(packetHead*p);
};

struct giveUpRetPack
{
	udword		m_result;
};
class giveUpRet :public CMsgBase
{
public:
	giveUpRet();
	giveUpRet(packetHead*p);
	giveUpRetPack*	pack();
};

struct regretAskPack
{
	uword		m_trackLen;
};
class regretAsk :public CMsgBase
{
public:
	regretAsk();
	regretAsk(packetHead*p);
	regretAskPack*	pack();
};

struct regretRetPack
{
	udword		m_result;
};
class regretRet :public CMsgBase
{
public:
	regretRet();
	regretRet(packetHead*p);
	regretRetPack*	pack();
};

struct acceptRegretAskPack
{
	ubyte		m_accept;
};
class acceptRegretAsk :public CMsgBase
{
public:
	acceptRegretAsk();
	acceptRegretAsk(packetHead*p);
	acceptRegretAskPack*	pack();
};

struct acceptRegretRetPack
{
	udword		m_result;
};
class acceptRegretRet :public CMsgBase
{
public:
	acceptRegretRet();
	acceptRegretRet(packetHead*p);
	acceptRegretRetPack*	pack();
};

class ntfReqRegretAsk :public CMsgBase
{
public:
	ntfReqRegretAsk();
	ntfReqRegretAsk(packetHead*p);
};

class ntfRefuseRegretAsk :public CMsgBase
{
public:
	ntfRefuseRegretAsk();
	ntfRefuseRegretAsk(packetHead*p);
};

class ntfRegretAsk :public CMsgBase
{
public:
	ntfRegretAsk();
	ntfRegretAsk(packetHead*p);
};

class peaceAsk :public CMsgBase
{
public:
	peaceAsk();
	peaceAsk(packetHead*p);
};

struct peaceRetPack
{
	udword		m_result;
};
class peaceRet :public CMsgBase
{
public:
	peaceRet();
	peaceRet(packetHead*p);
	peaceRetPack*	pack();
};

struct acceptPeaceAskPack
{
	ubyte		m_accept;
};
class acceptPeaceAsk :public CMsgBase
{
public:
	acceptPeaceAsk();
	acceptPeaceAsk(packetHead*p);
	acceptPeaceAskPack*	pack();
};

struct acceptPeaceRetPack
{
	udword		m_result;
};
class acceptPeaceRet :public CMsgBase
{
public:
	acceptPeaceRet();
	acceptPeaceRet(packetHead*p);
	acceptPeaceRetPack*	pack();
};

class ntfReqPeaceAsk :public CMsgBase
{
public:
	ntfReqPeaceAsk();
	ntfReqPeaceAsk(packetHead*p);
};

class ntfRefusePeaceAsk :public CMsgBase
{
public:
	ntfRefusePeaceAsk();
	ntfRefusePeaceAsk(packetHead*p);
};

class readyAsk :public CMsgBase
{
public:
	readyAsk();
	readyAsk(packetHead*p);
};

struct readyRetPack
{
	udword		m_result;
};
class readyRet :public CMsgBase
{
public:
	readyRet();
	readyRet(packetHead*p);
	readyRetPack*	pack();
};

struct moveAskPack
{
	ubyte		m_oldPos;
	ubyte		m_newPos;
};
class moveAsk :public CMsgBase
{
public:
	moveAsk();
	moveAsk(packetHead*p);
	moveAskPack*	pack();
};

struct moveRetPack
{
	udword		m_result;
};
class moveRet :public CMsgBase
{
public:
	moveRet();
	moveRet(packetHead*p);
	moveRetPack*	pack();
};

struct moveNtfAskPack
{
	udword		m_leafTime[2];
	ubyte		m_oldPos;
	ubyte		m_newPos;
	ubyte		m_side;
};
class moveNtfAsk :public CMsgBase
{
public:
	moveNtfAsk();
	moveNtfAsk(packetHead*p);
	moveNtfAskPack*	pack();
};

struct createGameAskPack
{
	udword		m_roomID;
	udword		m_sideTime;//局时(单位毫秒)
	udword		m_setpTime;//步时(单位毫秒)
	udword		m_setpAddTime;//步加时(单位毫秒)
	ubyte		m_first;
	ubyte		m_rad;
	playerInfo		m_mainPlayer[2];
};
class createGameAsk :public CMsgBase
{
public:
	createGameAsk();
	createGameAsk(packetHead*p);
	createGameAskPack*	pack();
};

struct watchPlayerAskPack
{
	udword		m_teagerID;
};
class watchPlayerAsk :public CMsgBase
{
public:
	watchPlayerAsk();
	watchPlayerAsk(packetHead*p);
	watchPlayerAskPack*	pack();
};

struct watchPlayerRetPack
{
	udword		m_result;
};
class watchPlayerRet :public CMsgBase
{
public:
	watchPlayerRet();
	watchPlayerRet(packetHead*p);
	watchPlayerRetPack*	pack();
};

class playingGameInfoAsk :public CMsgBase
{
public:
	playingGameInfoAsk();
	playingGameInfoAsk(packetHead*p);
};

struct playingGameInfoRetPack
{
	udword		m_result;
	udword		m_roomID;
	udword		m_sideTime;//局时(单位毫秒)
	udword		m_setpTime;//步时(单位毫秒)
	udword		m_setpAddTime;//步加时(单位毫秒)
	udword		m_leafTime[2];
	udword		m_setpLeaf[2];
	ubyte		m_first;
	ubyte		m_rad;
	udword	m_memberNum;
	playerInfo		m_member[256];
};
class playingGameInfoRet :public CMsgBase
{
public:
	playingGameInfoRet();
	playingGameInfoRet(packetHead*p);
	virtual packetHead*   toPack();
	playingGameInfoRetPack*	pack();
};

struct playerEnterAskPack
{
	playerInfo		m_player;
};
class playerEnterAsk :public CMsgBase
{
public:
	playerEnterAsk();
	playerEnterAsk(packetHead*p);
	playerEnterAskPack*	pack();
};

class roomDisAsk :public CMsgBase
{
public:
	roomDisAsk();
	roomDisAsk(packetHead*p);
};

class manSAsk :public CMsgBase
{
public:
	manSAsk();
	manSAsk(packetHead*p);
};

struct manSRetPack
{
	udword		m_result;
	ubyte		m_gameState;
	ubyte		m_endState;
	ubyte		m_winSide;
	uword		m_manS[16];//棋子
	udword	m_trackNum;
	trackNode		m_track[512];//棋谱
};
class manSRet :public CMsgBase
{
public:
	manSRet();
	manSRet(packetHead*p);
	virtual packetHead*   toPack();
	manSRetPack*	pack();
};

#endif
