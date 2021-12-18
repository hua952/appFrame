#ifndef IRunable_h__
#define IRunable_h__
#include "typeDef.h"
#include "packet.h"

enum packetProcRetType
{
	eDel_packetProcRetType,
	eUndel_packetProcRetType
};

typedef packetProcRetType (*procMsgFunType)(packetHead*);

struct IMsgPmp
{
public:
	virtual int removeMsg(uword uwMsgID) = 0;
	virtual int regMsg(uword msgID, procMsgFunType pFun) = 0;
	virtual procMsgFunType findMsg(uword msgID) = 0;
};
#endif
