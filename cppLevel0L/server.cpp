#include <iostream>
#include "server.h"
#include "loop.h"
#include "cppLevel0L.h"
#include "comFun.h"
#include "myAssert.h"
#include "tSingleton.h"
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds

server::server()
{
	m_loopHandle = c_emptyLoopHandle;
}

server::~server()
{
}

void server::ThreadFun(server* pS)
{
	pS->run();
}

int server::init(loopHandleType  loopHandle)
{
	m_loopHandle = loopHandle;
	return 0;
}

bool server::start()
{
	m_pTh =std::make_unique<std::thread>(server::ThreadFun, this);
	return true;
}

void server::join()
{
	myAssert (m_pTh);
	m_pTh->join();
}

void server::run()
{
	while(true)
	{
		auto bExit = onFrame();
		if (bExit)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono:: milliseconds(1));
	}
	auto& os = std::cout;
	os<<"Loop "<<(int)(m_loopHandle)<<" exit"<<std::endl;
}

bool server::pushPack (packetHead* pack)
{
	return m_slistMsgQue.pushPack (pack);
}

bool server::onFrame()
{
	bool bExit = false;
	auto nQuit = OnLoopFrame(m_loopHandle); // call by level 0
	if (procPacketFunRetType_exitNow == nQuit)
	{
		bExit = true;
	}
	else
	{
		if (procPacketFunRetType_exitAfterLoop == nQuit)
		{
			bExit = true;
		}
		packetHead head;
		auto pH = &head;
		m_slistMsgQue.getMsgS(pH);
		auto n = pH->pNext;
		auto& rMgr = tSingleton<serverMgr>::single().getPhyCallback();
		auto fnFreePack = rMgr.fnFreePack;
		while (n != pH)
		{
			auto d = n;
			n = n->pNext;
			auto nRet = processOncePack(m_loopHandle, d);// call by level 0
			if (procPacketFunRetType_doNotDel == nRet)
			{
				auto p = d->pPer;
				p->pNext = n;
				n->pPer = p;
			}
			else
			{
				if (procPacketFunRetType_exitNow == nRet)
				{
					bExit = true;
					break;
				}
				if (procPacketFunRetType_exitAfterLoop == nRet)
				{
					bExit = true;
				}
			}
		}
		n = pH->pNext;
		while (n != pH)
		{
			auto d = n;
			n = n->pNext;
			fnFreePack (d);
		}
	}
	return bExit;
}

void toGPL (serverIdType serverId, serverIdType& g, serverIdType& p, serverIdType& l)
{
	g = serverId;
	g >>= (ProcNumBitLen + LoopNumBitLen);
	p = serverId;
	p &= ProcMark;
	p >>= LoopNumBitLen;
	l = serverId;
	l &= LoopMark;
}

static void stopLoopS()
{
}

static int sendPackToLoop(packetHead* pack)
{
	int nRet = 0;
	auto pNH = P2NHead(pack);
	ubyte ubySg;
	ubyte ubySp;
	ubyte ubySl;
	toGPL (pNH->ubySrcServId, ubySg, ubySp, ubySl);

	ubyte ubyDg;
	ubyte ubyDp;
	ubyte ubyDl;
	toGPL (pNH->ubyDesServId, ubyDg, ubyDp, ubyDl);
	if (ubySg == ubyDg)
	{
		if (ubySp == ubyDp)
		{
			tSingleton<serverMgr>::createSingleton();
			auto& rMgr = tSingleton<serverMgr>::single();
			auto pServerS = rMgr.getServerS();
			auto pS = pServerS[ubyDl];
			pS->pushPack (pack);
		}
		else
		{
		}
	}
	else
	{
	}
	return nRet;
}


static packetHead* allocPack(udword udwSize)
{
	return (packetHead*)(new char [sizeof(packetHead) + udwSize]);
}

static void	freePack(packetHead* pack)
{
	delete [] ((char*)(pack));
}


serverMgr::serverMgr()
{
}

serverMgr::~serverMgr()
{
}

serverIdType 	serverMgr::getServerNum()
{
	return g_ServerNum;
}

pserver* serverMgr::getServerS()
{
	return g_serverS.get();
}

PhyCallback&  serverMgr::getPhyCallback()
{
	return m_PhyCallback;
}


int serverMgr::initFun (int cArg, const char* argS[])
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
