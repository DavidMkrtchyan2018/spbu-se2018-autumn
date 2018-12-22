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
        {
            arr[count] = (char*) malloc(nChars + 1);
            if(!arr[count])
            {
                printf("Memomory allocation error\n");
                deleteStringArr(arr, count - 1);
                free(text);
                exit(1);
            }
            memcpy(arr[count], pChar - nChars, nChars);
            arr[count][nChars] = '\0';
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

void usage()
{
	printf("this programm has to be caled with 3 arguments\n");
	printf("first argument must be number of strings to proceed\n");
	printf("second argument must be a file name\n");
	printf("third argument must be algoritm code: \n");
	printf("b for bubble, i for insertion, m for merge, q for quick, r for radix\n");
}

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

void bubbleSort(char* text, unsigned int n)
{
	unsigned int nStr = n;
	char** arr = getStrings(text, &nStr);
	int i = 0;
	int j = 0;
  	for (; i < nStr-1; i++)
	    for (j = 0; j < nStr-i-1; j++)
		{
			if (strcmp(arr[j], arr[j+1]) > 0)
			{
				swapStrings(&arr[j], &arr[j+1]);
			}
		}
	}
    printArr(arr, nStr);
    deleteStringArr(arr, nStr);
}

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

       while (j >= 0 && strcmp(arr[j], key) > 0)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
    printArr(arr, nStr);
	deleteStringArr(arr, nStr);
}

void merge(char* arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    char* L[n1];
    char* R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    i = 0;
    j = 0;
    k = l;
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

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortStrings(char* arr[], int l, int r)
{
    if (l < r)
    {
	printf("mergeSortStrings l=%d r=%d\n", l, r);
        int m = l+(r-l)/2;
        mergeSortStrings(arr, l, m);
        mergeSortStrings(arr, m+1, r);
        merge(arr, l, m, r);
    }
}

int partition (char* arr[], int low, int high)
{
    char* pivot = arr[high];
    int i = (low - 1);
    int j = low;

    for (; j <= high- 1; j++)
    {
	if( strcmp(arr[j], pivot) <= 0 )
        {
            i++;
            swapStrings(&arr[i], &arr[j]);
        }
    }
    swapStrings(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSortStrings(char* arr[], int low, int high)
{
    if (low < high)
    {
	printf("quickSortStrings low=%d high=%d\n", low, high);
        int pi = partition(arr, low, high);
	printf("quickSortStrings arr[%d]=%s\n", pi, arr[pi]);
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
    deleteStringArr(arr, nStr);
}

void radixSort(char* text, unsigned int n)
{
    unsigned int nStr = n;
    char** arr = getStrings(text, &nStr);
    
    radixSortStrings(arr, nStr);
    
    printArr(arr, nStr);
    deleteStringArr(arr, nStr);
}
