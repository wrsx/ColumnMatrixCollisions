#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
/*
 * 
 */
#define cols 500
#define rows 4400
#define keysize 15

float mat[cols][rows]; //test dimensions
char keys[rows][keysize];
int loadMatrix() {
    int bufsize = cols * sizeof(char) * 10;
    char buffer[bufsize];
    char *record, *line;
    int i = 0, j = 0;
    FILE *fstream = fopen("data.txt", "r");
    if (fstream == NULL)
    {
        printf("\n Opening matrix failed ");
        return EXIT_FAILURE;
    }
    while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL)
    {
        record = strtok(line, ",");
        while (record != NULL)
        {
            //printf("Recording: to [%d][%d] \n", i, j);
            mat[j++][i] = atof(record);
            record = strtok(NULL, ",");
        }
        j = 0;
        ++i;
    }
        return EXIT_SUCCESS;
}

int loadKeys() {
    char buffer[1024];
    char *record, *line;
    int i = 0;
    FILE *fstream = fopen("testkeys.txt", "r");
    if (fstream == NULL)
    {
        printf("\n Opening key vector failed ");
        return EXIT_FAILURE;
    }
    while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL)
    {
        record = strtok(line, " ");
        while (record != NULL)
        {
            //printf("Recording: %s to [%d] \n", record, i);
            strcpy(keys[i], record);
            keys[i][14] = '\0';
            record = strtok(NULL, " ");
            i++;
        }
    }
        return EXIT_SUCCESS;
}

char* genBlock(int col[], int colSize) {
    double dia = 0.000001; 
    bool within_dia[colSize][colSize];
    for (int i = 0; i < colSize; i++) {
        for (int j = 1; j < colSize; j++) {
            if (abs(col[i] - col[j]) < dia) {
                //the two elements neighbour
                //store the row of the matching element
                within_dia[i][j] = true;
            }
        }

    }
    return 0;
}

int floatcomp(const void* elem1, const void* elem2) {
    if(*(const float*)elem1 < *(const float*)elem2) {
    	return -1;
    }
    return *(const float*)elem1 > *(const float*)elem2;
}
 
void findBlocks(float col[], int colSize, float dia) { 	
    qsort(col, colSize, sizeof(float), floatcomp);

    float min, max = 0;
    //int result[colSize][colSize];
    int **result;
    result = malloc(colSize * sizeof(int *));
    int temp[colSize];  
    memset(temp, -1, sizeof(temp));
    int tempCount = 0;
    int resultCount = 0;

    for(int i = 0; i < colSize; i++) {
        //fprintf(stderr,"[%d] %f\n",i, col[i]);

        if(temp[0] == -1) {
            min = max = col[i];
            temp[tempCount++] = i;    
        } else {
            if (fabs(col[i] - min) < dia && fabs(col[i] - max) < dia && tempCount < 4) {
                temp[tempCount++] = i;
                if(col[i] < min) {
                    min = col[i];
                } else if(col[i] > max) {
                    max = col[i];
                }

            } else {
                if(temp[0] != -1 && tempCount > 3) {
                    //allocate the memory to store the result
                    result[resultCount] = malloc((tempCount + 1) * sizeof(int)); 
                    
                    //loops over all the results in temp and one extra to ensure -1 is the last value in the array
                    for(int j = 0; j <= tempCount; j++) {
                        result[resultCount][j] = temp[j];
                    }
                    resultCount++;
                }
                min = max = col[i];
                memset(temp, -1, sizeof(temp));
                temp[0] = i;
                i = i - tempCount;
                tempCount = 0;
            }
        }      
    }

    for(int j = 0; j < resultCount; j++) {
        int k = 0;
        printf("[");
        while(result[j][k] != -1) {
                printf("%f, ", col[result[j][k++]]);
        }
        printf("]\n");

    }
     
}
 
int main(int argc, char* argv[]) {
    loadMatrix();
    //loadKeys();
    findBlocks(mat[0], rows, 0.000001);
    printf("\n");
    findBlocks(mat[1], rows, 0.000001);

    
    /*
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%f ", mat[j][i]);
        }
        printf("\n");
    }
    */
    /*
    for (int i = 0; i < 4400; i++) {
        printf("%f \n", mat[0][i]);
    }
    */
    
    return (EXIT_SUCCESS);
}

