#include <iostream>
#include <memory>
#include "cppLevel0L.h"

static std::unique_ptr<pserver[]>	 g_serverS;
static uword			g_ServerNum;
uword getServerNum()
{
	return g_ServerNum;
}

pserver* getServerS()
{
	return g_serverS.get();
}

int sendPackToLoop(packetHead*)
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


int initFun (int cArg, const char* argS[])
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
		std::cout<<"initFun loopNum = "<<loopNum<<std::endl;
		if (loopNum > 0)
		{
			g_ServerNum = loopNum;
			g_serverS = std::make_unique<pserver[]>(loopNum);
			std::unique_ptr<server[]>	pServerImpS =  std::make_unique<server[]>(loopNum);

			auto pServerS = getServerS();
			auto pImpS = pServerImpS.get();
			for (int i = 0; i < loopNum; i++ )
			{
				pServerS[i] = &pImpS[i];
				auto loopH = loopHandleS [i];
				auto p = pServerS [i];
				p->init (loopH);
			}

			for (int i = 0; i < loopNum; i++ )
			{
				auto p = pServerS[i];
				p->start();
			}

			for (int i = 0; i < loopNum; i++ )
			{
				auto p = pServerS [i];
				p->join();
			}
			std::cout<<"All server End"<<std::endl;
		}
	} while (0);
	return nRet;

	return 0;
}
