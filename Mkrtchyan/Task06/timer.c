#include <stdlib.h>
#include <assert.h>
#include "timer.h"
//#include "utp.h"
//#include "utp_types.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else //!WIN32
	#include <time.h>
	#include <sys/time.h>		// Linux needs both time.h and sys/time.h
#endif

#if defined(__APPLE__)
	#include <mach/mach_time.h>
#endif

//#include "utp_utils.h"

#ifdef WIN32

typedef ULONGLONG (WINAPI GetTickCount64Proc)(void);
static GetTickCount64Proc *pt2GetTickCount64;
static GetTickCount64Proc *pt2RealGetTickCount;

static uint64 startPerformanceCounter;
static uint64 startGetTickCount;
static double counterPerMicrosecond;

static uint64 UTGetTickCount64()
{
	if (pt2GetTickCount64)
    {
		return pt2GetTickCount64();
	}
	if (pt2RealGetTickCount)
    {
		uint64 v = pt2RealGetTickCount();
		return (DWORD)v | ((v >> 0x18) & 0xFFFFFFFF00000000);
	}
	return (uint64)GetTickCount();
}

static void Time_Initialize()
{
	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	pt2GetTickCount64 = (GetTickCount64Proc*)GetProcAddress(kernel32, "GetTickCount64");
	pt2RealGetTickCount = (GetTickCount64Proc*)GetProcAddress(kernel32, "GetTickCount");

	uint64 frequency;
	QueryPerformanceCounter((LARGE_INTEGER*)&startPerformanceCounter);
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	counterPerMicrosecond = (double)frequency / 1000000.0f;
	startGetTickCount = UTGetTickCount64();
}

static int64 abs64(int64 x) { return x < 0 ? -x : x; }

static uint64 __GetMicroseconds()
{
	static bool time_init = false;
	if (!time_init)
    {
		time_init = true;
		Time_Initialize();
	}

	uint64 counter;
	uint64 tick;

	QueryPerformanceCounter((LARGE_INTEGER*) &counter);
	tick = UTGetTickCount64();
	int64 ret = (int64)(((int64)counter - (int64)startPerformanceCounter) / counterPerMicrosecond);
	int64 tick_diff = tick - startGetTickCount;
	if (abs64(ret / 100000 - tick_diff / 100) > 10)
    {
		startPerformanceCounter -= (uint64)((int64)(tick_diff * 1000 - ret) * counterPerMicrosecond);
		ret = (int64)((counter - startPerformanceCounter) / counterPerMicrosecond);
	}
	return ret;
}

static inline uint64 UTP_GetMilliseconds()
{
	return GetTickCount();
}

#else //!WIN32

static inline uint64 UTP_GetMicroseconds(void);
static inline uint64 UTP_GetMilliseconds()
{
	return UTP_GetMicroseconds() / 1000;
}

#if defined(__APPLE__)

static uint64 __GetMicroseconds()
{
	static mach_timebase_info_data_t sTimebaseInfo;
	static uint64_t start_tick = 0;
	uint64_t tick;
	tick = mach_absolute_time();
	if (sTimebaseInfo.denom == 0)
    {
		mach_timebase_info(&sTimebaseInfo);
		start_tick = tick;
	}
	return ((tick - start_tick) * sTimebaseInfo.numer) / (sTimebaseInfo.denom * 1000);
}

#else // !__APPLE__

#if ! (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC))
    #warning "Using non-monotonic function gettimeofday() in UTP_GetMicroseconds()"
#endif


static uint64_t __GetMicroseconds()
{
	struct timeval tv;

#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC)
	static int have_posix_clocks = -1;
	int rc;

	if (have_posix_clocks < 0)
    {
		struct timespec ts;
		rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		if (rc < 0) {
			have_posix_clocks = 0;
		}
		else
        {
			have_posix_clocks = 1;
		}
	}

	if (have_posix_clocks)
    {
		struct timespec ts;
		rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		return uint64(ts.tv_sec) * 1000000 + uint64(ts.tv_nsec) / 1000;
	}
#endif

	gettimeofday(&tv, NULL);
	return (uint64) tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif //!__APPLE__

#endif //!WIN32

static inline uint64 UTP_GetMicroseconds()
{
	static uint64 offset = 0, previous = 0;
	uint64 now = __GetMicroseconds() + offset;
	
	if (previous > now)
    {
		/* Eek! */
		offset += previous - now;
		now = previous;
	}
	previous = now;
	return now;
}

uint64 get_milliseconds() 
{
	return UTP_GetMilliseconds();
}

uint64 get_microseconds() 
{
	return UTP_GetMicroseconds();
}

static inline uint64 get_time_us(int a)
{
    static uint64 previous = 0;
    uint64 now = UTP_GetMicroseconds();

    switch (a)
    {
        case 0:
            previous = now;
            return 0;
        case 1:
            return now - previous;
        case 2:
            return previous;
        default:
            break;
    }
    return 0;
}

void startTimer()
{
    get_time_us(0);
}
uint64 getTime()
{
    return get_time_us(1);
}
uint64 getStartTime()
{
    return get_time_us(2);
}

void resatTimer()
{
    get_time_us(0);
}

#include <stdio.h>

void printElapsedTime()
{
    uint64 elapsed = get_time_us(1);
    unsigned int hours = elapsed / 3600000000;
    elapsed -= hours * 3600000000;
    unsigned int minutes = elapsed / 60000000;
    elapsed -= minutes * 60000000;
    unsigned int seconds = elapsed / 1000000;
    elapsed -= seconds * 1000000;
    unsigned int msecs = elapsed / 1000;
    elapsed -= msecs * 1000;
    unsigned int usecs = elapsed;

    if( hours )
    {
        printf("%u hours %u minutes %u seconds %u msecs %u usecs elapsed\n", hours, minutes, seconds, msecs, usecs);
    }
    else if( minutes )
    {
        printf("%u minutes %u seconds %u msecs %u usecs elapsed\n", minutes, seconds, msecs, usecs);
    }
    else if( seconds )
    {
        printf("%u seconds %u msecs %u usecs elapsed\n", seconds, msecs, usecs);
    }
    else if( msecs )
    {
        printf("%u msecs %u usecs elapsed\n", msecs, usecs);
    }
    else 
    {
        printf("%u usecs elapsed\n", usecs);
    }
}

void printTime(uint64 elapsed)
{
    unsigned int hours = elapsed / 3600000000;
    elapsed -= hours * 3600000000;
    unsigned int minutes = elapsed / 60000000;
    elapsed -= minutes * 60000000;
    unsigned int seconds = elapsed / 1000000;
    elapsed -= seconds * 1000000;
    unsigned int msecs = elapsed / 1000;
    elapsed -= msecs * 1000;
    unsigned int usecs = elapsed;

    if( hours )
    {
        printf("%u hours %u minutes %u seconds %u msecs %u usecs elapsed\n", hours, minutes, seconds, msecs, usecs);
    }
    else if( minutes )
    {
        printf("%u minutes %u seconds %u msecs %u usecs elapsed\n", minutes, seconds, msecs, usecs);
    }
    else if( seconds )
    {
        printf("%u seconds %u msecs %u usecs elapsed\n", seconds, msecs, usecs);
    }
    else if( msecs )
    {
        printf("%u msecs %u usecs elapsed\n", msecs, usecs);
    }
    else 
    {
        printf("%u usecs elapsed\n", usecs);
    }
}
