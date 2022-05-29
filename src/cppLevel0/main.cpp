#include "main.h"
#include "mainLoop.h"

int sendPackToLoop(netPacketHead*, uword)
{
	int nRet = 0;
	return nRet;
}

void stopLoopS()
{
}

packetHead* allocPack(udword udwSize)
{
	return (packetHead*)(new char [sizeof(packetHead) + udwSize]);
}

void	freePack(packetHead* pack)
{
	delete [] ((char*)(pack));
}

PhyCallback&  getPhyCallback()
{
	static PhyCallback s_Mgr;
	return s_Mgr;
}

int main(int cArg, const char* argS[])
{
	std::cout<<"start main"<<std::endl;
	int nRet = 0;
	auto& rMgr = getPhyCallback();
	rMgr.fnSendPackToLoop = sendPackToLoop;
	rMgr.fnStopLoopS = stopLoopS;
	rMgr.fnAllocPack = allocPack;
	rMgr.fnFreePack = freePack;

	do
	{
		auto nInitMidFrame = InitMidFrame(cArg, argS, &rMgr);
		if (0 != nInitMidFrame)
		{
			nRet = 1;
			break;
		}
		const auto c_maxLoopNum = 10;
		loopHandleType  loopHandleS[c_maxLoopNum];
		auto loopNum =  getAllLoopAndStart(loopHandleS, c_maxLoopNum);
	} while (0);
	return nRet;
}
