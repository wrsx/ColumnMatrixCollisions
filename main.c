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
#define blocksize 4

struct element {
   int index;
   float value;
};

struct element data[cols][rows];
//float mat[cols][rows]; //test dimensions
char keys[rows][keysize];
struct element **blocks[cols];

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
    while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL) {
        record = strtok(line, ",");
        while (record != NULL)
        {
            //printf("Recording: to [%d][%d] \n", i, j);
            data[j][i].index = i;
            data[j][i].value = atof(record);
            j++;
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

int elementComp(const void* p1, const void* p2) {
    const struct element *elem1 = p1;
    const struct element *elem2 = p2;

    if(elem1->value < elem2->value) {
    	return -1;
    }
    return (elem1->value > elem2->value);
}

int subset(struct element temp[], int tempCount, int col, int k, int start, int currLen, bool used[], int resultCount) {
    if (currLen == k) {
        blocks[col][resultCount] = malloc((k+1) * sizeof(struct element)); 
        int blockCount = 0;
	for (int i = 0; i < tempCount; i++) {
	    if (used[i] == true) {
                blocks[col][resultCount][blockCount++] = temp[i];
	    }
	}
        blocks[col][resultCount][blockCount] = temp[tempCount]; //ensures the last item is -1
        return 1;
    }
    if (start == tempCount) {
        return 0;
    }
    int new = 0;

    used[start] = true;
    new += subset(temp, tempCount, col, k, start + 1, currLen + 1, used, resultCount);

    used[start] = false;
    new += subset(temp, tempCount, col, k, start + 1, currLen, used, resultCount+new);
    
    return new;
}

void findBlocks(int col, int colSize, float dia) { 	
    struct element *column = data[col];
    qsort(column, colSize, sizeof(struct element), elementComp);

    float min, max = 0;
  
    struct element *result[colSize];
    //result = malloc(cols * sizeof(struct element *));  
    
    
    struct element temp[colSize];  
    memset(&temp, -1, sizeof(temp));
    
    int tempCount = 0;
    int lastTempCount = 0;
    int resultCount = 0;
    int finalResultCount = 0;
    
    for(int i = 0; i < colSize; i++) {
        //fprintf(stderr,"[%d] %f\n",col[i].index, col[i].value);

        if(temp[0].index == -1) {
            min = max = column[i].value;
            temp[tempCount++] = column[i];    
        } else {
            if (fabs(column[i].value - min) < dia && fabs(column[i].value - max) < dia) {
                temp[tempCount++] = column[i];
                if(column[i].value < min) {
                    min = column[i].value;
                } else if(column[i].value > max) {
                    max = column[i].value;
                }

            } else {
                /*
                -If the block is not empty (-1)
                -If the block is larger than 3
                -If the block is larger or equal in size to the previous block
                    This ensures that the current block is not a sub-block of the previous block
                */
                if((temp[0].index != -1) && (tempCount >= blocksize) && (tempCount >= lastTempCount)) {        
                    if(tempCount > blocksize) { //need to calculate the total combinations of 
                        //formula for working out combinations of size k(blocksize) for n(tempcount) values 
                        finalResultCount += round(exp(lgamma(tempCount+1)-lgamma(tempCount-blocksize+1))/tgamma(blocksize+1));
                    } else {
                        finalResultCount++;
                    }
                    //printf("count %i\n", tempCount);                    
                    //printf("finalresultCount %i\n", finalResultCount);
                    //allocate the memory to store the result
                    result[resultCount] = malloc((tempCount+1) * sizeof(struct element)); 
                    //loops over all the results in temp and one extra to ensure -1 is the last value in the array
                    for(int j = 0; j <= tempCount; j++) {
                        result[resultCount][j] = temp[j];
                    }
                    resultCount++;
                }
                min = max = column[i].value;
                memset(&temp, -1, sizeof(temp));
                i = i - tempCount;
                lastTempCount = tempCount;
                tempCount = 0;
            }
        }      
    }
    blocks[col] = malloc((finalResultCount) * sizeof(struct element *));
    //printf("resultcount %i | finalresultCount %i\n", resultCount, finalResultCount);
    int blockCount = 0;
    for(int j = 0; j < resultCount; j++) {
        blocks[col][blockCount] = malloc((blocksize+1) * sizeof(struct element)); 
        int length = 0;
        do {
            if(length < blocksize+1) {
                blocks[col][blockCount][length] = result[j][length];
            }
            //printf("[%i] %f, ",result[j][length].index , result[j][length].value);

        } while(result[j][length++].index != -1);
        length--;
        printf("%i\n", length);
        //if(length == 18) {
        if(length > blocksize) {
            bool used[tempCount];
            blockCount += subset(result[j], length, col, blocksize, 0, 0, used, blockCount);        
        } else {
            blockCount++;
        }
        free(result[j]);
    }

    /*
    for(int k = 0; k < blockCount; k++) {
        int l = 0;
        printf("[");          
        while(blocks[col][k][l].index != -1) {
            printf("[%i] %f, ",blocks[col][k][l].index , blocks[col][k][l].value);
            l++;
        }
        printf("]\n");        
    }      */
}
 
int main(int argc, char* argv[]) {
    loadMatrix();
    //loadKeys();
    

    
   
    for(int i = 499; i < cols; i++) {
        printf("\n-----------COLUMN %i---------\n",i);
        findBlocks(i, rows, 0.000001);
        printf("\n");
    }

    
    /*
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%f ", mat[j][i].value);
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

