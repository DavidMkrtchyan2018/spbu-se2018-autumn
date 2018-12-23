#include <stdio.h>
#include <stdlib.h>


extern int mysort (int sizein, int massive[]);

int main(int argc, char* argv[])
{
    int a = -2147483647;
    int b = 2147483647;
    int i = 1;
    while (i < argc)
    {
            if(argv[i][2] == 'f')
            {
                if(argv[i][6] == '=')
                {
                    if(argv[i][7] == '\0')
                    {
                        ++i;
                        a = atoi(argv[i]);
                    }
                    else
                    {
                        a = atoi(argv[i] + 7);
                    }
                }
                else
                {
                    ++i;
                    if(argv[i][1] == '\0')
                    {
                        ++i;
                        a = atoi(argv[i]);
                    }
                    else
                    {
                        a = atoi(argv[i] + 1);
                    }
                }
            }
            
            if(argv[i][2] == 't')
            {
                if(argv[i][4] == '=')
                {
                    if(argv[i][5] == '\0')
                    {
                        ++i;
                        b = atoi(argv[i]);
                    }
                    else
                    {
                        b = atoi(argv[i] + 5);
                    }
                }
                else
                {
                    ++i;
                    if(argv[i][1] == '\0')
                    {
                        ++i;
                        b = atoi(argv[i]);
                    }
                    else
                    {
                        b = atoi(argv[i] + 1);
                    }
                }
            }
	i++;
    }

    int j, res, count = 0, mas[100]={0};
    i = 0;
    char c;
    c = getchar();
    while (c != '\n')
    {
        if (c == ' ')
        {
            count++;
            c = getchar();
            continue;
        }
        
        if (c >= '0' && c <= '9')
        {
            mas[count] = mas [count]*10 + c - '0';
            c = getchar();
            continue;
        }
        
        if (c == '-')
        {
            c = getchar();
            while (c >= '0' && c <= '9')
            {
                mas[count] = mas[count]*10 - (c - '0');
                c = getchar();
            }
            continue;
        }
    }

    count++;
    int sort[100];
    i = 0;
    j = 0;
    
    while(i < count)
    {
        if (mas[i] <= a)
        {
            fprintf(stdout, "%d ", mas[i]);
            i++;
            continue;
        }
        
        if (mas[i] >= b)
        {
            fprintf(stderr, "%d ", mas[i]);
            i++;
            continue;
        }
        sort[j] = mas[i];
        j++;
        i++;
    }
    res = mysort (j, sort);
    printf("\n%d",res);

    return res;
}
