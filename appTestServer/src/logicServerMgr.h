#ifndef logicServerMgr_h__
#define logicServerMgr_h__

#include "mainLoop.h"
class logicServerMgr
{
public:
    logicServerMgr ();
    ~logicServerMgr ();
	void  afterLoad(ForLogicFun* pForLogic);
private:
};
#endif
