//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：跨平台的基础函数实现
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#if !defined(COMMON_H)
#define COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#ifdef WIN32
#include "tchar.h"
#include "WINSOCK2.H"
#include "WTYPES.H"
#include "WINBASE.H"
#include <io.h>

#pragma comment(lib, "ws2_32.lib")
#pragma  warning(disable:4081);
#pragma  warning(disable:4786);
#pragma  warning(disable:4018);
#pragma  warning(disable:4083);
#pragma  warning(disable:4786);
#pragma  warning(disable:4996);

typedef int				socklen_t;
typedef short			int16_t;
typedef unsigned long	INT64_C;

struct timezone 
{
	int  tz_minuteswest;
	int  tz_dsttime;
};
int gettimeofday(struct timeval *tv, struct timezone *tz);

#else

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#ifndef __APPLE__
#include <net/if_arp.h>
typedef int BOOL;
#else
#include <objc/objc.h>	
#endif


#include <errno.h>
#include <fcntl.h>

typedef int SOCKET;
#define SOCKET_ERROR (-1)
#define SO_DONTLINGER   (int)(~SO_LINGER)
#define INVALID_HANDLE_VALUE (HANDLE)-1

typedef void *HANDLE;

typedef int INT;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef long  LONG;
typedef unsigned short USHORT;
typedef USHORT WORD;
typedef unsigned long ULONG;
typedef unsigned long INT64_C;
typedef unsigned long UINT64_C;
typedef short SHORT;
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#endif

#if defined(__x86_64__) || defined(_M_X64)
typedef long long LLong;
typedef unsigned long long ULLong;
#else
typedef long LLong;
typedef unsigned long ULLong;
#endif


#ifdef ANDROID
#include <android/log.h>
#include <jni.h>
//#define max(a,b)    (((a) > (b)) ? (a) : (b))
//#define min(a,b)    (((a) < (b)) ? (a) : (b))
#else
#include <map>
#include <vector>
#include <list>
#endif


class CSDLog;
extern CSDLog g_mglog;

typedef int (*CallBack)(void);
typedef int (*CallBack1)(void*pParam);
typedef int (*CallBack2)(void*pParam, void *pObj);
typedef int (*CallBack3)(void*pParam, void *pPar1, void *pPar2);



#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(x) { if (x) { CloseHandle(x); x = NULL; } }
#endif
#ifndef SAFE_DEL
#define SAFE_DEL(x) { if (x) { delete x; x = NULL; } }
#endif
#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(x) { if (x) { delete[] x; x = NULL; } }
#endif
#ifndef SAFE_CLOSE_SOCKET
#define SAFE_CLOSE_SOCKET(x) { if (x) { closesocket(x); x = INVALID_SOCKET; } }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) {if(NULL!=x){free(x);x=NULL;}}
#endif


// 网络传输的发送和接受缓冲区大小
#define SD_SOCKET_RECIVE_BUF_SIZE			262144
#define SD_SOCKET_SEND_BUF_SIZE				262144

#define   MAX_UINT32						0xFFFFFFFF
#define   MAX_UINT64						0xFFFFFFFFFFFFFFFF

#define   MSG_CMD_RET_OFFSET                10000		// 同步命令的返回偏移值
#define   MAX_LEN_OUT_TIME                  5000


// 功能返回成功码
#define   RET_SUCCESS						0           // 执行成功
#define   RET_FAILED						-1			// 执行失败		
// 功能返回错误码(网络类)
#define   ERR_NET_OFF						-2			// 网络不通
#define   ERR_NET_TIME_OUT					-3			// 网络等待超时
#define   ERR_NET_SEND						-4			// 网络发送错误


#define   PACK_DATA_MAX_LEN					1048576

#define   PACK_HEAD_FLAGS					0x8BC67DF3
#define   PACK_TAIL_FLAGS					0x8AC67DFD
#define   PACK_ONE_BLOCK					0x9AC97DFD
#define   PACK_MEDL_BLOCK					0xDAC97CFD
#define   PACK_HANDLE_BLOCK					0xDAC97CFE
#define   PACK_SOCKET_CLOSE					0xDAB97CFE

//TCP信令接收状态
#define   TCP_MESSAGE_RECV_FAIL				-1
#define   TCP_MESSAGE_RECV_SUCCESS			0
#define   TCP_MESSAGE_RECV_OVER				1
#define   TCP_MESSAGE_RECV_INVALID			2

#define   SOCK_RECIVE_ERROR                 1
#define   SOCK_SEND_ERROR                   2
#define   SOCK_BEGINRECONNECT               3
#define   SOCK_WAIT_RECONNECT               4
#define   SOCK_FINISHRECONNECT              5

#ifndef WIN32
#define WM_USER     20000
#define MAX_PATH    256
#endif

#ifndef MYSOCKADDR
#define MYSOCKADDR
typedef struct sockaddr_in mysockaddr;
#endif

typedef struct
{
	USHORT  byCmdType;							// 消息的类型
	USHORT  byReserve;							// 保留位,暂未使用
	USHORT  byBroadcaseType;					// 广播消息的类型
	USHORT  shCmdSubMsg;                        // 执行的消息的子类型
	USHORT  shCmdMessage;                       // 执行的消息
	SHORT   shRetCode;                          // 执行返回码
	UINT    unPackPriority;						// 发送包的优先级
	UINT    unDataLen;							// 消息执行的数据位的长度
	UINT    unOther1;                           // 保留位1
	UINT    unOther2;                           // 保留位2
	UINT    unSrcAdress;                        // 消息发送的源IP地址
	UINT    unSrcUid;                           // 消息发送的源ID
	UINT    unDesUid;                           // 消息发送的目标ID
	BYTE    byData[PACK_DATA_MAX_LEN];          // 数据包指针
} PACK_MESSAGE;

#define PACK_HEAD_SIZE    (sizeof(PACK_MESSAGE) - PACK_DATA_MAX_LEN) // 数据包的包头长度
#define PACK_HEAD_LENGTH  (sizeof(PACK_MESSAGE) - PACK_DATA_MAX_LEN) // 数据包的包头长度


//获得中间件版本，用于服务器兼容性判定
UINT SD_GetCurrentBaseVersion();

//将一个字符串类型的IP地址转换成用一个整数类型表示的IP地址
void SD_IPStringToint(char *strbuf, UINT &nIp);

//将一个整数类型的IP地址转换成用一个字符串类型表示的IP地址
void SD_IPintToString(UINT nIp, char *strbuf);

//获取系统当前时间，精确到毫秒
long SD_GetTickCount(void);

//阻塞线程执行多长时间，时间单位为毫秒（跟系统的sleep类似）
void SD_Sleep(DWORD dwMilliseconds);

//获取某时间与当前时间的差
int  SD_GetTimeToCurrentTimeDifference(UINT tv_sec, UINT tv_usec);

//获取系统当前时间，tv_sec为时间部分，tv_usec为毫秒部分
void SD_GetCurrentTime(UINT &tv_sec, UINT &tv_usec);

//获取系统当前字符串类型的时间
void SD_GetCurrentStringTime(char *strTime);

//将字符串类型的时间转换成数字类型, 格式：年-月-日 时:分:秒:毫秒
void SD_StringToTime(char *strTime, UINT &tv_sec, UINT &tv_usec);

//将数字类型的时间转换成字符串类型
void SD_TimeToString(UINT tv_sec, UINT tv_usec, char *strTime);

//获取系统日期字符串
void SD_GetCurrentStringDate(char *strTime);

//将日期时间类型转化为字符串
void SD_DateToString(UINT tv_sec, UINT tv_usec, char *strTime);

//新建并复制PACK_MESSAGE回应包，申请的字节数按照nLen+头来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewLenRespondPackMessge(int nLen, USHORT shCmdMessage = 0);

//新建PACK_MESSAG回应E包，申请的字节数按照PACK_MESSAGE的大小来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewFullRespondPackMessge(USHORT shCmdMessage = 0);

//新建并复制PACK_MESSAGE包，申请的字节数按照nLen+头来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewLenPackMessge(int nLen, USHORT shCmdMessage = 0);

//新建PACK_MESSAGE包，申请的字节数按照PACK_MESSAGE的大小来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewFullPackMessge(USHORT shCmdMessage = 0);

//新建并复制PACK_MESSAGE包，申请的字节数按照实际占用的空间来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewCopyPackMessge(PACK_MESSAGE *pack);


//释放上述三个接口申请的内存空间
void FreePackMessge(PACK_MESSAGE *pack);

//取得与当前执行程序同名的不同扩展名的文件名
char* GetSameExeFile(char* strFileName, const char* strExtName);

//取得与当前执行程序的路径
char* GetExeFilePath(char* strFileName);


//创建指定文件夹,若已存在则返回成功, 支持相对路径，一次只支持创建一级路径
BOOL SD_CreateDir(const char *pstrDir);

//创建指定文件夹,若已存在则返回成功, 支持相对路径，支持创建多级路径
BOOL SD_CreateRecursionDir(const char *pstrDir);

//自定义随机数
void SD_srandom(unsigned int x);
long SD_random();
UINT SD_random32();

//根据目的IP获得本地出口IP
char* SD_GetExportIp(char *strRemoteIp);

#endif
