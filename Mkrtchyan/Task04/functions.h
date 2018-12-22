#ifndef __FUNCTIONS__H__
#define __FUNCTIONS__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


void usage();
int isInt(char* str);
void bubbleSort(char* text, unsigned int n);
void insertionSort(char* text, unsigned int n);
void mergeSort(char* text, unsigned int n);
void quickSort(char* text, unsigned int n);
void radixSort(char* text, unsigned int n);



#endif //__FUNCTIONS__H__


