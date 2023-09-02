#ifndef _comMsgRpch__
#define _comMsgRpch__
#include "msg.h"
/*
class addChannelAskMsg :public CMsgBase
{
public:
	addChannelAskMsg ();
	addChannelAskMsg (packetHead* p);
	
};
struct addChannelRet
{
    udword    m_result;
};
class addChannelRetMsg :public CMsgBase
{
public:
	addChannelRetMsg ();
	addChannelRetMsg (packetHead* p);
	addChannelRet* pack ();
    
};
class delChannelAskMsg :public CMsgBase
{
public:
	delChannelAskMsg ();
	delChannelAskMsg (packetHead* p);
	
};
struct delChannelRet
{
    udword    m_result;
};
class delChannelRetMsg :public CMsgBase
{
public:
	delChannelRetMsg ();
	delChannelRetMsg (packetHead* p);
	delChannelRet* pack ();
    
};
class listenChannelAskMsg :public CMsgBase
{
public:
	listenChannelAskMsg ();
	listenChannelAskMsg (packetHead* p);
	
};
struct listenChannelRet
{
    udword    m_result;
};
class listenChannelRetMsg :public CMsgBase
{
public:
	listenChannelRetMsg ();
	listenChannelRetMsg (packetHead* p);
	listenChannelRet* pack ();
    
};
class quitChannelAskMsg :public CMsgBase
{
public:
	quitChannelAskMsg ();
	quitChannelAskMsg (packetHead* p);
	
};
struct quitChannelRet
{
    udword    m_result;
};
class quitChannelRetMsg :public CMsgBase
{
public:
	quitChannelRetMsg ();
	quitChannelRetMsg (packetHead* p);
	quitChannelRet* pack ();
    
};
struct sendToChannelAsk
{
    udword    m_unUse;
    udword    m_packNum;
    ubyte    m_pack [1100000];
};
class sendToChannelAskMsg :public CMsgBase
{
public:
	sendToChannelAskMsg ();
	sendToChannelAskMsg (packetHead* p);
	sendToChannelAsk* pack ();
    packetHead*   toPack() override;
};
struct sendToChannelRet
{
    udword    m_result;
};
class sendToChannelRetMsg :public CMsgBase
{
public:
	sendToChannelRetMsg ();
	sendToChannelRetMsg (packetHead* p);
	sendToChannelRet* pack ();
    
};
*/
#endif
