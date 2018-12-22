#include "functions.h"
#include "timer.h"


int main(int argc, char* argv[])
{
	FILE *fp;
	int numStr;
	char* fileName;
	char sortingMetod;

	if(argc != 4)
	{
		usage();
		return 1;
	}

	if(isInt(argv[1]))
	{
		numStr = atoi(argv[1]);
	}
	else
	{
		printf("wrong parameter 1\n");
		return 1;
	}

	fileName = argv[2];

	if ((fp = fopen(fileName, "r"))==NULL)
	{
	 	printf("Error opening file, %s\n", strerror(errno));
		return 1;
	}

	if(strlen(argv[3]) != 1)
	{
		fclose(fp);
		printf("wrong parameter 3\n");
		usage();
		return 1;
	}

	sortingMetod = argv[3][0];

	unsigned int count = 0;
	unsigned int nStrings = 0;
	char ch;
	char* text = NULL;
	size_t nRead = 0;

	while((ch=fgetc(fp)) != EOF)
	{
		count++;
		if(ch == '\n')
		{
			nStrings++;
		}
	}

	text = (char*) malloc(count + 1);
	rewind(fp);
	nRead = fread(text, 1, count, fp);
	fclose(fp);

	if (nRead != count)
	{
		printf("Error opening reading file, %s, nRead = %lu, count = %d\n", strerror(errno), (long unsigned int)nRead, count);
		free(text);
		return 1;
	}

	text[count] = '\0';
    printf("Number of strings in text file is %u\n", nStrings);

	if( numStr > nStrings )
    {
        printf("Info: Number of strings you entered (%u) is more than exists in the text provided (%u),\n", numStr, nStrings);
        printf("using %u instead.\n", nStrings);
        numStr = nStrings;
    }
    
    startTimer();

    switch(sortingMetod)
    {
      case 'b':
	bubbleSort(text, numStr);
	printElapsedTime();
	break;
      case 'i':
	insertionSort(text, numStr);
	printElapsedTime();
	break;
      case 'm':
	mergeSort(text, numStr);
	printElapsedTime();
	break;
      case 'q':
	quickSort(text, numStr);
	printElapsedTime();
	break;
      case 'r':
	radixSort(text, numStr);
	printElapsedTime();
	break;
      default:
	printf("wrong parameter 3\n");
	usage();
    }

    free(text);

    return 0;
}

