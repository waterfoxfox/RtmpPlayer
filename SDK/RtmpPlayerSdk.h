//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：Rtmp播放器对外DLL接口
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2020-03-01
//**************************************************************************//
#ifndef _RTMP_PLAYER_SDK_H_
#define _RTMP_PLAYER_SDK_H_

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#ifndef BUILDING_DLL
#define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
#define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
typedef signed __int64       int64_t;
#else
#define DLLIMPORT
#endif

#ifdef __APPLE__
#ifndef OBJC_BOOL_DEFINED
typedef int BOOL;
#endif 
#else
#ifndef BOOL
typedef int BOOL;
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
//输出日志级别
typedef enum LOG_OUTPUT_LEVEL
{
	LOG_OUTPUT_LEVEL_DEBUG = 1,
	LOG_OUTPUT_LEVEL_INFO,
	LOG_OUTPUT_LEVEL_WARNING,
	LOG_OUTPUT_LEVEL_ERROR,
	LOG_OUTPUT_LEVEL_ALARM,
	LOG_OUTPUT_LEVEL_FATAL,
	LOG_OUTPUT_LEVEL_NONE
} LOG_OUTPUT_LEVEL;

//RTMP连接状态
typedef enum RtmpPlay_Status{
	//开始连接
	RTMP_CONNECT_START = 0,
	//连接成功
	RTMP_CONNECT_SUCCESS,
	//开始重连
	RTMP_CONNECT_RESTART,
	//错误
	RTMP_CONNECT_ERROR
} RtmpPlay_Status;

//////////////////////////////////////////////////////////////////////////
// RtmpPlayer接口

/***
* 环境初始化，系统只需调用一次
* @param: outputPath：日志文件输出的目录，若目录不存在将自动创建
* @param: outputLevel：日志输出的级别，只有等于或者高于该级别的日志输出到文件
* @return: 
*/
DLLIMPORT void  RtmpPlayer_Enviroment_Init(const char * outputPath, LOG_OUTPUT_LEVEL outputLevel);

DLLIMPORT void  RtmpPlayer_Enviroment_Free();


/***
* 创建RtmpPlayer
* @return: 返回模块指针，为NULL则失败
*/
DLLIMPORT void*  RtmpPlayer_Create();

/***
* 销毁RtmpPlayer,注意：【涉及到资源消耗，使用者应该做好本接口与其他接口的互斥保护】
* @param pRtmpPlayer: 模块指针
* @return: 
*/
DLLIMPORT void  RtmpPlayer_Delete(void* pRtmpPlayer);

/***
* 开始拉流Rtmp并播放
* @param pRtmpPlayer: 模块指针
* @param strRtmpPlayUrl: Rtmp地址
* @param unJitterBuffDelay: 内部缓存时间，缓存时间越大延时越大、流畅性越好。反之延时越小，流畅性越差。范围[0, 2000]，单位毫秒
* @param pDisplayHandle: 渲染输出的窗口句柄
* @return: TURE成功，FALSE失败
*/
DLLIMPORT BOOL  RtmpPlayer_Start(void* pRtmpPlayer, char *strRtmpPlayUrl, UINT unJitterBuffDelay, void* pDisplayHandle);


/***
* 停止拉流Rtmp播放
* @param pRtmpPlayer: 模块指针
* @return: 
*/
DLLIMPORT void  RtmpPlayer_Stop(void* pRtmpPlayer);

/***
* 设置音量大小
* @param pRtmpPlayer: 模块指针
* @param fVolume: 范围[0, 60]，设置为0时即静音，10为原始音量，值越大音量越大
* @return: 
*/
DLLIMPORT void  RtmpPlayer_SetVolume(void* pRtmpPlayer, int nVolume);


/***
* 获取RTMP连接状态
* @param pRtmpPlayer: 模块指针
* @return: RTMP连接状态
*/
DLLIMPORT RtmpPlay_Status  RtmpPlayer_GetRtmpStatus(void* pRtmpPlayer);

#ifdef __cplusplus
}
#endif

#endif // _RTMP_PLAYER_SDK_H_
