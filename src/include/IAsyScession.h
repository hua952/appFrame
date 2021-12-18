#ifndef _IAsyScession_h__
#define _IAsyScession_h__

#include "typeDef.h"
#include "packet.h"
enum IAsyScessionState
{
	oK_IAsyScessionState,
	disCon_IAsyScessionState,
	closed_IAsyScessionState
};

struct IAsyScessionCallBack
{
	virtual void onStateChangea(IAsyScessionState from, IAsyScessionState to) = 0;
	virtual void onRecPack(packetHead* pack) = 0;
};

struct IAsyScession
{
	virtual void* userData() = 0;
	virtual void  setUserData(void* pData) = 0;
	virtual IAsyScessionState state() =0;
	virtual int send(packetHead* pack) = 0;
	virtual int close() = 0;
	virtual void setCallBack(IAsyScessionCallBack* pI) = 0;
};

#endif 
