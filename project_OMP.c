#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
#define INPUTFOLDER "inputs"

char *textData;
int textLength;

char *patternData;
int patternLength;

// Hold the control data values
int controlData[1000][3];

FILE *output;

void outOfMemory()
{
        fprintf (stderr, "Out of memory\n");
        exit (0);
}

void readFromFile (FILE *f, char **data, int *length)
{
        int ch;
        int allocatedLength;
        char *result;
        int resultLength = 0;

        allocatedLength = 0;
        result = NULL;
		
        ch = fgetc (f);
        while (ch >= 0)
        {
                resultLength++;
                if (resultLength > allocatedLength)
                {
                        allocatedLength += 10000;
                        result = (char *) realloc (result, sizeof(char)*allocatedLength);
                        if (result == NULL)
                                outOfMemory();
                }
                result[resultLength-1] = ch;
                ch = fgetc(f);
        }
        *data = result;
        *length = resultLength;
}

int readData (int textFileNo, int patternFileNo)
{
	FILE *f;
	char fileName[1000];
#ifdef DOS
        sprintf (fileName, "%s\\text%d.txt", INPUTFOLDER, textFileNo);
#else
	sprintf (fileName, "%s/text%d.txt", INPUTFOLDER, textFileNo);
#endif
	f = fopen (fileName, "r");
	if (f == NULL)
		return 0;
	
	readFromFile (f, &textData, &textLength);
	fclose (f);
#ifdef DOS
    sprintf (fileName, "%s\\pattern%d.txt", INPUTFOLDER, patternFileNo);
#else
	sprintf (fileName, "%s/pattern%d.txt", INPUTFOLDER, patternFileNo);
#endif
	f = fopen (fileName, "r");
	if (f == NULL){
		return 0;
	}
	readFromFile (f, &patternData, &patternLength);
	fclose (f);

	return 1;

}


int readControlFile(){

    FILE *f;
    char fileName[1000];

#ifdef DOS
    sprintf (fileName, "%s\\control.txt", INPUTFOLDER);
#else
    sprintf (fileName, "%s/control.txt", INPUTFOLDER);
#endif
    f = fopen (fileName, "r");
    if (f == NULL)
        return 0;

	int count = 0;
	// Count number of lines
    while(fscanf(f, "%d %d %d", &controlData[count][0], &controlData[count][1], &controlData[count][2]) !=EOF){
        count++;
    }
    fclose (f);
	return count;
}

void hostMatchAll(int textFileNo, int patternFileNo)
{
	int i,j,k, lastI;
	int matchingLocation = -1;
	lastI = textLength - patternLength;
	i=0;
	j=0;
	k=0;
	
	// Parallel for loop
	#pragma omp parallel for private(i, j, k) shared(matchingLocation) schedule(static)
	for(i = 0; i <= lastI; i++)
	{
		k=i;
		j=0;
		while (textData[k] == patternData[j] && j < patternLength)
		{
			k++;
			j++;
		}
		if (j == patternLength)
		// Match found, add text + pattern file number and the matching index to the text file
		#pragma omp critical
		{
			matchingLocation = i;
			fprintf(output, "%d %d %d\n", textFileNo, patternFileNo, matchingLocation);
		}
	}
	// No match, return -1 with the files
	if (matchingLocation == -1)
	{
		fprintf(output, "%d %d %d\n", textFileNo, patternFileNo, matchingLocation);
	}
}

void hostMatchOne(int textFileNo, int patternFileNo)
{
	int i, j, k, lastI, flag;
	// Default value -1 for no match
	int matchingLocation = -1;
	lastI = textLength - patternLength;
	i=0;
	j=0;
	k=0;
	
	// Parallel for loop
	#pragma omp parallel for private(i,j,k) shared(matchingLocation,flag) schedule(static)
	for(i = 0; i <= lastI; i++)
	{
		// Use a flag to notify other threads a match has been found
		if(flag == 0)
		{
			k=i;
			j=0;
			while(textData[k] == patternData[j] && j < patternLength)
			{
				k++;
				j++;
			}
			if(j == patternLength)
			// Match found
			#pragma omp critical
			{
				matchingLocation = -2;
				flag++;
			}
		}
	}
	// No match found, return -1
	if(matchingLocation==-1){
		fprintf(output, "%d %d %d\n", textFileNo, patternFileNo, matchingLocation);
	// Match found, return -2
	} else {
		fprintf(output, "%d %d %d\n", textFileNo, patternFileNo, matchingLocation);
	}
}

void processData(int oneOrAll, int textFileNo, int patternFileNo)
{
	// If the pattern size is larger than the text size, don't search, just return -1
    if(patternLength > textLength){
       fprintf(output, "%d %d %d\n", textFileNo, patternFileNo, -1);
       return;
    }
	// Only return 1 match
    if(oneOrAll == 0){ 
        hostMatchOne(textFileNo, patternFileNo);
    }
	// Return all matches
    else if(oneOrAll == 1){ 
        hostMatchAll(textFileNo, patternFileNo);
    }
}

int main(int argc, char *argv[])
{
	// Open result_OMP.txt
    output = fopen("result_OMP.txt", "w");

	// Count the number of lines in the control file and read the data
    int testNumber = readControlFile();

    int i=0;
	// For the number of lines in the control file
	// Read the text and pattern files
	// Pass the files and the mode (any or all) to processData
    while(i<testNumber){
		readData(controlData[i][1], controlData[i][2]);
        processData(controlData[i][0], controlData[i][1], controlData[i][2]);
        i++;
    }
	
    fclose(output);
    return 0;
}

