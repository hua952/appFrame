#ifndef _cTimer_h_
#define _cTimer_h_
#include "tJumpListNS.h"
#include <chrono>
#include <memory>
#include "comFun.h"

enum cTimerRemoveType
{
	eDeleteTimer,
	eNotRemoveTimer
};

class cTimer
{
public:
	cTimer(udword udwNextTime, udword udwSetp);
	int init(udword udwNextTime, udword udwSetp);
	virtual cTimerRemoveType onTimer() = 0;
	virtual ~cTimer();
	udword  nextTime()const;
	void    setNextTime(udword udwT);
	udword  setp();
	void    setSetp(udword udwS);
	udword  ser();
	void    setSer(udword udwSer);
	uqword  cmpKey();
protected:	
	udword					m_NextTime;
	udword					m_Setp;
	udword					m_Ser;
};

typedef cTimer* PcTimer;

class cTimerMgr
{
public:
	class ComTimer:public cTimer
	{
	public:
		ComTimer(udword  udwNextTime, udword udwSetp, ComTimerFun pF, void* pUsrData, udword userDataLen);
		virtual ~ComTimer();
		virtual cTimerRemoveType onTimer();
	private:
		ComTimerFun m_pF;
		//void*  m_pUsrData;
		std::unique_ptr<char[]>  m_pUsrData;
	};
	class PcTimerCmp
	{
	public:
		bool operator()(const PcTimer& a, const PcTimer& b);
	};
	typedef tJumpListNS<PcTimer, 6, PcTimerCmp, 8> TimerList;
	cTimerMgr();
	~cTimerMgr();
	void addComTimer(udword udwSetp, ComTimerFun pF, void* pUsrData, udword userDataLen);
	void addComTimer(udword firstSetp, udword udwSetp, ComTimerFun pF, void* pUsrData, udword userDataLen);
	void addTimer(cTimer* pTimer);
	void onFrame();
	virtual udword curClock ();
	virtual udword clockNumPerSec ();
private:
	std::chrono::system_clock::time_point  m_startTime;
	udword	   m_udwNexSer;
	TimerList  m_list;
};

#endif
