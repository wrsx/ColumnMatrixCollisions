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

struct neighbourhoods {
   int count;
   int blockCount;
   int col;
   struct element **neighbourhoods;
};

struct block {
   char *signature;
    int col;
    struct element *elements;
};

struct blocks {
   int count;
   struct block *blocks;
};

struct element data[cols][rows];

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


void printBlocks(struct blocks b) {
    for(int i = 0; i < b.count; i++) {
        int j = 0;
        printf("[");          
        while(b.blocks[i].elements[j].index != -1) {
            printf("[%i] %f, ",b.blocks[i].elements[j].index, b.blocks[i].elements[j].value);
            j++;
        }
        printf("]\n");        
    } 

}

int elementComp(const void* p1, const void* p2) {
    const struct element *elem1 = p1;
    const struct element *elem2 = p2;

    if(elem1->value < elem2->value) {
    	return -1;
    }
    return (elem1->value > elem2->value);
}

int blockComp(const void* p1, const void* p2) {

    const struct block *elem1 = p1;
    const struct block *elem2 = p2;
    char *value1 = elem1->signature;
    char *value2 = elem2->signature;

    int i = 0;
    while(value1[i] != '\0' && value2[i] != '\0') {
        if((value1[i] - '0') < (value2[i] - '0')) {
            return -1;
        } else if((value1[i] - '0') > (value2[i] - '0')) {
            return 1;
        }
        i++;        
    }
    return 0;
}

int findCombinations(struct blocks *b, struct element neighbourhood[], int neighbourhoodSize, int start, int currLen, bool used[], int col) {    
    if (currLen == blocksize) {
        b->blocks[b->count].elements = malloc((blocksize+1) * sizeof(struct element)); 
        b->blocks[b->count].col = col;
        int blockCount = 0;
	for (int i = 0; i < neighbourhoodSize; i++) {
	    if (used[i] == true) {
                b->blocks[b->count].elements[blockCount++] = neighbourhood[i];
	    }
	}
        b->blocks[b->count].elements[blocksize] = neighbourhood[neighbourhoodSize]; //ensures the last item is -1
        return 1;
    }
    if (start == neighbourhoodSize) {
        return 0;
    }
    int new = 0;

    used[start] = true;
    b->count += findCombinations(b, neighbourhood, neighbourhoodSize, start + 1, currLen + 1, used, col);

    used[start] = false;
    b->count += findCombinations(b, neighbourhood, neighbourhoodSize, start + 1, currLen, used, col);
    
    return new;
}

struct neighbourhoods getNeighbourhoods(int col, float dia) { 	
    //sort the column by size of the value
    struct element *column = data[col];
    qsort(column, rows, sizeof(struct element), elementComp);

    struct neighbourhoods n;
    n.neighbourhoods = malloc(cols * sizeof(struct element *));  
    n.count = 0; 
    n.blockCount = 0;
    n.col = col;

    struct element neighbourhood[rows];  
    memset(&neighbourhood, -1, sizeof(neighbourhood));

    float min, max = 0;
    int neighbourhoodSize = 0;
    int lastNeighbourhoodSize = 0;
    
    for(int i = 0; i < rows; i++) {
        //fprintf(stderr,"[%d] %f\n",col[i].index, col[i].value);
        if(neighbourhood[0].index == -1) {
            min = max = column[i].value;
            neighbourhood[neighbourhoodSize++] = column[i];    
        } else {
            if (fabs(column[i].value - min) < dia && fabs(column[i].value - max) < dia) {
                neighbourhood[neighbourhoodSize++] = column[i];
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
                if((neighbourhood[0].index != -1) && (neighbourhoodSize >= blocksize) && (neighbourhoodSize >= lastNeighbourhoodSize)) {        
                    if(neighbourhoodSize > blocksize) { //need to calculate the total combinations of 
                        //formula for working out combinations of size k(blocksize) for n(tempcount) values 
                       n.blockCount += round(exp(lgamma(neighbourhoodSize+1)-lgamma(neighbourhoodSize-blocksize+1))/tgamma(blocksize+1));
                    } else {
                        n.blockCount++;
                    }
                    //allocate the memory to store the neighbourhoods
                    n.neighbourhoods[n.count] = malloc((neighbourhoodSize+1) * sizeof(struct element)); 
                    //loops over all the elements in temp and one extra to ensure -1 is the last value in the array
                    for(int j = 0; j <= neighbourhoodSize; j++) {
                        n.neighbourhoods[n.count][j] = neighbourhood[j];
                    }
                    n.count++;
                }
                min = max = column[i].value;
                memset(&neighbourhood, -1, sizeof(neighbourhood));
                i = i - neighbourhoodSize;
                lastNeighbourhoodSize = neighbourhoodSize;
                neighbourhoodSize = 0;
            }
        }      
    }
    return n;
}

struct blocks getBlocks(struct neighbourhoods *n, int neighbourhoodsCount, int totalBlockCount) {
    struct blocks b;
    b.blocks = malloc(totalBlockCount * sizeof(struct block));
    b.count = 0;
    for(int i = 0; i < neighbourhoodsCount; i++) {
    //printf("neighbourhoodscount %i | finalresultCount %i\n", neighbourhoodCount, totalBlockCount);
        for(int j = 0; j < n[i].count; j++) {
            b.blocks[b.count].col = n[i].col;
            b.blocks[b.count].elements = malloc((blocksize+1) * sizeof(struct element)); 
            int length = 0;
            do {
                if(length < blocksize+1) {
                    b.blocks[b.count].elements[length] = n[i].neighbourhoods[j][length];
                }
            } while(n[i].neighbourhoods[j][length++].index != -1);
            length--;
            //printf("%i\n", length);
            if(length > blocksize) {
                free(b.blocks[b.count].elements);
                bool used[length];
                memset(used, 0, sizeof(used));
                findCombinations(&b, n[i].neighbourhoods[j], length, 0, 0, used, n[i].col);        
            } else {
                b.count++;
            }
            free(n[i].neighbourhoods[j]);
        }
        free(n[i].neighbourhoods);
        //printf("blockCount %i, totalBlockCount %i\n", b.count, n.blockCount);
    }
    free(n);
    return b;
}

struct blocks getAllBlocks(float dia) {
    struct neighbourhoods *allNeighbourhoods = malloc(cols * sizeof(struct neighbourhoods));
    int totalBlockCount =0;
    for(int i = 0; i < cols; i++) {
       // printf("\n-----------COLUMN %i---------\n",i);
        allNeighbourhoods[i] = getNeighbourhoods(i, dia);
        totalBlockCount += allNeighbourhoods[i].blockCount;
    }
    struct blocks allBlocks = getBlocks(allNeighbourhoods, cols, totalBlockCount);
    return allBlocks;
}


void getCollisions(struct blocks allBlocks) {
 //   char 
}
 
int main(int argc, char* argv[]) {
    loadMatrix();
    //loadKeys();
    
    int blocks = 0;


        //struct neighbourhoods n = getNeighbourhoods(i, 0.000001);
        //struct blocks b = getAllBlocks(0.000001);
        //blocks += b.count;
        //printBlocks(b);

    //while(string1[i] != '\0' || string2[i] != '\0') {
        
    //}
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

