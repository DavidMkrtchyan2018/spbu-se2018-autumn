#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef DEBUG  // switch DEBUG_PRINT with ifdef / ifndef
#define PR_DEBUG
#endif


#ifdef PR_DEBUG
#define DEBUG_PRINT(fmt, args...)    printf(fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif


typedef struct _strListNode
{
    char *str;
    size_t len;
    struct _strListNode *next;
} strListNode;


typedef struct _linkListOfStrings
{
    strListNode* head;
    strListNode* tail;
} listOfStrings;


static void nullifyLists(listOfStrings* list)
{
    unsigned int i = 1;
    for (; i < 256; i++) 
    {
        list[i].head = NULL;
        list[i].tail = NULL;
    }
}


static size_t initEntries(char** strings, size_t nStrings, strListNode* arr)
{
    unsigned int i;
    size_t maxlen = 0;
    
    for (i = 0; i < nStrings; i++) 
    {
        arr[i].str = strings[i];
        arr[i].len = strlen(strings[i]);
	
        if (arr[i].len > maxlen) 
	{
            maxlen = arr[i].len;
        }
        
        if (i < nStrings - 1) 
	{
            arr[i].next = &arr[i + 1];
        }
        else 
	{
            arr[i].next = NULL;
        }
    }
    
    return maxlen;
}


static void sortStrings(strListNode* head, listOfStrings* arr, unsigned int c, int* last, int backwards)
{
    strListNode* current = head;
    unsigned int i = 0;
    int pos = 0;
    unsigned char ch = 0;
    #include <ctype.h>
    while (current != NULL) 
    {
        strListNode* next = current->next;
        current->next = NULL;
        pos = backwards ? current->len - 1 - c : c;
	
        if( backwards ? pos < 0 : pos >= current->len - 1 )
        {
            ch = 0;
        }
        else
        {
            ch = current->str[pos];
            i++;
        }
        
        if (arr[ch].head == NULL) 
        {
            arr[ch].head = current;
            arr[ch].tail = current;
        }
        else 
        {
            arr[ch].tail->next = current;
            arr[ch].tail = arr[ch].tail->next;
        }
        
        current = next;
    }

    if (i == 1)
    {
        DEBUG_PRINT("Last string\n");
        *last = 1;
    }
}
 

static strListNode* mergeEntries(listOfStrings* arr, unsigned int num)
{
    strListNode *head = NULL;
    strListNode *tail = NULL;
    unsigned int i = num;
    
    for (; i < 256; i++)
    {
        if (arr[i].head != NULL) 
        {
            if (head == NULL) 
            {
                head = arr[i].head;
                tail = arr[i].tail;
                DEBUG_PRINT("head set to %s\n", head->str);
            }
            else 
            {
                tail->next = arr[i].head;
                tail = arr[i].tail;
                DEBUG_PRINT("tail set to %s\n", tail->str);
            }
        }
    }
    
    return head;
}


static void copyBack(const strListNode *head, char **strings)
{
    const strListNode *current = head;
    unsigned int i = 0;
    
    if( !head )
    {
        printf("copyBack: error: head = NULL\n");
        return;
    }
    
    for (; current != NULL; current = current->next) 
    {
        strings[i++] = current->str;
    }
}


#ifdef PR_DEBUG
#error (A)
void printStrArr(const listOfStrings* arr)
{
    unsigned int i;
    
    for (i = 0; i < 256; i++) 
    {
        if (arr[i].head != NULL)
        {
            const strListNode *entry;
            printf("[%d] ", i);
            
            for (entry = arr[i].head; entry != NULL; entry = entry->next) 
            {
                printf("%s", entry->str);
                if (entry->next) 
                {
                    printf(" -> ");
                }
            }
            putchar('\n');
        }
    }
    
    putchar('\n');
}

void printList(const strListNode* head)
{
    const strListNode* current;
    printf("TO DO: ");
    for (current = head; current != NULL; current = current->next) 
    {
        printf("%s", current->str);
        
        if (current->next != NULL) 
        {
            printf(" -> ");
        }
    }
    
    printf("\n\n");
}

#else

void printStrArr(const listOfStrings* arr){}
void printList(const strListNode* head){}

#endif


void radixSortStrings(char** strings, size_t nStrings)
{
    register size_t maxlen;
    register size_t max1;
    unsigned int i = 0;
    int last = 0;
    strListNode* head;
    
    strListNode* entries = malloc(nStrings * sizeof(strListNode));
    
    listOfStrings* arrOfLists = malloc(256 * sizeof(listOfStrings));
    
    if (!entries)
    {
	printf("Memory allocation error\n");
        if (arrOfLists)
        {
            free(arrOfLists);
        }
        return;
    }
    
    if (!arrOfLists)
    {
        printf("Memory allocation error\n");
        free(entries);
        return;
    }
    
    arrOfLists[0].head = NULL;
    arrOfLists[0].tail = NULL;
    nullifyLists(arrOfLists);
    maxlen = initEntries(strings, nStrings, entries);
    head = &entries[0];
    max1 = maxlen - 1;
    
    for (i = 0; i < maxlen; i++)
    {
        DEBUG_PRINT("Bucketing on char %d\n", i);
        sortStrings(head, arrOfLists, i, &last, 1);
	
        if( last || i == max1 )
        {
            break;
        }
        head = mergeEntries(arrOfLists, 1);
        nullifyLists(arrOfLists);
    }
    
    head = mergeEntries(arrOfLists, 0);
    nullifyLists(arrOfLists);
    copyBack(head, strings);
    
    free(arrOfLists);
    free(entries);
}
