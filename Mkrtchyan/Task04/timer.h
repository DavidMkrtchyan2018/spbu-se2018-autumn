/* 
 * Code created by Edward Divanyan on the base of libutp/utp_utils.cpp
 * (see https://github.com/bittorrent/libutp/blob/master/utp_utils.cpp)
 * 
 */


#ifndef __TIMER_STRUCT_H__ED__
#define __TIMER_STRUCT_H__ED__

#include <stdint.h>

typedef uint64_t uint64;


void startTimer();
void resatTimer();
void printElapsedTime();
void printTime(uint64 elapsed);
uint64 getTime();
uint64 getStartTime();


#endif //__TIMER_STRUCT_H__ED__