#ifndef _logicServer_h_
#define _logicServer_h_
#include <memory>
#include "mainLoop.h"
class logicServerMgr
{
public:
	void  afterLoad(const ForLogicFun* pForLogic);
private:
};
#endif