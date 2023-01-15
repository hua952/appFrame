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
	uword                   uwTag;
 }netPacketHead ,*pNetPacketHead;

#define     P2NHead(p)  ((pNetPacketHead)(p+1))
#define     N2PHead(p)  ((pPacketHead)(p-1))

#define     N2User(p)   ((p)+1)
#define     P2User(p)   (N2User(P2NHead(p)))

#define     T2NHead(p)  ((PNetPacketHead)(p+1))
#define     N2THead(p)  ((PThreadPacketHead)(p-1))

#define     NIsRet(p) ((p->uwTag&1))
#define     NSetRet(p) ((p->uwTag|=1))

//#define     ThreadHeadSize (sizeof(ThreadPacketHead))
#define		PacketHeadSize    (sizeof(packetHead))
#define     NetMsgLenSize  (sizeof(netPacketHead))
#define     NetHeadSize   (NetMsgLenSize)
#define     AllPacketHeadSize  (PacketHeadSize+NetHeadSize)
//#define     NetPacketHeadSize (sizeof(NetPacketHead))
//#define     FullPacketHeadSize   (sizeof(PacketHead)+NetPacketHeadSize)

typedef packetHead* (*allocPackFT)(udword udwSize);
typedef void		(*freePackFT)(packetHead* pack);
typedef int (*processNetPackFunT)(packetHead* pack, uqword session);
#endif
