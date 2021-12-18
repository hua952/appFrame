#ifndef _impMainLoop_h_
#define _impMainLoop_h_
#include "mainLoop.h"
#include <memory>
#include <map>
#include "tJumpListNS.h"

class PhyInfo
{
public:
	PhyInfo();
	int init(int nArgC, char* argS[], PhyCallback& info);
private:		
	PhyCallback  m_callbackS;
};

class impLoop;
class loopMgr
{
	public:
		loopMgr();
		~loopMgr();
};
#endif
