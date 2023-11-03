//***************************************************************************//
//* 版权所有  www.mediapro.cc
//*
//* 内容摘要：跨平台的基础函数实现
//*	
//* 当前版本：V1.0		
//* 作    者：mediapro
//* 完成日期：2015-12-26
//**************************************************************************//
#include "SDCommon.h"
#include "SDLog.h"
#include "stdio.h"

#ifdef WIN32
//#define OS_WINDOWS WIN32
#include <windows.h>
#include <time.h>
#include <Tlhelp32.h>
#include <mmsystem.h>
#pragma comment( lib,"winmm.lib")
#include <direct.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#else
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <dirent.h>
#endif


UINT SD_GetCurrentBaseVersion()
{
	return 2019112801;
}

void SD_IPintToString(unsigned int nIp, char *strbuf)
{
	BYTE *byip = (BYTE*)&nIp;
	sprintf(strbuf, "%d.%d.%d.%d", byip[3], byip[2], byip[1], byip[0]);
}

void SD_IPStringToint(char *strbuf, unsigned int &nIp)
{
	BYTE *byip = (BYTE*)&nIp;
	int n1 = 0, n2 = 0, n3= 0, n4 = 0;
	sscanf(strbuf, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
	byip[0] = n4;
	byip[1] = n3;
	byip[2] = n2;
	byip[3] = n1;
}

void SD_Sleep(DWORD dwMilliseconds)
{
#ifdef WIN32
	timeBeginPeriod(1);
	Sleep(dwMilliseconds);
	timeEndPeriod(1);
#else
	// usleep(dwMilliseconds*1000);
	// POSIX has both a usleep() and a nanosleep(), but the former is deprecated,
	// so we use nanosleep() even though it has greater precision than necessary.
	struct timespec ts;
	ts.tv_sec = dwMilliseconds / 1000;
	ts.tv_nsec = (dwMilliseconds % 1000) * 1000000;
	int ret = nanosleep(&ts, NULL);
	if (ret != 0) 
	{
		static int nCount = 0;
		if ((nCount % 5000) == 0)
		{
			SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "nanosleep() returning early!!!");
		}
		nCount++;
		
		usleep(dwMilliseconds*1000);
	}
#endif
}


#ifdef WIN32
time_t SystemTimeToTime_t( const SYSTEMTIME& st )
{
    tm temptm = {st.wSecond, 
		st.wMinute, 
		st.wHour, 
		st.wDay, 
		st.wMonth - 1, 
		st.wYear - 1900, 
		st.wDayOfWeek, 
		0, 
		0};
    return mktime(&temptm);
}
#endif

#define ULONAG unsigned long


void SD_GetCurrentStringTime(char *strTime)
{
	UINT tv_sec = 0;
	UINT tv_usec = 0;
	SD_GetCurrentTime(tv_sec, tv_usec);
	SD_TimeToString(tv_sec, tv_usec, strTime);
}

void SD_StringToTime(char *strTime, UINT &tv_sec, UINT &tv_usec)
{
	tm tm1;
	sscanf(strTime,"%d-%d-%d %d:%d:%d:%d",&tm1.tm_year,&tm1.tm_mon,&tm1.tm_mday,&tm1.tm_hour,&tm1.tm_min,&tm1.tm_sec, &tv_usec);   
	tm1.tm_year -= 1900; 
	tm1.tm_mon -= 1; 
	tv_sec = (UINT)mktime(&tm1);
}



void SD_GetCurrentStringDate(char *strTime)
{
	UINT tv_sec = 0;
	UINT tv_usec = 0;
	SD_GetCurrentTime(tv_sec, tv_usec);
	SD_DateToString(tv_sec, tv_usec, strTime);
}
void SD_DateToString(UINT tv_sec, UINT tv_usec, char *strTime)
{
	struct tm *p;
	time_t timeData = tv_sec;
	p = localtime(&timeData);
	p->tm_year = p->tm_year + 1900;
	p->tm_mon = p->tm_mon + 1;
	sprintf(strTime, "%04d-%02d-%02d", p->tm_year, p->tm_mon, p->tm_mday);
}

void SD_TimeToString(UINT tv_sec, UINT tv_usec, char *strTime)
{
	struct tm *p;
	time_t timeData = tv_sec;
	p = localtime(&timeData);
	p->tm_year = p->tm_year + 1900;
	p->tm_mon = p->tm_mon + 1;
	sprintf(strTime, "%04d-%02d-%02d %02d:%02d:%02d:%02d"
		, p->tm_year, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv_usec);
}

//获取当前时间毫秒
void SD_GetCurrentTime(UINT &tv_sec, UINT &tv_msec)
{
#ifdef WIN32
	SYSTEMTIME   st; 
	GetLocalTime(&st);
	tv_sec = (UINT)SystemTimeToTime_t(st);
	tv_msec = st.wMilliseconds;
#else
	struct timeval tpstart;
	gettimeofday(&tpstart,NULL);
	tv_sec = tpstart.tv_sec;
	tv_msec = tpstart.tv_usec / 1000;
#endif
}

//获取当前时间毫秒
long SD_GetTickCount(void)
{
	long currentTime = 0;
#ifdef WIN32
	timeBeginPeriod(1);
	currentTime = timeGetTime();
	timeEndPeriod(1);
#else
#ifdef __APPLE__
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	currentTime = (long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
#endif
	return currentTime;
}


int  SD_GetTimeToCurrentTimeDifference(UINT tv_sec, UINT tv_usec)
{
	UINT lsec = 0;
	UINT lusec = 0;
	SD_GetCurrentTime(lsec, lusec);
	return (tv_sec-lsec)*1000+(tv_usec-lusec);
}


PACK_MESSAGE *NewCopyPackMessge(PACK_MESSAGE *pack)
{
	PACK_MESSAGE *pNewPack = (PACK_MESSAGE*)malloc(PACK_HEAD_SIZE+pack->unDataLen);
	memcpy(pNewPack, pack, PACK_HEAD_SIZE+pack->unDataLen);
	return pNewPack;
}

void FreePackMessge(PACK_MESSAGE *pack)
{
	if (pack != NULL)
	{
		free(pack);
	}

}


//新建并复制PACK_MESSAGE回应包，申请的字节数按照nLen+头来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewLenRespondPackMessge(int nLen, USHORT shCmdMessage)
{
	PACK_MESSAGE *pNewPack = (PACK_MESSAGE*)malloc(PACK_HEAD_SIZE+nLen);
	memset(pNewPack, 0, PACK_HEAD_SIZE+nLen);
	pNewPack->shCmdMessage = shCmdMessage+MSG_CMD_RET_OFFSET;
	pNewPack->unPackPriority = 2;
	pNewPack->unDataLen = nLen;
//	pNewPack->byRetMsg = TRUE;
	return pNewPack;
}


//新建PACK_MESSAGE回应包，申请的字节数按照PACK_MESSAGE的大小来算的，必须用FreePackMessge释放新建的包
PACK_MESSAGE *NewFullRespondPackMessge(USHORT shCmdMessage)
{
	PACK_MESSAGE *pNewPack = (PACK_MESSAGE*)malloc(sizeof(PACK_MESSAGE));
	memset(pNewPack, 0, sizeof(PACK_MESSAGE));
	pNewPack->shCmdMessage = shCmdMessage+MSG_CMD_RET_OFFSET;
	pNewPack->unPackPriority = 2;
//	pNewPack->byRetMsg = TRUE;
	return pNewPack;
}

PACK_MESSAGE *NewFullPackMessge(USHORT shCmdMessage)
{
	PACK_MESSAGE *pNewPack = (PACK_MESSAGE*)malloc(sizeof(PACK_MESSAGE));
	memset(pNewPack, 0, sizeof(PACK_MESSAGE));
	pNewPack->shCmdMessage = shCmdMessage;
	pNewPack->unPackPriority = 2;
	return pNewPack;
}

PACK_MESSAGE *NewLenPackMessge(int nLen, USHORT shCmdMessage)
{
	PACK_MESSAGE *pNewPack = (PACK_MESSAGE*)malloc(PACK_HEAD_SIZE+nLen);
	memset(pNewPack, 0, PACK_HEAD_SIZE+nLen);
	pNewPack->shCmdMessage = shCmdMessage;
	pNewPack->unPackPriority = 2;
	pNewPack->unDataLen = nLen;
	return pNewPack;
}

//***********************************************************************/
// 取得与当前执行程序同名的不同扩展名的文件名                           */
//***********************************************************************/
char* GetSameExeFile(char* strFileName, const char* strExtName)
{
	strFileName[0] = '\0';
#ifdef WIN32
	GetModuleFileNameA(NULL, strFileName, MAX_PATH);
#else

#ifndef ANDROID
	char szBuf[MAX_PATH];
	int nCount;
	memset(szBuf, 0, sizeof(szBuf));
	nCount=readlink("/proc/self/exe", szBuf, MAX_PATH);
	if(nCount<0 || nCount>=MAX_PATH)
	{
		return NULL;
	}
	szBuf[nCount]='\0';
	strcpy(strFileName, szBuf);
#else
	strcpy(strFileName, "/sdcard/mediapro");
#endif

#endif
	char* pstr = strrchr(strFileName, '.');
	if (pstr)
	{
		*pstr = 0;
	}			
	return strcat(strFileName, strExtName);
}

char* GetExeFilePath(char* strFileName)
{
	strFileName[0] = '\0';
#ifdef WIN32
	GetModuleFileNameA(NULL, strFileName, MAX_PATH);
#else

#ifndef ANDROID
	char szBuf[MAX_PATH];
	int nCount;
	memset(szBuf, 0, sizeof(szBuf));
	nCount=readlink("/proc/self/exe", szBuf, MAX_PATH);
	if(nCount<0 || nCount>=MAX_PATH)
	{
		return NULL;
	}
	szBuf[nCount]='\0';
	strcpy(strFileName, szBuf);
#else
	strcpy(strFileName, "/sdcard/mediapro");
#endif

#endif
	char* pstr = strrchr(strFileName, '.');
	if (pstr == NULL)
		pstr = strFileName + strlen(strFileName) -1;
	while ( (pstr >= strFileName) && (*pstr != '\\') && (*pstr != '/') ) pstr--;
	if (pstr)
	{
		pstr[1] = '\0';
	}
	return strFileName;
}


BOOL SD_CreateDir(const char *pstrDir)
{
	BOOL bCreateRet = FALSE;

#ifdef WIN32

	if (0 != _access(pstrDir, 0))
	{
		if (ENOENT == errno)
		{
			if (-1 == _mkdir(pstrDir))
			{
				SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Create Dir:%s failed, error:%d!!!", pstrDir, errno);
				bCreateRet = FALSE;
			}
			else
			{
				bCreateRet = TRUE;
			}

		}
		else
		{
			SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Create Dir:%s failed, error:%d!!!", pstrDir, errno);
			bCreateRet = FALSE;
		}
	}
	else
	{
		//文件夹已存在
		bCreateRet = TRUE;
	}

#else
	struct stat statbuf;
	if (0 != stat(pstrDir, &statbuf))  
	{
		if (ENOENT == errno)
		{
			//文件夹不存在，创建
			if (-1 == mkdir(pstrDir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IXOTH))
			{
				SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Create Dir:%s failed, error:%d!!!", pstrDir, errno);
				bCreateRet = FALSE;
			}
			else
			{
#if 0
				if (-1 == chmod("./myfolder", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IXOTH)) 
				{
					SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Create Dir:%s failed, error:%d!!!", pstrDir, errno);
					bCreateRet = FALSE;
				}
				else
				{
					bCreateRet = TRUE;
				}
#else
				bCreateRet = TRUE;
#endif

			}

		}
		else
		{
			SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Create Dir:%s failed, error:%d!!!", pstrDir, errno);
			bCreateRet = FALSE;
		}
	}
	else
	{
		//文件夹已存在
		bCreateRet = TRUE;
	}
#endif

	return bCreateRet;
}

static void fixPath(std::string &path)
{
	if (path.empty()){return;}
	for (std::string::iterator iter = path.begin(); iter != path.end(); ++iter)
	{
		if (*iter == '\\'){*iter = '/';}
	}
	if (path.at(path.length()-1) != '/'){path.append("/");}
}


BOOL SD_IsDirectory(std::string path)
{
#ifdef WIN32
	return PathIsDirectoryA(path.c_str()) ? TRUE : FALSE;
#else
	DIR * pdir = opendir(path.c_str());
	if (pdir == NULL)
	{
		return FALSE;
	}
	else
	{
		closedir(pdir);
		pdir = NULL;
		return TRUE;
	}
#endif
}


//递归创建多层次目录
BOOL SD_CreateRecursionDir(const char *pstrDir)
{
	if (pstrDir == NULL)
	{
		SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "SD_CreateRecursionDir failed point null!");
		return FALSE;
	}

	std::string path = pstrDir;
	if (path.length() == 0) 
	{	
		SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "SD_CreateRecursionDir failed path invalid!");
		return FALSE;
	}

	if (SD_IsDirectory(path))
	{
		//目录已经存在
		return TRUE;
	}

	std::string sub;
	fixPath(path);

	std::string::size_type pos = path.find('/');
	while (pos != std::string::npos)
	{
		std::string cur = path.substr(0, pos-0);
		if (cur.length() > 0 && !SD_IsDirectory(cur))
		{
			BOOL ret = FALSE;
#ifdef WIN32
			ret = CreateDirectoryA(cur.c_str(), NULL) ? TRUE : FALSE;
#else
			ret = (mkdir(cur.c_str(), S_IRWXU|S_IRWXG|S_IRWXO) == 0);
#endif
			if (!ret)
			{
				SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "SD_CreateRecursionDir:%s failed error:%d!", cur.c_str(), errno);
				return FALSE;
			}
		}
		pos = path.find('/', pos+1);
	}

	return TRUE;
}

//自定义随机数, from live555
#ifdef USE_SYSTEM_RANDOM
/* Use the system-supplied "random()" and "srandom()" functions */
#include <stdlib.h>
long SD_random() 
{
#if defined(__WIN32__) || defined(_WIN32)
  return rand();
#else
  return random();
#endif
}
void SD_srandom(unsigned int x) 
{
#if defined(__WIN32__) || defined(_WIN32)
  srand(x);
#else
  srandom(x);
#endif
}

#else

/* Use our own implementation of the "random()" and "srandom()" functions */
/*
 * random.c:
 *
 * An improved random number generation package.  In addition to the standard
 * rand()/srand() like interface, this package also has a special state info
 * interface.  The our_initstate() routine is called with a seed, an array of
 * bytes, and a count of how many bytes are being passed in; this array is
 * then initialized to contain information for random number generation with
 * that much state information.  Good sizes for the amount of state
 * information are 32, 64, 128, and 256 bytes.  The state can be switched by
 * calling the our_setstate() routine with the same array as was initiallized
 * with our_initstate().  By default, the package runs with 128 bytes of state
 * information and generates far better random numbers than a linear
 * congruential generator.  If the amount of state information is less than
 * 32 bytes, a simple linear congruential R.N.G. is used.
 *
 * Internally, the state information is treated as an array of longs; the
 * zeroeth element of the array is the type of R.N.G. being used (small
 * integer); the remainder of the array is the state information for the
 * R.N.G.  Thus, 32 bytes of state information will give 7 longs worth of
 * state information, which will allow a degree seven polynomial.  (Note:
 * the zeroeth word of state information also has some other information
 * stored in it -- see our_setstate() for details).
 *
 * The random number generation technique is a linear feedback shift register
 * approach, employing trinomials (since there are fewer terms to sum up that
 * way).  In this approach, the least significant bit of all the numbers in
 * the state table will act as a linear feedback shift register, and will
 * have period 2^deg - 1 (where deg is the degree of the polynomial being
 * used, assuming that the polynomial is irreducible and primitive).  The
 * higher order bits will have longer periods, since their values are also
 * influenced by pseudo-random carries out of the lower bits.  The total
 * period of the generator is approximately deg*(2**deg - 1); thus doubling
 * the amount of state information has a vast influence on the period of the
 * generator.  Note: the deg*(2**deg - 1) is an approximation only good for
 * large deg, when the period of the shift register is the dominant factor.
 * With deg equal to seven, the period is actually much longer than the
 * 7*(2**7 - 1) predicted by this formula.
 */

/*
 * For each of the currently supported random number generators, we have a
 * break value on the amount of state information (you need at least this
 * many bytes of state info to support this random number generator), a degree
 * for the polynomial (actually a trinomial) that the R.N.G. is based on, and
 * the separation between the two lower order coefficients of the trinomial.
 */
#define	TYPE_0		0		/* linear congruential */
#define	BREAK_0		8
#define	DEG_0		0
#define	SEP_0		0

#define	TYPE_1		1		/* x**7 + x**3 + 1 */
#define	BREAK_1		32
#define	DEG_1		7
#define	SEP_1		3

#define	TYPE_2		2		/* x**15 + x + 1 */
#define	BREAK_2		64
#define	DEG_2		15
#define	SEP_2		1

#define	TYPE_3		3		/* x**31 + x**3 + 1 */
#define	BREAK_3		128
#define	DEG_3		31
#define	SEP_3		3

#define	TYPE_4		4		/* x**63 + x + 1 */
#define	BREAK_4		256
#define	DEG_4		63
#define	SEP_4		1

/*
 * Array versions of the above information to make code run faster --
 * relies on fact that TYPE_i == i.
 */
#define	MAX_TYPES	5		/* max number of types above */

static int const degrees[MAX_TYPES] = { DEG_0, DEG_1, DEG_2, DEG_3, DEG_4 };
static int const seps [MAX_TYPES] = { SEP_0, SEP_1, SEP_2, SEP_3, SEP_4 };

/*
 * Initially, everything is set up as if from:
 *
 *	our_initstate(1, &randtbl, 128);
 *
 * Note that this initialization takes advantage of the fact that srandom()
 * advances the front and rear pointers 10*rand_deg times, and hence the
 * rear pointer which starts at 0 will also end up at zero; thus the zeroeth
 * element of the state information, which contains info about the current
 * position of the rear pointer is just
 *
 *	MAX_TYPES * (rptr - state) + TYPE_3 == TYPE_3.
 */

#if 1
static long randtbl[DEG_3 + 1] = {
	TYPE_3,
	static_cast<long>(0x9a319039),
    static_cast<long>(0x32d9c024),
    static_cast<long>(0x9b663182),
    static_cast<long>(0x5da1f342),
    static_cast<long>(0xde3b81e0),
    static_cast<long>(0xdf0a6fb5),
	static_cast<long>(0xf103bc02),
    static_cast<long>(0x48f340fb),
    static_cast<long>(0x7449e56b),
    static_cast<long>(0xbeb1dbb0),
    static_cast<long>(0xab5c5918),
    static_cast<long>(0x946554fd),
	static_cast<long>(0x8c2e680f),
    static_cast<long>(0xeb3d799f),
    static_cast<long>(0xb11ee0b7),
    static_cast<long>(0x2d436b86),
    static_cast<long>(0xda672e2a),
    static_cast<long>(0x1588ca88),
	static_cast<long>(0xe369735d),
    static_cast<long>(0x904f35f7),
    static_cast<long>(0xd7158fd6),
    static_cast<long>(0x6fa6f051),
    static_cast<long>(0x616e6b96),
    static_cast<long>(0xac94efdc),
	static_cast<long>(0x36413f93),
    static_cast<long>(0xc622c298),
    static_cast<long>(0xf5a42ab8),
    static_cast<long>(0x8a88d77b),
    static_cast<long>(0xf5ad9d0e),
    static_cast<long>(0x8999220b),
	static_cast<long>(0x27fb47b9),
};
#else
static long randtbl[DEG_3 + 1] = {
	TYPE_3,
	0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342, 0xde3b81e0, 0xdf0a6fb5,
	0xf103bc02, 0x48f340fb, 0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd,
	0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86, 0xda672e2a, 0x1588ca88,
	0xe369735d, 0x904f35f7, 0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc,
	0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b, 0xf5ad9d0e, 0x8999220b,
	0x27fb47b9,
};
#endif
/*
 * fptr and rptr are two pointers into the state info, a front and a rear
 * pointer.  These two pointers are always rand_sep places aparts, as they
 * cycle cyclically through the state information.  (Yes, this does mean we
 * could get away with just one pointer, but the code for random() is more
 * efficient this way).  The pointers are left positioned as they would be
 * from the call
 *
 *	our_initstate(1, randtbl, 128);
 *
 * (The position of the rear pointer, rptr, is really 0 (as explained above
 * in the initialization of randtbl) because the state table pointer is set
 * to point to randtbl[1] (as explained below).
 */
static long* fptr = &randtbl[SEP_3 + 1];
static long* rptr = &randtbl[1];

/*
 * The following things are the pointer to the state information table, the
 * type of the current generator, the degree of the current polynomial being
 * used, and the separation between the two pointers.  Note that for efficiency
 * of random(), we remember the first location of the state information, not
 * the zeroeth.  Hence it is valid to access state[-1], which is used to
 * store the type of the R.N.G.  Also, we remember the last location, since
 * this is more efficient than indexing every time to find the address of
 * the last element to see if the front and rear pointers have wrapped.
 */
static long *state = &randtbl[1];
static int rand_type = TYPE_3;
static int rand_deg = DEG_3;
static int rand_sep = SEP_3;
static long* end_ptr = &randtbl[DEG_3 + 1];

/*
 * srandom:
 *
 * Initialize the random number generator based on the given seed.  If the
 * type is the trivial no-state-information type, just remember the seed.
 * Otherwise, initializes state[] based on the given "seed" via a linear
 * congruential generator.  Then, the pointers are set to known locations
 * that are exactly rand_sep places apart.  Lastly, it cycles the state
 * information a given number of times to get rid of any initial dependencies
 * introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
 * for default usage relies on values produced by this routine.
 */
long SD_random(void); /*forward*/
void SD_srandom(unsigned int x)
{
	register int i;

	if (rand_type == TYPE_0)
		state[0] = x;
	else {
		state[0] = x;
		for (i = 1; i < rand_deg; i++)
			state[i] = 1103515245 * state[i - 1] + 12345;
		fptr = &state[rand_sep];
		rptr = &state[0];
		for (i = 0; i < 10 * rand_deg; i++)
			(void)SD_random();
	}
}

/*
 * our_initstate:
 *
 * Initialize the state information in the given array of n bytes for future
 * random number generation.  Based on the number of bytes we are given, and
 * the break values for the different R.N.G.'s, we choose the best (largest)
 * one we can and set things up for it.  srandom() is then called to
 * initialize the state information.
 *
 * Note that on return from srandom(), we set state[-1] to be the type
 * multiplexed with the current value of the rear pointer; this is so
 * successive calls to our_initstate() won't lose this information and will be
 * able to restart with our_setstate().
 *
 * Note: the first thing we do is save the current state, if any, just like
 * our_setstate() so that it doesn't matter when our_initstate is called.
 *
 * Returns a pointer to the old state.
 */
char *our_initstate(unsigned int seed, char *arg_state, int n)
{
	register char *ostate = (char *)(&state[-1]);

	if (rand_type == TYPE_0)
		state[-1] = rand_type;
	else
		state[-1] = MAX_TYPES * (long)(rptr - state) + rand_type;
	if (n < BREAK_0) {
#ifdef DEBUG
		(void)fprintf(stderr,
		    "random: not enough state (%d bytes); ignored.\n", n);
#endif
		return(0);
	}
	if (n < BREAK_1) {
		rand_type = TYPE_0;
		rand_deg = DEG_0;
		rand_sep = SEP_0;
	} else if (n < BREAK_2) {
		rand_type = TYPE_1;
		rand_deg = DEG_1;
		rand_sep = SEP_1;
	} else if (n < BREAK_3) {
		rand_type = TYPE_2;
		rand_deg = DEG_2;
		rand_sep = SEP_2;
	} else if (n < BREAK_4) {
		rand_type = TYPE_3;
		rand_deg = DEG_3;
		rand_sep = SEP_3;
	} else {
		rand_type = TYPE_4;
		rand_deg = DEG_4;
		rand_sep = SEP_4;
	}
	state = &(((long *)arg_state)[1]);	/* first location */
	end_ptr = &state[rand_deg];	/* must set end_ptr before srandom */
	SD_srandom(seed);
	if (rand_type == TYPE_0)
		state[-1] = rand_type;
	else
		state[-1] = MAX_TYPES*(long)(rptr - state) + rand_type;
	return(ostate);
}

/*
 * our_setstate:
 *
 * Restore the state from the given state array.
 *
 * Note: it is important that we also remember the locations of the pointers
 * in the current state information, and restore the locations of the pointers
 * from the old state information.  This is done by multiplexing the pointer
 * location into the zeroeth word of the state information.
 *
 * Note that due to the order in which things are done, it is OK to call
 * our_setstate() with the same state as the current state.
 *
 * Returns a pointer to the old state information.
 */
char *our_setstate(char *arg_state)
{
	register long *new_state = (long *)arg_state;
	register int type = new_state[0] % MAX_TYPES;
	register int rear = new_state[0] / MAX_TYPES;
	char *ostate = (char *)(&state[-1]);

	if (rand_type == TYPE_0)
		state[-1] = rand_type;
	else
		state[-1] = MAX_TYPES * (long)(rptr - state) + rand_type;
	switch(type) {
	case TYPE_0:
	case TYPE_1:
	case TYPE_2:
	case TYPE_3:
	case TYPE_4:
		rand_type = type;
		rand_deg = degrees[type];
		rand_sep = seps[type];
		break;
	default:
#ifdef DEBUG
		(void)fprintf(stderr,
		    "random: state info corrupted; not changed.\n");
#endif
		break;
	}
	state = &new_state[1];
	if (rand_type != TYPE_0) {
		rptr = &state[rear];
		fptr = &state[(rear + rand_sep) % rand_deg];
	}
	end_ptr = &state[rand_deg];		/* set end_ptr too */
	return(ostate);
}

/*
 * random:
 *
 * If we are using the trivial TYPE_0 R.N.G., just do the old linear
 * congruential bit.  Otherwise, we do our fancy trinomial stuff, which is
 * the same in all the other cases due to all the global variables that have
 * been set up.  The basic operation is to add the number at the rear pointer
 * into the one at the front pointer.  Then both pointers are advanced to
 * the next location cyclically in the table.  The value returned is the sum
 * generated, reduced to 31 bits by throwing away the "least random" low bit.
 *
 * Note: the code takes advantage of the fact that both the front and
 * rear pointers can't wrap on the same call by not testing the rear
 * pointer if the front one has wrapped.
 *
 * Returns a 31-bit random number.
 */
long SD_random() 
{
  long i;

  if (rand_type == TYPE_0) {
    i = state[0] = (state[0] * 1103515245 + 12345) & 0x7fffffff;
  } else {
    /* Make copies of "rptr" and "fptr" before working with them, in case we're being called concurrently by multiple threads: */
    long* rp = rptr;
    long* fp = fptr;

    /* Make sure "rp" and "fp" are separated by the correct distance (again, allowing for concurrent access): */
    if (!(fp == rp+SEP_3 || fp+DEG_3 == rp+SEP_3)) {
      /* A rare case that should occur only if we're being called concurrently by multiple threads. */
      /* Restore the proper separation between the pointers: */
      if (rp <= fp) rp = fp-SEP_3; else rp = fp+DEG_3-SEP_3;
    }

    *fp += *rp;
    i = (*fp >> 1) & 0x7fffffff;	/* chucking least random bit */
    if (++fp >= end_ptr) {
      fp = state;
      ++rp;
    } else if (++rp >= end_ptr) {
      rp = state;
    }

    /* Restore "rptr" and "fptr" from our working copies: */
    rptr = rp;
    fptr = fp;
  }

  return i;
}
#endif

UINT SD_random32() 
{
  /* Return a 32-bit random number.
     Because "our_random()" returns a 31-bit random number, we call it a second
     time, to generate the high bit.
     (Actually, to increase the likelhood of randomness, we take the middle 16 bits of two successive calls to "our_random()")
  */
  long random_1 = SD_random();
  UINT random16_1 = (UINT)(random_1&0x00FFFF00);

  long random_2 = SD_random();
  UINT random16_2 = (UINT)(random_2&0x00FFFF00);

  return (random16_1<<8) | (random16_2>>8);
}


//windows平台的类linux api:gettimeofday
#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		//converting file time to unix epoch
		tmpres -= DELTA_EPOCH_IN_MICROSECS; 
		tmpres /= 10;  //convert into microseconds
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif

void SD_CloseSock(int nsockId)
{
	if (nsockId)
	{
#ifdef WIN32
		shutdown(nsockId, SD_BOTH);
		closesocket(nsockId);
#else
		shutdown(nsockId, SHUT_RDWR);
		close(nsockId);
#endif
	}
}

char* SD_GetExportIp(char *strRemoteIp)
{
	int      sock_fd;
	struct sockaddr_in    addr4;
#ifdef __APPLE__
	unsigned int      len;
#else
#ifdef WIN32
	int      len;
#else
	socklen_t len;
#endif
#endif
	sock_fd = (int)socket(AF_INET, SOCK_DGRAM, 0);
	memset(&addr4, 0, sizeof(addr4));

	addr4.sin_addr.s_addr = inet_addr(strRemoteIp);
	addr4.sin_family = AF_INET;
	addr4.sin_port = htons(9990);

	int nRet = ::connect(sock_fd, (struct sockaddr *)&addr4, sizeof(struct sockaddr_in));
	if (nRet != 0)
	{
		SDLOG_PRINTF_U(0, SD_LOG_LEVEL_ERROR, "Get Export IP address failed!");
		SD_CloseSock(sock_fd);
		return NULL;
	}

	memset(&addr4, 0, sizeof(addr4));
	len =sizeof(addr4);
	getsockname(sock_fd, (struct sockaddr *)&addr4, &len);
	SD_CloseSock(sock_fd);
	return inet_ntoa(addr4.sin_addr);
}
