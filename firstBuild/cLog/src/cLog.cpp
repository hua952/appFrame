#include "cLog.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include "myLogger.h"
#include "myLoggerMgr.h"
#include "tSingleton.h"

/*
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

static int getLogLevel (uword wI)
{
	int nRet = 0;
	int  levelS[] = {spdlog::level::trace, spdlog::level::debug, spdlog::level::info, spdlog::level::warn, spdlog::level::err, spdlog::level::critical};
	const auto c_levelNum = sizeof (levelS) / sizeof (levelS[0]);
	if (wI < c_levelNum) {
		nRet = levelS[wI];
	}
	return nRet;
}

int initLog (const char* logName, const char* logfileName, uword minLevel)
{
	int nRet = 0;
	auto nLevel = getLogLevel (minLevel);
	std::string strFile;
	strFile += logfileName;
	auto file_logger = spdlog::rotating_logger_mt("cppLevel0", strFile.c_str(), 1024 * 1024 * 20,  10);
	auto nL = getLogLevel (minLevel);
	spdlog::flush_every(std::chrono::seconds(2));
	spdlog::set_level((decltype(spdlog::level::trace))(nL));
	spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%L] [%t] %v");
	return nRet;
}

void loggerDrop()
{
	spdlog::drop_all();
}

int logMsg (const char* logName, const char* szMsg, uword wLevel)
{
	int nRet = 0;
	auto nL = getLogLevel (wLevel);
	auto enL = (decltype(spdlog::level::trace))(nL);
	auto logger = spdlog::get(logName);
	switch (enL) {
	case spdlog::level::trace:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->trace(szMsg); });
		break;
	case spdlog::level::debug:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->debug(szMsg); });
		break;
	case spdlog::level::info:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info(szMsg); });
		break;
	case spdlog::level::warn:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->warn(szMsg); });
		break;
	case spdlog::level::err:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->error(szMsg); });
		break;
	case spdlog::level::critical:
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->critical(szMsg); });
		break;
	default:
		nRet = 1;
		break;
	}
	return nRet;
}
*/

int initLogGlobal ()
{
	int nRet = 0;
	do {
		tSingleton<myLoggerMgr>::createSingleton ();
	} while(0);
	return nRet;
}

int initLog (const char* logName, const char* logfileName, uword minLevel, bool logCon)
{
	int nRet = 0;
	do {
		auto c_num = myLogger::c_comLevelNum;
		if (minLevel >= c_num) {
			nRet = 1;
			break;
		}
		auto& logs = tSingleton<myLoggerMgr>::single();
		logs.m_myLogger = std::make_unique<myLogger>(logfileName);
		//auto pLog = std::make_shared <myLogger>(logfileName);
		auto pLog = logs.m_myLogger.get(); //std::make_shared <myLogger>(logfileName);
		pLog->setMinLevel (myLogger::s_comLevel[minLevel] - 1000);
		pLog->setLogToCon (logCon);
		// nRet = logs.addLogger (logName, pLog);
	} while (0);
	return nRet;
}
/*
int initLog (const char* logName, const char* logfileName, uword minLevel)
{
	return 	initLog (logName, logfileName, minLevel, true);
}
*/
int logMsg (const char* logName, const char* szMsg, uword wLevel)
{
	int nRet = 0;
	do {
		auto& logs = tSingleton<myLoggerMgr>::single();
		auto pL = logs.findLogger(logName);
		if (!pL) {
			nRet = 1;
			break;
		}

		auto c_num = myLogger::c_comLevelNum;
		// const auto c_num = sizeof(myLogger::s_comLevel)/sizeof(myLogger::s_comLevel[0]);
		if (wLevel >= c_num) {
			nRet = 2;
			break;
		}
		nRet = pL->logTxt (szMsg, myLogger::s_comLevel[wLevel] - 500);
	} while(0);
	return nRet;
}

int setLogAttr(const char* logName, const char* szKey, const char* szVal) // Not Thread safety, when you neet it, you shuld call it before start log
{
	int nRet = 0;
	do {
		auto& logs = tSingleton<myLoggerMgr>::single();
		auto pL = logs.findLogger(logName);
		if (!pL) {
			nRet = 1;
			break;
		}
		if (strcmp(szKey, "minLevel") == 0) {
			pL->setMinLevel((uword)(atoi(szVal)));
		} else if (strcmp(szKey, "logToCon") == 0) {
			pL->setLogToCon(atoi(szVal));
		} else {
			nRet = 2;
		}
	} while (0);
	return nRet;
}

void loggerDrop()
{
}
