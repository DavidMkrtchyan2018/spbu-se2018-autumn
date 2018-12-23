#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#define HASH_SZ 512
#define HT_MISSING 	(-3)
#define HT_FULL 	(-2)
#define HT_MEM_ERR 	(-1)
#define HT_OK 		(0)
#define HT_FOUND	(1)
#define OUT
#define IN_OUT
#define INITIAL_SIZE (256)

typedef uint64_t uint64;
typedef unsigned int (*pfHash)(unsigned int, char*);
typedef int (*pfFind)(char*, char*);
typedef int (*pfCheck)(unsigned int, char*);

void startTimer();
void resatTimer();
void printElapsedTime();
void printTime(uint64 elapsed);
void usage();
char* getFileContent(char* fileName, unsigned int* n);
char** getWordsFromText(char* text, unsigned int* n IN_OUT);
void deleteStringArr(char** arr, unsigned int nStrings);
void* htCreate(unsigned int, pfHash);
void htFree(void* hashtable);
int htPut(void* hashtable, char* key);
int htGet(void* hashtable, char* key, int* answer OUT);
int htIterate(void* hash, pfFind func, unsigned int minOcc, char* str, unsigned int* prnt);
int htCheck(void* hash, pfCheck func);
int htPrintStatistics(void* hashtable, unsigned int nWords);
unsigned int hash(unsigned int size, char* word);
int findSubstring(char* str, char* string);
int checkEntry(unsigned int val, char* str);
uint64 getTime();
uint64 getStartTime();



int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage();
        return 1;
    }

    unsigned int nWords = 0;
    char* text = getFileContent(argv[1], &nWords);

    printf("the file opened and read successfully, text length is %lu characters, about %u words found.\n", strlen(text), nWords);

    char** words = getWordsFromText(text, &nWords);
    // we don't need plain text anymore, let's delete it
    free(text);

    //printf("nWords = %u\n", nWords);
    unsigned int index = 0;
    //unsigned int N = ANUM < nWords ? ANUM : nWords;
    int retVal = 0;

    //void* hashT = htCreate(HASH_SZ, NULL);
    void* hashT = htCreate(HASH_SZ, hash);

    for(index = 0; index < nWords; index++)
    {
        retVal = htPut(hashT, words[index]);
        if(retVal != HT_OK)
        {
            printf("ERROR: something goes wrong\n");
            deleteStringArr(words, nWords);
            htFree(hashT);
            exit(1);
        }
    }

    deleteStringArr(words, nWords);
    retVal = htCheck(hashT, checkEntry);
    if(retVal != HT_OK)
    {
        printf("ERROR: htCheck not passed!\n");
        htFree(hashT);
        return 1;
    }

    printf("htCheck passed.\n");

    htPrintStatistics(hashT, nWords);

    char word[100] = "hello";
    int nOccurances = 0;

    while( 1 )
    {
        printf("Please enter a word to see if it was used in the book (0 to cancel):\n");
        scanf("%s", word);

        if( word[0] == '0' )
        {
            break;
        }

        retVal = htGet(hashT, word, &nOccurances);

        if(retVal == HT_OK)
        {
            printf("%s occurs in the given text %d times\n", word, nOccurances);
        }
        else
        {
            printf("%s not found in the given text\n", word);
        }
    }

    printf("Please enter some letters to see if they are words starting so : ");
    scanf("%s", word);
    printf("Please enter a number, indicating minimal occurrences of such words in the book to seek for : ");
    scanf("%d", &nOccurances);

    retVal = htIterate(hashT,
		      findSubstring,
		      nOccurances,
		      word,
		      &nWords);

    htFree(hashT);

    if( retVal != HT_OK )
    {
        printf("htIterate returns an error code: %d\n", retVal);
    }
    if( !nWords )
    {
        printf("No such words occur in the text more than %d times.\n", nOccurances);
    }

    printf("Buy-buy!\n");

    return 0;
}


unsigned int hash(unsigned int size, char* word)
{
    unsigned int hash = 0;
    for (int i = 0 ; word[i] != '\0' ; i++)
    {
        hash = 31*hash + word[i];
    }
    return hash % size;
}

int findSubstring(char* str, char* string)
{
    if( !str || !string )
      return HT_MISSING;

    size_t len1 = strlen(str);
    size_t len2 = strlen(string);
    if( !len1 || !len2 )
      return HT_MISSING;

    if( len1 > len2 )
      return HT_OK;

    for(size_t i = 0; i < len1; i++)
    {
        if( str[i] != string[i] )
          return HT_OK;
    }

    return HT_FOUND;
}

int checkEntry(unsigned int val, char* str)
{
    if( !str || !strlen(str) || !val )
      return HT_MISSING;
    return HT_OK;
}

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
// MSVC 6 standard doesn't like division with uint64s
static double counterPerMicrosecond;

static uint64 UTGetTickCount64()
{
	if (pt2GetTickCount64) {
		return pt2GetTickCount64();
	}
	if (pt2RealGetTickCount) {
		uint64 v = pt2RealGetTickCount();
		// fix return value from GetTickCount
		return (DWORD)v | ((v >> 0x18) & 0xFFFFFFFF00000000);
	}
	return (uint64)GetTickCount();
}

static void Time_Initialize()
{
	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	pt2GetTickCount64 = (GetTickCount64Proc*)GetProcAddress(kernel32, "GetTickCount64");
	// not a typo. GetTickCount actually returns 64 bits
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
	if (!time_init) {
		time_init = true;
		Time_Initialize();
	}

	uint64 counter;
	uint64 tick;

	QueryPerformanceCounter((LARGE_INTEGER*) &counter);
	tick = UTGetTickCount64();

	// unfortunately, QueryPerformanceCounter is not guaranteed
	// to be monotonic. Make it so.
	int64 ret = (int64)(((int64)counter - (int64)startPerformanceCounter) / counterPerMicrosecond);
	// if the QPC clock leaps more than one second off GetTickCount64()
	// something is seriously fishy. Adjust QPC to stay monotonic
	int64 tick_diff = tick - startGetTickCount;
	if (abs64(ret / 100000 - tick_diff / 100) > 10) {
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
	// http://developer.apple.com/mac/library/qa/qa2004/qa1398.html
	// http://www.macresearch.org/tutorial_performance_and_time
	static mach_timebase_info_data_t sTimebaseInfo;
	static uint64_t start_tick = 0;
	uint64_t tick;
	// Returns a counter in some fraction of a nanoseconds
	tick = mach_absolute_time();
	if (sTimebaseInfo.denom == 0) {
		// Get the timer ratio to convert mach_absolute_time to nanoseconds
		mach_timebase_info(&sTimebaseInfo);
		start_tick = tick;
	}
	// Calculate the elapsed time, convert it to microseconds and return it.
	return ((tick - start_tick) * sTimebaseInfo.numer) / (sTimebaseInfo.denom * 1000);
}

#else // !__APPLE__

#if ! (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC))
//    #warning "Using non-monotonic function gettimeofday() in UTP_GetMicroseconds()"
#endif

/* Unfortunately, #ifdef CLOCK_MONOTONIC is not enough to make sure that
   POSIX clocks work -- we could be running a recent libc with an ancient
   kernel (think OpenWRT). -- jch */

static uint64_t __GetMicroseconds()
{
	struct timeval tv;

#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC)
	static int have_posix_clocks = -1;
	int rc;

	if (have_posix_clocks < 0) {
		struct timespec ts;
		rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		if (rc < 0) {
			have_posix_clocks = 0;
		} else {
			have_posix_clocks = 1;
		}
	}

	if (have_posix_clocks) {
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

/*
 * Whew.  Okay.  After that #ifdef maze above, we now know we have a working
 * __GetMicroseconds() implementation on all platforms.
 *
 * Because there are a number of assertions in libutp that will cause a crash
 * if monotonic time isn't monotonic, now apply some safety checks.  While in
 * principle we're already protecting ourselves in cases where non-monotonic
 * time is likely to happen, this protects all versions.
 */

static inline uint64 UTP_GetMicroseconds()
{
	static uint64 offset = 0, previous = 0;
	uint64 now = __GetMicroseconds() + offset;

	if (previous > now) {
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
    printf("%u hours %u minutes %u seconds %u msecs %u usecs\n", hours, minutes, seconds, msecs, usecs);
  }
  else if( minutes )
  {
    printf("%u minutes %u seconds %u msecs %u usecs\n", minutes, seconds, msecs, usecs);
  }
  else if( seconds )
  {
    printf("%u seconds %u msecs %u usecs\n", seconds, msecs, usecs);
  }
  else if( msecs )
  {
    printf("%u msecs %u usecs\n", msecs, usecs);
  }
  else
  {
    printf("%u usecs\n", usecs);
  }
}

typedef struct _hashtableNode
{
    char* key;
    unsigned int value;
    struct _hashtableNode* next;
} hashtableNode;


typedef struct _hashtable
{
    unsigned int table_size;
    unsigned int usedNodes;
    pfHash hashFunc;
    hashtableNode* data;
} Hashtable;

  
static unsigned long crc32_tab[] = {
      0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
      0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
      0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
      0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
      0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
      0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
      0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
      0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
      0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
      0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
      0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
      0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
      0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
      0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
      0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
      0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
      0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
      0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
      0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
      0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
      0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
      0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
      0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
      0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
      0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
      0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
      0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
      0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
      0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
      0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
      0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
      0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
      0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
      0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
      0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
      0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
      0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
      0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
      0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
      0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
      0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
      0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
      0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
      0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
      0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
      0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
      0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
      0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
      0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
      0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
      0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
      0x2d02ef8dL
   };

/* Returns a 32- or 64-bit CRC of the contents of the string s.
 */
static unsigned long int crc(const unsigned char *s, unsigned int len)
{
  unsigned int i = 0;
  unsigned long int crc = 0;

  for (;  i < len;  i++)
  {
      crc = crc32_tab[(crc ^ s[i]) & 0xff] ^ (crc >> 8);
  }

  return crc;
}

/*
 * Hashing function for a string
 */
static unsigned int str2hash(unsigned int table_size, char* keystr)
{
    unsigned long int key = crc((unsigned char*)keystr, strlen(keystr));

    /* Robert Jenkins' 32 bit Mix Function */
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);

    /* Knuth's Multiplicative Method */
    key = (key >> 3) * 2654435761;

    return key % table_size;
}


/* 		OTHER FUNCTIONS		 */

void usage()
{
    printf("this program has to be called with 1 argument\n");
    printf("the argument must be file name to proceed\n");
}

char* getFileContent(char* fileName, unsigned int* n)
{
    FILE *fp;
    unsigned int count = 0;
    char ch;
    char* text = NULL;
    size_t nRead = 0;

    if ((fp = fopen(fileName, "r"))==NULL)
    {
        printf("Error opening file, %s\n", strerror(errno));
        return NULL;
    }

    while((ch=fgetc(fp)) != EOF)
    {
        count++;
        if(ch == ' ' || ch == '\n' || ch == '-')
        {
            (*n)++;
        }
    }

    text = (char*) malloc(count + 1);
    rewind(fp);
    nRead = fread(text, 1, count, fp);
    fclose(fp);

    if (nRead != count)
    {
        printf("Error reading file, %s, nRead = %lu, count = %d\n", strerror(errno), (long unsigned int)nRead, count);
        free(text);
        return NULL;
    }

    text[count] = '\0';

    return text;
}

void deleteStringArr(char** arr, unsigned int nStrings)
{
    unsigned int count = 0;
    for(; count < nStrings; count++)
    {
        if(arr[count])
        {
            free(arr[count]);
        }
    }
    free(arr);
}

static char** getWords(char* text, unsigned int* nWords)
{
    unsigned int N = *nWords;
    unsigned int strLen = strlen(text);
    unsigned int charsPassed = 0;
    char** arr = (char**) malloc(N * sizeof(char**));
    unsigned int count = 0;
    int nChars = 0;
    char* pChar = text;

    if(!arr)
    {
        printf("Memory allocation error 1\n");
        free(text);
        exit(1);
    }

    for(; count < N; count++)
    {
    	arr[count] = NULL;
    }

    for(count = 0; charsPassed < strLen && count < N; count++)
    {
        while( isalpha(*pChar) )
        {
            pChar++;
            if(*pChar == EOF)
            {
                break;
            }

            if(*pChar == '\'')
            {
                if( isalpha(*(pChar + 1)) )
                {
                    *(pChar + 1) = ' ';
                }
                break;
            }
            nChars++;
        }

        if( nChars )
        {// get only words with letters
            // create a buffer for zero-ending string with n Chars, save its address in arr[count]
            arr[count] = (char*) malloc(nChars + 1);
            if(!arr[count])
            {
                printf("Memory allocation error 2\n");
                deleteStringArr(arr, count - 1);
                free(text);
                exit(1);
            }
            // copy n Chars in it
            memcpy(arr[count], pChar - nChars, nChars);
            // and set zero insdead of \n
            arr[count][nChars] = '\0';
            //printf("str[%d]: %s nChars: %d\n", count, arr[count], nChars);
            charsPassed += nChars;
            nChars = 0;
        }
        else
        {
            count--;
        }

        if(*pChar == EOF)
        {
            break;
        }
        // go to the next of \n char
        pChar++;
        charsPassed++;
    }

    if(count < N)
    {
        printf("No enough words with letters found, working with %u ones\n", count);
        *nWords = count;
    }

    return arr;
}

char** getWordsFromText(char* text, unsigned int* n)
{
    unsigned int nStr = *n;
    char** arr = getWords(text, &nStr);
    *n = nStr;
    return arr;
}

void* htCreate(unsigned int sz, pfHash hashFunc)
{
    Hashtable* ht = (Hashtable*) malloc(sizeof(Hashtable));

    if( !ht )
    {
        return NULL;
    }

    ht->table_size = sz ? sz : INITIAL_SIZE;
    ht->usedNodes = 0;
    ht->hashFunc = hashFunc ? hashFunc : str2hash;
    ht->data = (hashtableNode*) calloc(ht->table_size, sizeof(hashtableNode));

    if(!ht->data)
    {
        htFree(ht);
        return NULL;
    }

    return ht;
}

static void freeList(hashtableNode* head)
{
    hashtableNode* tmp;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;

        free(tmp->key);
        free(tmp);
    }
}

void htFree(void* hashtable)
{
    Hashtable* ht = (Hashtable*) hashtable;
    unsigned int sz = ht->table_size;
    unsigned int index = 0;

    for(; index < sz; index++)
    {// delete lists
        if(ht->data[index].value)
        {
            freeList(ht->data[index].next);
        }
    }

    free(ht->data);
    free(ht);
}

static hashtableNode* nodeWithString(char* key)
{
    hashtableNode* node = (hashtableNode*) calloc(1, sizeof(hashtableNode));
    if(!node)
    {
        printf("Memory ERROR\n");
        return NULL;
    }
    node->key = (char*) malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = 1;
    return node;
}

int htPut(void* hashtable,
	  char* key)
{
    Hashtable* ht = (Hashtable*) hashtable;
    unsigned int index = ht->hashFunc(ht->table_size, key);
    unsigned int nDiffWords = ht->data[index].value;

    if(!nDiffWords)
    {
        ht->data[index].next = nodeWithString(key);
        if(!ht->data[index].next)
        {
            printf("ERROR\n");
            return HT_MEM_ERR;
        }
        ht->data[index].value++;
        ht->usedNodes++;
        return HT_OK;
    }

    hashtableNode* tmp = ht->data[index].next;
    for(unsigned i = 0; i < nDiffWords; i++)
    {
        if(strcmp(tmp->key, key) == 0)
        {
            tmp->value++;
            return HT_OK;
        }

        if(tmp->next)
          tmp = tmp->next;
        else
          break;
    }

    tmp->next = nodeWithString(key);
    if(!tmp->next)
    {
        printf("htPut Memory ERROR\n");
        return HT_MEM_ERR;
    }

    ht->data[index].value++;
    ht->usedNodes++;

    return HT_OK;
}

int htGet(void* hashtable,
          char* key,
          int* answer OUT)
{
    Hashtable* ht = (Hashtable*) hashtable;
    unsigned int index = ht->hashFunc(ht->table_size, key);
    unsigned int nDiffWords = ht->data[index].value;

    if(!nDiffWords)
    {
        return HT_MISSING;
    }

    hashtableNode* tmp = ht->data[index].next;
    for(unsigned i = 0; i < nDiffWords; i++)
    {
        if(strcmp(tmp->key, key) == 0)
        {
            *answer = tmp->value;
            return HT_OK;
        }
        if(tmp->next)
          tmp = tmp->next;
        else
          break;
    }

    return HT_MISSING;
}

int htIterate(void* hash,
              pfFind func,
              unsigned int minOcc,
              char* str,
              unsigned int* prnt)
{
    Hashtable* ht = (Hashtable*) hash;
    unsigned int tableSize = ht->table_size;
    hashtableNode* arrHeads = ht->data;
    hashtableNode* tmp = NULL;
    unsigned int index = 0;
    int retVal = 0;
    *prnt = 0;

    for(; index < tableSize; index++)
    {
        if( arrHeads[index].value )
        {// entering to list
            tmp = arrHeads[index].next;
            while( tmp )
            {
                retVal = func(str, tmp->key);
                if( retVal == HT_MISSING )
                {
                    return HT_MISSING;
                }
                else
                {
                    if( retVal == HT_FOUND )
                    {
                        if( tmp->value >= minOcc )
                        {
                            printf("%s occurs %u times\n", tmp->key, tmp->value);
                            (*prnt)++;
                        }
                    }
                    tmp = tmp->next;
                }
            }
        }
    }

    return HT_OK;
}

int htCheck(void* hash, pfCheck func)
{
    Hashtable* ht = (Hashtable*) hash;
    unsigned int tableSize = ht->table_size;
    hashtableNode* arrHeads = ht->data;
    hashtableNode* tmp = NULL;
    unsigned int index = 0;
    int retVal = 0;

    for(; index < tableSize; index++)
    {
        if( arrHeads[index].value )
        {// entering to list
            tmp = arrHeads[index].next;
            while( tmp )
            {
                retVal = func(tmp->value, tmp->key);
                if( retVal != HT_OK )
                {
                    return retVal;
                }
                tmp = tmp->next;
            }
        }
    }

    return HT_OK;
}

static void htGetMaxOccuranceEntry(Hashtable* hash,
                                  unsigned int* nMax,
                                  char** strMax)
{
    unsigned int tableSize = hash->table_size;
    hashtableNode* arrHeads = hash->data;
    hashtableNode* tmp = NULL;
    unsigned int index = 0;

    for(; index < tableSize; index++)
    {
        if( arrHeads[index].value )
        {// entering to list
            tmp = arrHeads[index].next;
            while( tmp )
            {
                if( tmp->value > *nMax )
                {
                    *nMax = tmp->value;
                    *strMax = tmp->key;
                }
                tmp = tmp->next;
            }
        }
    }
}

int htPrintStatistics(void* hashtable, unsigned int nWords)
{
    Hashtable* ht = (Hashtable*) hashtable;
    unsigned int tableSize = ht->table_size;
    unsigned int usedNodes = ht->usedNodes;
    hashtableNode* arrHeads = ht->data;
    unsigned int nMax = 0;
    char* strMax = NULL;
    unsigned int index = 0;
    char yn = 'a';

    printf("Hash table size = %u\nNumber of different words in the Hash table is %u\n", tableSize, usedNodes);
    printf("Average chain length is %.2f\n", (float)usedNodes/(float)tableSize);
    printf("Average word re-usage coefficient is %.2f\n", (float)nWords/(float)usedNodes);

    startTimer();
    htGetMaxOccuranceEntry(ht, &nMax, &strMax);
    uint64 end = getTime();
    printf("Maximal occurrence of %u has word \'%s\'; calculated in %lu usecs\n", nMax, strMax, end);
    printf("Do you want to see chain length distribution? (y/n) :");
    scanf("%c", &yn);

    if( yn == 'y' )
    {
        printf("Chain lengths in the Hash table are the following:\n");
        for(; index < tableSize; index++)
        {
            printf("Chain[%u] -> %2u\t", index, arrHeads[index].value);
            if( index % 5 == 0 )
              printf("\n");
        }
    }

    printf("\n");
    return HT_OK;
}
