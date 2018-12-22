#include "functions.h"
#include "radixSort.h"

static void deleteStringArr(char** arr, unsigned int nStrings)
{
    int count = 0;
    for(; count < nStrings; count++)
    {
        if(arr[count])
        {
            free(arr[count]);
        }
    }
    free(arr);
}

static void printArr(char** arr, unsigned int nStrings)
{
    int count = 0;
    for(; count < nStrings; count++)
    {
        printf("str[%d]: %s\n", count, arr[count]);
    }
}

static void saveArr(char** arr, unsigned int nStrings)
{
    int count = 0;
    FILE *fp;
    
    if ((fp = fopen("out.txt", "w"))==NULL)
    {
	printf("Error opening file, %s\n", strerror(errno));
	return;
    }
    
    for(; count < nStrings; count++)
    {
        fprintf(fp, "%s\n", arr[count]);
    }
    fclose(fp);
}

static char** getStrings(char* text, unsigned int* nStrings)
{
    unsigned int N = *nStrings;
    unsigned int strLen = strlen(text);
    unsigned int charsPassed = 0;
	char** arr = (char**) malloc(N * sizeof(char**));
	unsigned int count = 0;
	int nChars = 0;
	char* pChar = text;

	if(!arr)
    {
        printf("Memomory allocation error\n");
        free(text);
        exit(1);
    }
    
    for(; count < N; count++)
    {
    	arr[count] = NULL;
    }

	for(count = 0; charsPassed < strLen && count < N; count++)
    {
        while(*pChar != '\n')
        {
            pChar++;
            if(*pChar == EOF)
            {
                break;
            }
            nChars++;
        }

        if( nChars )
        {// get only strings with letters
            // create a buffer for zero-ending string with n Chars, save its address in arr[count]
            arr[count] = (char*) malloc(nChars + 1);
            if(!arr[count])
            {
                printf("Memomory allocation error\n");
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
        printf("no enogh strings with letters found, working with %u strings\n", count);
        *nStrings = count;
    }

	return arr;
}

static void swapStrings(char** px, char** py)
{
    char* tmp = *px;
    *px = *py;
    *py = tmp;
}

//prints usage of the programm
void usage()
{
	printf("this programm has to be caled with 3 arguments\n");
	printf("first argument must be number of strings to proceed\n");
	printf("second argument must be a file name\n");
	printf("third argument must be algoritm code: \n");
	printf("b for bubble, i for insertion, m for merge, q for quick, r for radix\n");
}

//checks if input string represents an unsigned integer
int isInt(char* str)
{
	size_t count = 0;
	size_t strLen = strlen(str);

	for(count = 0; count < strLen; count++)
	{
		if(!isdigit(str[count]))
		{
			printf("error: non numeric value in first parameter\n");
			return 0;
		}
	}
	return 1;
}

// A function to implement bubble sort
void bubbleSort(char* text, unsigned int n)
{
	unsigned int nStr = n;
	char** arr = getStrings(text, &nStr);
	int i = 0;
	int j = 0;
    //printf("sorting started\n");
  	for (; i < nStr-1; i++)
	{// Last i elements are already in place
	    for (j = 0; j < nStr-i-1; j++)
		{
			if (strcmp(arr[j], arr[j+1]) > 0)
			{
				swapStrings(&arr[j], &arr[j+1]);
			}
		}
	}
    //printf("sorting ended\n");
    //printArr(arr, nStr);
    //saveArr(arr, nStr);
    //printf("array printed\n");
    deleteStringArr(arr, nStr);
}

/* Function to sort an array using insertion sort*/
void insertionSort(char* text, unsigned int n)
{
   unsigned int nStr = n;
   char** arr = getStrings(text, &nStr);
   char* key = 0;
   int i = 0;
   int j = 0;
   for (i = 1; i < nStr; i++)
   {
       key = arr[i];
       j = i-1;

       /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
       while (j >= 0 && strcmp(arr[j], key) > 0)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
   //printf("sorting ended\n");
    printArr(arr, nStr);
    //printf("array printed\n");
	deleteStringArr(arr, nStr);
}

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(char* arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    char* L[n1];
    char* R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if( strcmp(L[i], R[j]) <= 0 )
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSortStrings(char* arr[], int l, int r)
{
    if (l < r)
    {
	printf("mergeSortStrings l=%d r=%d\n", l, r);
        // Same as (l+r)/2, but avoids overflow for large l and h
        int m = l+(r-l)/2;
        // Sort first and second halves
        mergeSortStrings(arr, l, m);
        mergeSortStrings(arr, m+1, r);
        merge(arr, l, m, r);
    }
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (char* arr[], int low, int high)
{
    char* pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
    int j = low;

    for (; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
	if( strcmp(arr[j], pivot) <= 0 )
        {
            i++;    // increment index of smaller element
            swapStrings(&arr[i], &arr[j]);
        }
    }
    swapStrings(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSortStrings(char* arr[], int low, int high)
{
    if (low < high)
    {
	printf("quickSortStrings low=%d high=%d\n", low, high);
        /* pi is partitioning index, arr[pi] is now at right place */
        int pi = partition(arr, low, high);
	printf("quickSortStrings arr[%d]=%s\n", pi, arr[pi]);
        // Separately sort elements before
        // partition and after partition
        quickSortStrings(arr, low, pi - 1);
        quickSortStrings(arr, pi + 1, high);
    }
}

void quickSort(char* text, unsigned int n)
{
    unsigned int nStr = n;
    char** arr = getStrings(text, &nStr);
    printf("sorting started\n");
    quickSortStrings(arr, 0, nStr-1);
    printf("sorting ended\n");
    printArr(arr, nStr);
    //printf("array printed\n");
    deleteStringArr(arr, nStr);
}

void mergeSort(char* text, unsigned int n)
{
    unsigned int nStr = n;
    char** arr = getStrings(text, &nStr);
    printf("sorting started\n");
    mergeSortStrings(arr, 0, nStr-1);
    printf("sorting ended\n");
    printArr(arr, nStr);
    //printf("array printed\n");
    deleteStringArr(arr, nStr);
}

// The main function to that sorts arr[] of size n using
// Radix Sort
void radixSort(char* text, unsigned int n)
{
    // Find the maximum number to know number of digits
    unsigned int nStr = n;
    char** arr = getStrings(text, &nStr);
    
    radixSortStrings(arr, nStr);
    
    //printf("sorting ended\n");
    printArr(arr, nStr);
    //printf("array printed\n");
    deleteStringArr(arr, nStr);
}






