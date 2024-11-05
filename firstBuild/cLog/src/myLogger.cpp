#include "myLogger.h"
#include "strFun.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <thread>

uword myLogger::s_comLevel[c_comLevelNum ] = {1000, 2000, 3000, 4000, 5000};
const char* myLogger::s_comLevelShow[c_comLevelNum + 1] = {"T", "D", "I", "W", "E", "F"};
uword myLogger::getLevelIndex (uword uwLevel)
{
	uword nRet = 0;
	do {
		auto nS = sizeof (s_comLevel) / sizeof (s_comLevel[0]);
		decltype (nS) i = 0;
		for (; i < nS; i++) {
			if (uwLevel < s_comLevel[i]) {
				break;
			}
		}
		nRet = i;
	} while(0);
	return nRet;
}

bool  myLogger:: logLevelType ()
{
    return m_logLevelType;
}

void  myLogger:: setLogLevelType (bool v)
{
    m_logLevelType = v;
}

myLogger:: myLogger (const char* fileName):m_os(fileName, std::ios::app)
{
	m_minLevel = s_comLevel[2] - 500;
	m_logLevelType = true;
	m_logTime = true;
	m_logToCon = true;
	m_logToFile = true;
	m_logThreId = true;
}

myLogger:: ~myLogger ()
{
}

bool  myLogger:: logTime ()
{
    return m_logTime;
}

void  myLogger:: setLogTime (bool v)
{
    m_logTime = v;
}

uword  myLogger:: minLevel ()
{
    return m_minLevel;
}

void  myLogger:: setMinLevel (uword v)
{
    m_minLevel = v;
}

int    myLogger:: logTxt (const char* szTxt, uword uwLevel)
{
    int    nRet = 0;
    do {
		if (uwLevel < m_minLevel) {
			break;
		}
		std::stringstream ss;
		if (logTime ()) {
			time_t t = time(0); 
			char tmp[32]={NULL};
			strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&t));
			ss<<tmp<<" ";
		}
		if (logLevelType ()) {
			auto uwI = getLevelIndex (uwLevel);
			ss<<s_comLevelShow[uwI]<<" ";
		}
		if (logThreId ()) {
			auto uwI = getLevelIndex (uwLevel);
			ss<<"["<<std::this_thread::get_id()<<"] ";

		}
		ss<<szTxt<<" "<<std::endl;
		// auto& ws = std::cout;
		if (logToCon ()) {
			/*
			std::lock_guard<std::mutex> lock(m_mtxCon);
			ws<<ss.str();
			*/
			writeToCon (ss.str().c_str());
		}

		// auto& os = m_os;
		if (logToFile ()) {
			/*
			std::lock_guard<std::mutex> lock(m_mtxFile);
			os<<ss.str();
			*/
			writeToFile (ss.str().c_str());
		}
    } while (0);
    return nRet;
}

void   myLogger:: writeToCon(const char* txt)
{
	std::lock_guard<std::mutex> lock(m_mtxCon);
	std::cout<<txt;
}

void   myLogger:: writeToFile(const char* txt)
{
	std::lock_guard<std::mutex> lock(m_mtxFile);
	m_os<<txt;
}

bool  myLogger:: logToCon ()
{
    return m_logToCon;
}

void  myLogger:: setLogToCon (bool v)
{
    m_logToCon = v;
}

bool  myLogger:: logToFile ()
{
    return m_logToFile;
}

void  myLogger:: setLogToFile (bool v)
{
    m_logToFile = v;
}

bool  myLogger:: logThreId ()
{
    return m_logThreId;
}

void  myLogger:: setLogThreId (bool v)
{
    m_logThreId = v;
}

