//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：对Log4Z日志的简单封装
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//

#if !defined(SDLOG_H)
#define SDLOG_H

#include "SDCommon.h"
#include "log4z.h"

using namespace zsummer::log4z;
#define SD_LOG_LEVEL_DEBUG      LOG_LEVEL_DEBUG
#define SD_LOG_LEVEL_INFO		LOG_LEVEL_INFO
#define SD_LOG_LEVEL_WARNING    LOG_LEVEL_WARN
#define SD_LOG_LEVEL_ERROR      LOG_LEVEL_ERROR
#define SD_LOG_LEVEL_FATAL      LOG_LEVEL_FATAL
#define SD_LOG_LEVEL_ALARM		LOG_LEVEL_ALARM
#define SD_LOG_LEVEL_NONE		(LOG_LEVEL_FATAL + 1)

#define SD_LOG_LEVEL_FORCE_FILE SD_LOG_LEVEL_INFO

#define SDLOG_INIT             g_mglog.Init

#define SDLOG_CLOSE            g_mglog.Close

#define SDLOG_PRINTF_U(userid, level, fmt, ...)  LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, level, __FILE__, __LINE__, userid, fmt, ##__VA_ARGS__)

#define SDLOG_PRINTF(module, level, fmt, ...)  LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, level, __FILE__, __LINE__, 0, fmt, ##__VA_ARGS__)

extern CSDLog g_mglog;


class CSDLog 
{
public:
	CSDLog();
	virtual ~CSDLog();

public:

	BOOL  Init(const char * outputPath, int outputLevel, const char* configFile = NULL, const char*outputName = NULL);
	void  Close();

};

#endif // !defined(SDLOG_H)
