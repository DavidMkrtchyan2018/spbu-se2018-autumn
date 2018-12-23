#include "functions.h"

#define ANUM 10000
#define HASH_SZ 512

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
    {
        return HT_MISSING;
    }
    
    size_t len1 = strlen(str);
    size_t len2 = strlen(string);
    if( !len1 || !len2 )
    {
        return HT_MISSING;
    }
    
    if( len1 > len2 )
    {
        return HT_OK;
    }
    
    for(int i = 0; i < len1; i++)
    {
        if( str[i] != string[i] )
        {
            return HT_OK;
        }
    }
    
    return HT_FOUND;
}

int checkEntry(unsigned int val, char* str)
{
    if( !str || !strlen(str) || !val )
    return HT_MISSING;
    return HT_OK;
}


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
    free(text);
    
    unsigned int index = 0;
    int retVal = 0;
    
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
            printf("%s occures in the given text %d times\n", word, nOccurances);
        }
        else
        {
            printf("%s not found in the given text\n", word);
        }
    }
    
    printf("Please enter some letters to see if they are words starting so : ");
    scanf("%s", word);
    printf("Please enter a number, indicating minimal occurances of such words in the book to seek for : ");
    scanf("%d", &nOccurances);
    
    retVal = htIterate(hashT, findSubstring, nOccurances, word, &nWords);
    
    htFree(hashT);
    
    if( retVal != HT_OK )
    {
        printf("htIterate returns an error code: %d\n", retVal);
    }
    if( !nWords )
    {
        printf("No such words occured in the text more than %d times.\n", nOccurances);
    }
    
    printf("Buy-buy!\n");
    
    return 0;
}
