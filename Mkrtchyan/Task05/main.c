#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void input();
void other();


int main()
{
    input();
    return 0;
}

void other()
{
    printf("Augh! other() hacked!\n\n");
    exit(0);
}

void input()
{
    unsigned long int buf = (unsigned long int)main;
    unsigned long int* ptr = &buf;
    int i = 0;

    for(; i < 10; i++, ptr++)
    {
      if( *ptr > buf && *ptr < buf + 24 )
      {
        printf("enter %lu: ", (unsigned long int)other);
        if( scanf("%lu", &buf) != 1 )
        {
            printf("scanf error: could not read input. Probably, wrong input.\nPlease, try again.\n");
            return;
        }
        *ptr = buf;
        /* *ptr = (unsigned long int)other; */
        break;
      }
    }
    printf("\n");
}
