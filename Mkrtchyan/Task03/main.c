#include <stdio.h>
#include <math.h>


double sum_series( int n)
{
    double res=0,z=1;
    int i;

    for(i=1;i<=n;i++)
    {
        z=-z;
        res+=z/i;
    }

    return res;
}


int main(int argc, char **argv)
{

char *s;
double sum;
int n,i ;
if (argc!=2)
{

printf("parametr N required\n");
return -1;

}else{


s=argv[1];
n=0;
i=0;
int t;

while (s[i])
{
    t = (int )s[i];
    n = n*10+t-48;
    i++;
}
sum = sum_series(n);
printf ("%.14lf\n",sum);
}
return 0;

}
