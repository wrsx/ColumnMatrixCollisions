#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
/*
 * 
 */

float mat[10][20]; //test dimensions
char keys[20][15];

int loadMatrix() {
    
    char buffer[1024];
    char *record, *line;
    int i = 0, j = 0;
    FILE *fstream = fopen("testdata.txt", "r");
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
    float result[colSize][colSize];
    float temp[colSize];    
    memset(result, 0, sizeof(result[0][0]) * colSize * colSize);
    memset(temp, 0, sizeof(temp));
    int tempCount = 0;
    int resultCount = 0;

    for(int i = 0; i < colSize; i++) {
        //fprintf(stderr,"[%d] %f\n",i, col[i]);

        if(temp[0] == 0) {
            min = max = col[i];
            temp[tempCount++] = col[i];    
        } else {
            if (fabs(col[i] - min) < dia && fabs(col[i] - max) < dia && tempCount < 4) {
                temp[tempCount++] = col[i];
                if(col[i] < min) {
                    min = col[i];
                } else if(col[i] > max) {
                    max = col[i];
                }

            } else {
                if(temp[0] != 0 && tempCount > 3) {
                     for(int j = 0; j < tempCount; j++) {
                        result[resultCount][j] = temp[j];
                    }
                    resultCount++;
                }
                min = max = col[i];
                memset(temp, 0, sizeof(temp));
                temp[0] = col[i];
                i = i - tempCount;
                tempCount = 0;
            }
        }      
    }

    for(int j = 0; j < resultCount; j++) {
        int k = 0;
        printf("[");
        while(result[j][k] != 0) {
                printf("%f, ", result[j][k++]);
        }
        printf("]\n");

    }
     
}
 
int main(int argc, char* argv[]) {
    loadMatrix();
    loadKeys();
    findBlocks(mat[0], 20, 0.01);

    /*
    
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%f ", mat[j][i]);
        }
        printf("\n");
    }
    */
    /*
    for (int i = 0; i < 20; i++) {
        printf("%s \n", keys[i]);
    }*/
    
    return (EXIT_SUCCESS);
}

