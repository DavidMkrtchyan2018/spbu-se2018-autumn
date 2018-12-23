#ifndef __HT_FUNCTIONS__H__
#define __HT_FUNCTIONS__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HT_MISSING 	(-3)
#define HT_FULL 	(-2)
#define HT_MEM_ERR 	(-1)
#define HT_OK 		(0)
#define HT_FOUND	(1)

#define OUT
#define IN_OUT


typedef unsigned int (*pfHash)(unsigned int, char*);
typedef int (*pfFind)(char*, char*);
typedef int (*pfCheck)(unsigned int, char*);


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

#endif //__HT_FUNCTIONS__H__
