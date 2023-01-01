#ifndef _cLog_h__
#define _cLog_h__

#include "typeDef.h"
#include <iostream>
#include <memory>

int initLog (const char* logName, const char* logfileName, uword minLevel);
int logMsg (const char* logName, const char* szMsg, uword wLevel);
void loggerDrop();
/*
#define rTrace(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_TRACE(spdlog::get("cppLevel0"), __VA_ARGS__)
#define rDebug(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_DEBUG(spdlog::get("cppLevel0"), __VA_ARGS__)
#define rInfo(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_INFO(spdlog::get("cppLevel0"), __VA_ARGS__)
#define rWarn(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_WARN(spdlog::get("cppLevel0"), __VA_ARGS__)
#define rError(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__);SPDLOG_LOGGER_ERROR(spdlog::get("cppLevel0"), __VA_ARGS__)
*/
#endif
