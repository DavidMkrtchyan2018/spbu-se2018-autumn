/* Radix sort of strings in C
 * original idea - of Martin 
 * http://www.martinbroadhurst.com/radix-sort-of-strings-in-c.html
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG  // switch DEBUG_PRINT with ifdef / ifndef
#define PR_DEBUG
#endif

#ifdef PR_DEBUG
#define DEBUG_PRINT(fmt, args...)    printf(fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Do nothing in release */
#endif

/* 		STRUCTURE DECLARATIONS		 */

/* A linked list node containing a string and length */
typedef struct _strListNode
{
    char *str;
    size_t len;
    struct _strListNode *next;
} strListNode;

 
/* A linked list */
typedef struct _linkListOfStrings
{
    strListNode* head;
    strListNode* tail;
} listOfStrings;

/* 		STATIC FUNCTIONS		 */

/* Initialise lists with NULL values */
static void nullifyLists(listOfStrings* list)
{
    unsigned int i = 1;
    for (; i < 256; i++) 
    {
        list[i].head = NULL;
        list[i].tail = NULL;
    }
}
 
/* Turn entries into a linked list of strings with their lengths (strListNodes)
 * Return the length of the longest string
 */
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

 
/* Put strings into appropriate place according to the position */
static void sortStrings(strListNode* head, 
			listOfStrings* arr, 
			unsigned int c, 
			int* last,
			int backwards
 		      )
{
    strListNode* current = head;
    unsigned int i = 0;
    int pos = 0;
    unsigned char ch = 0;
    
    while (current != NULL) 
    {
        strListNode* next = current->next;
        current->next = NULL;
        pos = backwards ? current->len - 1 - c : c;
	//printf("cp 0 pos = %d\n", pos);
	
        if( backwards ? pos < 0 : pos >= current->len - 1 )
	{
            ch = 0;
        }
        else
	{
            ch = current->str[pos];
	    i++;
	    //printf("cp 1 setting ch to %c\n", ch);
        }
        //printf("cp 1 ch = %c\n", ch);
        
        if (arr[ch].head == NULL) 
	{
            arr[ch].head = current;
            arr[ch].tail = current;
	    //printf("cp 2 setting head for arr[%d]\n", ch);
        }
        else 
	{
            arr[ch].tail->next = current;
            arr[ch].tail = arr[ch].tail->next;
	    //printf("cp 3 Adding entry to arr[%d]\n", ch);
        }
        
        current = next;
    }

    if (i == 1)
    {
	DEBUG_PRINT("Last string\n");
	*last = 1;
    }
}
 
/* Merge list into a single linked list */
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
    
    //printf("copyBack %s\n", current->str);
    
    for (; current != NULL; current = current->next) 
    {
        strings[i++] = current->str;
	//printf("cp %s\n", current->str);
    }
}

/* 		PRINT FUNCTIONS	(just for debug)	 */
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

/* 		WORKING FUNCTION			*/

void radixSortStrings(char** strings, size_t nStrings)
{
    register size_t maxlen;
    register size_t max1;
    unsigned int i = 0;
    int last = 0;
    strListNode* head;
    
    /* buffer for convenient structures, where all nodes live */
    strListNode* entries = malloc(nStrings * sizeof(strListNode));
    
    /* array of linked lists, sorted entries are collecting to the arrOfLists[0].  
     * algorithm stops when all but the last string are sorted.
     */
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
    //printf("maxlen = %d\n", maxlen);
    
    /* Main loop */
    for (i = 0; i < maxlen; i++)
    {
        DEBUG_PRINT("Bucketing on char %d\n", i);
        sortStrings(head, arrOfLists, i, &last, 1);
	
	if( last || i == max1 )
	{
	    break;
	}
        //printStrArr(arrOfLists);
        head = mergeEntries(arrOfLists, 1);
        //printList(head);
        nullifyLists(arrOfLists);
    }
    
    head = mergeEntries(arrOfLists, 0);
    nullifyLists(arrOfLists);
    //printf("before copyBack head = %p\n", head);
    /* Copy back into array */
    copyBack(head, strings);
    
    free(arrOfLists);
    free(entries);
}

