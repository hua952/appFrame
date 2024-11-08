#ifndef _packet__h
#define _packet__h

#include<string>
#include "typeDef.h"

typedef struct _ThreadPacketHead
{
    struct _ThreadPacketHead* pNext;
    struct _ThreadPacketHead* pPer;
    udword udwRef;
    uword udwCmdID;
    ubyte  ubyMemLv;
    ubyte  ubyUnuse;
}ThreadPacketHead, *PThreadPacketHead;

typedef struct _packetHead
{
	struct _packetHead*    pNext;
	struct _packetHead*    pPer;
	// uqword    pAsk;
	uqword    packArg;
	udword    sessionID;
	ubyte     loopId;
	ubyte    Tag[3];
}packetHead;

typedef packetHead* pPacketHead;

typedef udword NetTokenType;
#define c_null_msgID c_null_uword

//typedef ubyte   serverIdType;
typedef uword serverIdType;
typedef uword msgIdType;
typedef struct  _netPacketHead
 {
	udword					udwLength;
	NetTokenType			dwToKen;
	serverIdType	ubySrcServId;
	serverIdType	ubyDesServId;
	msgIdType		uwMsgID;
	ubyte			uwTag;
	ubyte           inGateSerId;
 }netPacketHead ,*pNetPacketHead;

#define     P2NHead(p)  ((pNetPacketHead)(p+1))
#define     N2PHead(p)  ((pPacketHead)(p-1))

#define     NIsExtPH(p) ((p->uwTag&0x40))
#define     NSetExtPH(p) ((p->uwTag|=0x40))

#define     NIsRet(p) (p->uwTag&0x01)
#define     NSetRet(p) (p->uwTag|=0x01)

#define     NIsDesOnce(p) (p->uwTag&0x02)
#define     NSetDesOnce(p) (p->uwTag|=0x02)

#define     NIsToAllGate(p) (p->uwTag&0x04)
#define     NSetToAllGate(p) (p->uwTag|=0x04)
/*
#define     NIsOtherNetLoopSend(p) ((p->uwTag&0x20))
#define     NSetOtherNetLoopSend(p) ((p->uwTag|=0x20))
#define     NSetNotOtherNetLoopSend(p) ((p->uwTag&=~0x20))
*/

#define     NNeetRet(p) ((p->uwTag&0x80))
#define     NSetNeetRet(p) ((p->uwTag|=0x80))
#define     NSetUnRet(p) ((p->uwTag&=~0x80))

/*
#define     NIsAskSave(p) ((p->uwTag&0x40))
#define     NSetAskSave(p) ((p->uwTag|=0x40))

*/
#define     N2User(p)   ((p)+((NIsExtPH(p))?2:1))
#define     P2User(p)   (N2User(P2NHead(p)))

#define     T2NHead(p)  ((PNetPacketHead)(p+1))
#define     N2THead(p)  ((PThreadPacketHead)(p-1))

enum packAddrType
{
	addrType_ser,
	addrType_ch,
};

#define     NAddType(p) ((p->uwTag&0x03))
#define     NSetAddSer(p) ((p->uwTag&=(~0x03)))
#define     NSetAddCh(p) (p->uwTag&=(~0x03));(p->uwTag|=0x01)
#define     NIsChAddType(p) (NAddType(p)==addrType_ch)

#define		PacketHeadSize    (sizeof(packetHead))
#define     NetMsgLenSize  (sizeof(netPacketHead))
#define     NetHeadSize   (NetMsgLenSize)
// #define     AllNetHeadSize(p)   (((NIsExtPH(p))?2:1)*NetMsgLenSize)
// #define     AllNetSendSize(p)   (AllNetHeadSize(p)+p->udwLength)
#define     AllPacketHeadSize  (PacketHeadSize+NetHeadSize)
//#define     NetPacketHeadSize (sizeof(NetPacketHead))
//#define     FullPacketHeadSize   (sizeof(PacketHead)+NetPacketHeadSize)

typedef packetHead* (*allocPackFT)(udword udwSize);
typedef void		(*freePackFT)(packetHead* pack);
typedef int (*processNetPackFunT)(packetHead* pack, uqword session);
#endif
