#include"cTimer.h"
#include <cstring>

cTimer::cTimer(udword udwNextTime, udword udwSetp)
{
	init(udwNextTime, udwSetp);
}

int cTimer::init(udword udwNextTime, udword udwSetp)
{
	m_NextTime = udwNextTime;
	m_Setp = udwSetp;
	return 0;
}

udword  cTimer::nextTime()const
{
	return m_NextTime;
}   

void    cTimer::setNextTime(udword udwT)
{
	m_NextTime = udwT;
}

udword  cTimer::setp()
{
	return m_Setp;
}

void    cTimer::setSetp(udword udwS)
{
	m_Setp = udwS;
}

udword  cTimer::ser()
{
	return m_Ser;
}

void  cTimer::  setSer(udword udwSer)
{
	m_Ser = udwSer;
}

uqword  cTimer::cmpKey()
{
	uqword uqwR = m_NextTime;
	uqwR <<= 32;
	uqwR += m_Ser;
	return uqwR;
}

cTimer::~cTimer()
{
}
	
cTimerMgr::ComTimer::ComTimer(udword udwNextTime, udword udwSetp,
	ComTimerFun pF, void* pUsrData, udword userDataLen):cTimer(udwNextTime, udwSetp)/*, m_pUsrData(pUsrData)*/,
	m_pF(pF)
{
	if (pUsrData && userDataLen) {
		m_pUsrData = std::make_unique<char[]>(userDataLen);
		auto pD = m_pUsrData.get();
		memcpy(pD, pUsrData, userDataLen);
	}
}

cTimerMgr::ComTimer::~ComTimer()
{
}

cTimerRemoveType cTimerMgr::ComTimer::onTimer()
{
	return m_pF(m_pUsrData.get()) ? eNotRemoveTimer : eDeleteTimer;
}

void cTimerMgr::addComTimer(udword udwSetp, ComTimerFun pF, void* pUsrData, udword userDataLen)
{
	addComTimer(udwSetp, udwSetp, pF, pUsrData, userDataLen);
}

void cTimerMgr::addComTimer(udword udwFirst, udword udwSetp, ComTimerFun pF,
		void* pUsrData, udword userDataLen)
{
	auto nCur = curClock();
	auto reFirst = udwFirst;
	reFirst += 9;
	reFirst /= 10;
	auto nextT = reFirst + nCur;
	auto reSetp = udwSetp;
	reSetp += 9;
	reSetp /= 10;
	auto pAdd = new ComTimer(nextT, reSetp, pF, pUsrData, userDataLen);
	pAdd->setSer(m_udwNexSer++);
	addTimer(pAdd);
}

bool cTimerMgr::PcTimerCmp::operator()(const PcTimer& a, const PcTimer& b)
{
	return a->cmpKey() < b->cmpKey();
}

cTimerMgr::cTimerMgr()
{
	m_udwNexSer = 1;
	m_startTime = std::chrono::system_clock::now();
}

static bool s_DelTimerV(void* pU, PcTimer& rT)
{
	delete rT;
	return true;
}

cTimerMgr::~cTimerMgr()
{
	m_list.visit(NULL, s_DelTimerV);
}

void cTimerMgr::addTimer(cTimer* pTimer)
{
	m_list.insert(pTimer);
}

static bool s_onTimer(void* pU, PcTimer& pT)
{
	cTimerRemoveType nR = pT->onTimer();
	if(eNotRemoveTimer == nR)
	{
		udword udwNext = pT->nextTime();
		udwNext += pT->setp();
		pT->setNextTime(udwNext);
		cTimerMgr* pMgr = (cTimerMgr*)pU;
		pMgr->addTimer(pT);
	}
	else
	{
		s_DelTimerV(NULL, pT);
	}
	return true;
}

namespace temp
{
	class findTimer:public cTimer
	{
	public:

		findTimer(udword udwNextTime, udword udwSetp):cTimer(udwNextTime, udwSetp)
		{
		}
		cTimerRemoveType onTimer(){ return eDeleteTimer;}
	};
}

void cTimerMgr::onFrame()
{
	auto cur = curClock ();
	temp::findTimer ft(cur, 0);
	m_list.delAndVisit(&ft, this, s_onTimer);
}

udword cTimerMgr::curClock ()
{
	auto curTP = std::chrono::system_clock::now();
	auto curDu = curTP - m_startTime;
	auto nRet = (std::chrono::duration_cast<std::chrono::milliseconds>(curDu)).count();
	nRet /= 10;
	return (udword)nRet;
}

udword cTimerMgr::clockNumPerSec ()
{
	return 100;
}

