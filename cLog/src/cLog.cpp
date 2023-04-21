#include "cLog.h"
#include <iostream>
#include <windows.h>
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
	auto file_logger = spdlog::rotating_logger_mt("cppLevel0", "logs/cppLevel0.txt", 1024 * 1024 * 20,  10);
	//auto con_logger = spdlog::color_logger_mt ("cppLevel0_con");
	auto nL = getLogLevel (minLevel);
	spdlog::flush_every(std::chrono::seconds(2));
	spdlog::set_level((decltype(spdlog::level::trace))(nL));
	// spdlog::set_pattern("[%Y-%m-%d %H:%M:%S][%l][%p:%t][%s:%#] %v");
	// spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l] [%t] - <%s>|<%#>|<%!>,%v");
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
		//SPDLOG_LOGGER_DEBUG(logger, szMsg);
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->debug(szMsg); });
		break;
	case spdlog::level::info:
		//SPDLOG_LOGGER_INFO(logger, szMsg);
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info(szMsg); });
		break;
	case spdlog::level::warn:
		//SPDLOG_LOGGER_WARN(logger, szMsg);
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->warn(szMsg); });
		break;
	case spdlog::level::err:
		//SPDLOG_LOGGER_ERROR(logger, szMsg);
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->error(szMsg); });
		break;
	case spdlog::level::critical:
		//SPDLOG_LOGGER_CRITICAL(logger, szMsg);
		spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->critical(szMsg); });
		break;
	default:
		nRet = 1;
		break;
	}
	return nRet;
}
/*
int logTxt (const char* logName, const char* szTxt, int level)
{
}
*/
