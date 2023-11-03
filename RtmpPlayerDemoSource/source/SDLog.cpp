//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：对Log4Z日志的简单封装
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "SDLog.h"


CSDLog g_mglog;
static BOOL g_log_init = FALSE;

CSDLog::CSDLog()
{

}

CSDLog::~CSDLog()
{

}

//如果未调用本函数不会有异常，只是不能输出日志
BOOL  CSDLog::Init(const char* outputPath, int outputLevel, const char* configFile, const char*outputName)
{
	if (g_log_init)
	{
		return FALSE;
	}

	if ((outputLevel >= SD_LOG_LEVEL_NONE) || (outputLevel < SD_LOG_LEVEL_DEBUG))
	{
		//不启用日志
		ILog4zManager::getRef().enableLogger(LOG4Z_MAIN_LOGGER_ID, false);
		g_log_init = TRUE;
		return TRUE;
	}

	if (outputPath)
	{
		ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, outputPath);
	}

	if (outputName)
	{
		ILog4zManager::getRef().setLoggerName(LOG4Z_MAIN_LOGGER_ID, outputName);
	}

	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, (ENUM_LOG_LEVEL)outputLevel);

	//若指定了配置文件，则从配置文件读取outputPath、outputLevel等配置，将覆盖传入的值
	if (configFile)
	{
		ILog4zManager::getRef().config(configFile);
		//若指定了配置文件，则启用配置文件热更新，即间隔一定时间秒数读一次配置文件看是否发生变化
	    ILog4zManager::getRef().setAutoUpdate(10);
	}

	bool bRet = ILog4zManager::getRef().start();

	g_log_init = TRUE;

	return bRet == true ? TRUE:FALSE;
}

void  CSDLog::Close()
{
	g_log_init = FALSE;
}

