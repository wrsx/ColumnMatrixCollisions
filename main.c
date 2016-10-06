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
#define keysize 14
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
    long long signature;
    int col;
    struct element *elements;
};

struct blocks {
   int count;
   struct block *blocks;
};

struct collisions {
    int count;
    struct blocks *collisions;
};

struct element data[cols][rows];
long long keys[rows];

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
    FILE *fstream = fopen("keys.txt", "r");
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
            /*
            strcpy(keys[i], record);
            keys[i][14] = '\0';*/
            keys[i] = atoll(record);
            record = strtok(NULL, " ");
            i++;
        }
    }
        return EXIT_SUCCESS;
}


void printBlocks(struct blocks b) {
    for(int i = 0; i < b.count; i++) {
        int j = 0;
        printf("col %i [", b.blocks[i].col);          
        while(b.blocks[i].elements[j].index != -1) {
            printf("[%i] %f, key %lld ",b.blocks[i].elements[j].index, b.blocks[i].elements[j].value, keys[b.blocks[i].elements[j].index]);
            j++;
        }
        printf("] - sig %lld\n", b.blocks[i].signature);
    } 

}

void printCollisions(struct collisions c) {
    for(int i = 0; i < c.count; i++) {
        struct blocks b = c.collisions[i];
        printf("\nsignature %llu\n", b.blocks[0].signature);
        for(int j = 0; j < b.count; j++) {
            struct block bl = b.blocks[j];
            printf("col %i: [", bl.col);
            for(int k = 0; k  < blocksize; k++) {
                printf("%f, ", bl.elements[k].value);
            }
            printf("]\n");
        }
    }
}

long long getSignature(struct element elements[]) {
    long long signature = 0;
    for (int i=0; i<blocksize; i++) {
        struct element e = elements[i];
        signature += keys[e.index];
    }
    return signature;
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
    
    if(elem1->signature < elem2->signature) {
        return -1;
    }
    return (elem1->signature > elem2->signature);
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
        b->blocks[b->count].signature = getSignature(b->blocks[b->count].elements);
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

struct blocks getBlocks(struct neighbourhoods *n, int neighbourhoodsCount, int totalBlockCount) {
    struct blocks b;
    b.blocks = malloc(totalBlockCount * sizeof(struct block));
    b.count = 0;
    for(int i = 0; i < neighbourhoodsCount; i++) {
        for(int j = 0; j < n[i].count; j++) { 
            int length = 0;
            while(n[i].neighbourhoods[j][length].index != -1) length++;
            bool used[length];
            memset(used, false, sizeof(used));
            findCombinations(&b, n[i].neighbourhoods[j], length, 0, 0, used, n[i].col);        
            free(n[i].neighbourhoods[j]);
        }
        free(n[i].neighbourhoods);
    }
    free(n);
    return b;
}

struct blocks getAllBlocks(float dia) {
    struct neighbourhoods *allNeighbourhoods = malloc(cols * sizeof(struct neighbourhoods));
    int totalBlockCount =0;
    for(int i = 0; i < cols; i++) {
        allNeighbourhoods[i] = getNeighbourhoods(i, dia);
        totalBlockCount += allNeighbourhoods[i].blockCount;
    }
    struct blocks allBlocks = getBlocks(allNeighbourhoods, cols, totalBlockCount);
    qsort(allBlocks.blocks, allBlocks.count, sizeof(struct block), blockComp);
    return allBlocks;
}

struct collisions getCollisions(struct blocks allBlocks) {
    struct collisions c;
    c.collisions = malloc(allBlocks.count * sizeof(struct blocks));
    
    struct block *currentBlock;
    int collisionCount = 0;
    
    int i = 0;

    while(i < allBlocks.count) {
        int blockCount = 0;        
        do {
            currentBlock = &allBlocks.blocks[i++];
            blockCount++;
        } while(currentBlock->signature == allBlocks.blocks[i].signature);
        //collision found
        if(blockCount > 1) {
            c.collisions[collisionCount].blocks = malloc(blockCount * sizeof(struct block));
            c.collisions[collisionCount].count = blockCount;
            for(blockCount; blockCount > 0; blockCount--) {
                c.collisions[collisionCount].blocks[blockCount-1] = allBlocks.blocks[i-blockCount];
            }
            collisionCount++;
        }
    }
    c.count = collisionCount;
    return c;
}
 
int main(int argc, char* argv[]) {
    loadMatrix();
    loadKeys();
    
    struct blocks b = getAllBlocks(0.000001);
    printf("total memory usage should be %ldmb\n", (b.count *(sizeof(struct block) * 4 * sizeof(struct element)))/1000000);
    //struct collisions c = getCollisions(b);
    //printCollisions(c);
                                   
    return (EXIT_SUCCESS);
}
