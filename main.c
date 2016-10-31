#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
//#include <omp.h>
//#include <libiomp/omp.h>
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

//a group of elements, either a block or neighbourhood
struct elementGroup {
    int count;
    int col;
    //optional (used in block)
    long long signature;
    struct element *elements;
};

struct elementGroups {
    int count;
    //optional (used in neighbourhood)
    int blockCount;
    //optional (used in neighbourhood)
    int col;
    struct elementGroup *groups;
};

struct collisions {
    int count;
    struct elementGroups *collisions;
};

struct element data[cols][rows];
long long keys[rows];
int z =0;

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
            keys[i] = atoll(record);
            record = strtok(NULL, " ");
            i++;
        }
    }
    return EXIT_SUCCESS;
}


void printBlocks(struct elementGroups blocks) {
    for(int i = 0; i < blocks.count; i++) {
        int j = 0;
        printf("col %i [", blocks.groups[i].col);
        while(blocks.groups[i].elements[j].index != -1) {
            printf("[%i] %f, key %lld ",blocks.groups[i].elements[j].index, blocks.groups[i].elements[j].value, keys[blocks.groups[i].elements[j].index]);
            j++;
        }
        printf("] - sig %lld\n", blocks.groups[i].signature);
    }
    
}

void printCollisions(struct collisions c) {
    for(int i = 0; i < c.count; i++) {
        struct elementGroups blocks = c.collisions[i];
        printf("\nsignature %llu\n", blocks.groups[0].signature);
        for(int j = 0; j < blocks.count; j++) {
            struct elementGroup block = blocks.groups[j];
            printf("col %i: [", block.col);
            for(int k = 0; k  < blocksize; k++) {
                printf("%f, ", block.elements[k].value);
            }
            printf("]\n");
        }
    }
}

long long getSignature(struct element elements[]) {
    long long signature = 0;
    for (int i = 0; i<blocksize; i++) {
        struct element e = elements[i];
        //printf("%d\n", e.index);
        //printf("%lld\n", keys[e.index]);
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

int groupComp(const void* p1, const void* p2) {
    const struct elementGroup *elem1 = p1;
    const struct elementGroup *elem2 = p2;
    
    if(elem1->signature < elem2->signature) {
        return -1;
    }
    return (elem1->signature > elem2->signature);
}


struct elementGroups getNeighbourhoods(int col, float dia) {
    //sort the column by size of the value
    struct element *column = data[col];
    qsort(column, rows, sizeof(struct element), elementComp);
    
    struct elementGroups neighbourhoods;
    neighbourhoods.groups = malloc(cols * sizeof(struct elementGroup));
    neighbourhoods.count = 0;
    neighbourhoods.blockCount = 0;
    neighbourhoods.col = col;
    
    struct element temp[rows];
    memset(&temp, -1, sizeof(temp));
    
    float min = 0, max = 0;
    int neighbourhoodSize = 0;
    int lastNeighbourhoodSize = 0;
    
    for(int i = 0; i < rows; i++) {
        //fprintf(stderr,"[%d] %f\n",col[i].index, col[i].value);
        if(temp[0].index == -1) {
            min = max = column[i].value;
            temp[neighbourhoodSize++] = column[i];
        } else {
            if (fabs(column[i].value - min) < dia && fabs(column[i].value - max) < dia) {
                temp[neighbourhoodSize++] = column[i];
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
                if((temp[0].index != -1) && (neighbourhoodSize >= blocksize) && (neighbourhoodSize >= lastNeighbourhoodSize)) {
                    if(neighbourhoodSize > blocksize) { //need to calculate the total combinations of
                        //formula for working out combinations of size k(blocksize) for n(tempcount) values
                        neighbourhoods.blockCount += round(exp(lgamma(neighbourhoodSize+1)-lgamma(neighbourhoodSize-blocksize+1))/tgamma(blocksize+1));
                    } else {
                        neighbourhoods.blockCount++;
                    }
                    //allocate the memory to store the neighbourhoods
                    neighbourhoods.groups[neighbourhoods.count].count = neighbourhoodSize;
                    neighbourhoods.groups[neighbourhoods.count].elements = malloc(neighbourhoodSize * sizeof(struct element));
                    //loops over all the elements in temp and adds them to the neighbourhoods struct
                    for(int j = 0; j < neighbourhoodSize; j++) {
                        neighbourhoods.groups[neighbourhoods.count].elements[j] = temp[j];
                    }
                    neighbourhoods.count++;
                }
                min = max = column[i].value;
                memset(&temp, -1, sizeof(temp));
                i = i - neighbourhoodSize;
                lastNeighbourhoodSize = neighbourhoodSize;
                neighbourhoodSize = 0;
            }
        }
    }
    return neighbourhoods;
}

void findCombinations(struct elementGroups *blocks, struct elementGroup neighbourhood, int start, int currLen, bool used[]) {
    if (currLen == blocksize) {
        int blockCount;
        #pragma omp atomic capture
        {
            blockCount = blocks->count;
            blocks->count++;
        }   
        blocks->groups[blockCount].elements = malloc(blocksize * sizeof(struct element));
        blocks->groups[blockCount].col = neighbourhood.col;
        int elementCount = 0;
        for (int i = 0; i < neighbourhood.count; i++) {
            if (used[i] == true) {
                blocks->groups[blockCount].elements[elementCount++] = neighbourhood.elements[i];
                //printf("%d\n",neighbourhood.elements[i].index);
            }
        }
        //printf("\n\n");
        blocks->groups[blockCount].count = elementCount;
        blocks->groups[blockCount].signature = getSignature(blocks->groups[blockCount].elements);
        return;
    }
    if (start == neighbourhood.count) {
        return;
    }
    
    used[start] = true;
    findCombinations(blocks, neighbourhood, start + 1, currLen + 1, used);
    
    used[start] = false;
    findCombinations(blocks, neighbourhood, start + 1, currLen, used);
}

struct elementGroups getBlocks(struct elementGroups neighbourhoods) {
    struct elementGroups blocks;
    blocks.groups = malloc(neighbourhoods.blockCount * sizeof(struct elementGroup));
    blocks.count = 0;
    //#pragma omp parallel for
    for(int i = 0; i < neighbourhoods.count; i++) {
        int length = neighbourhoods.groups[i].count;
        bool used[length];
        memset(used, false, sizeof(used));
        findCombinations(&blocks, neighbourhoods.groups[i], 0, 0, used);
    }
    return blocks;
}

struct elementGroups getAllNeighbourhoods(float dia) {
    struct elementGroups temp[cols];
    int totalBlockCount = 0;
    int totalNeighbourhoodCount = 0;
    //#pragma omp parallel for
    for(int i = 0; i < cols; i++) {
        temp[i] = getNeighbourhoods(i, dia);
        totalBlockCount += temp[i].blockCount;
        totalNeighbourhoodCount += temp[i].count;
    }
    struct elementGroups neighbourhoods;
    neighbourhoods.groups = malloc(totalNeighbourhoodCount * sizeof(struct elementGroup));
    neighbourhoods.count = 0;
    neighbourhoods.blockCount = totalBlockCount;
    for(int i = 0; i < cols; i++) {
        struct elementGroups column = temp[i];
        for(int j = 0; j < column.count; j++) {
            neighbourhoods.groups[neighbourhoods.count++] = column.groups[j];
        }
    }
    return neighbourhoods;
}

struct collisions getCollisions(struct elementGroups blocks) {
    qsort(blocks.groups, blocks.count, sizeof(struct elementGroup), groupComp);

    struct collisions c;
    c.collisions = malloc(blocks.count * sizeof(struct elementGroups));  
    struct elementGroup *currentBlock;
    int collisionCount = 0;
    
    int i = 0;
    
    while(i < blocks.count) {
        int blockCount = 0;
        int trueBlockCount = 0;
        //this is used to ensure collisions do not occur inside the same column
        bool columns[cols];
        memset(columns, false, sizeof(columns));
        do {
            currentBlock = &blocks.groups[i++];
            //if no collision in that column
            if(columns[currentBlock->col] == false) {
                trueBlockCount++;
                columns[currentBlock->col] = true;
            }
            blockCount++;
        } while(currentBlock->signature == blocks.groups[i].signature);
        //collision found
        if(trueBlockCount > 1) {
            c.collisions[collisionCount].groups = malloc(trueBlockCount * sizeof(struct elementGroup));
            c.collisions[collisionCount].count = trueBlockCount;
            for(blockCount; blockCount > 0; blockCount--) {
                //if first instance of collision in that column
                if(columns[blocks.groups[i-blockCount].col] == true) {
                    c.collisions[collisionCount].groups[--trueBlockCount] = blocks.groups[i-blockCount];
                    columns[blocks.groups[i-blockCount].col] = false;
                }                
            }
            collisionCount++;
        }
    }
    printf("colcount %d\n", collisionCount);
    c.count = collisionCount;
    return c;
}

int main(int argc, char* argv[]) {
    clock_t start = clock();
    loadMatrix();
    loadKeys();
    int msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Time taken to load data: %d seconds %d milliseconds\n", msec/1000, msec%1000);

    start = clock();
    struct elementGroups n = getAllNeighbourhoods(0.000001); 
    msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Time taken to find %d neighbourhoods: %d seconds %d milliseconds\n", n.count, msec/1000, msec%1000);

    start = clock();
    struct elementGroups b = getBlocks(n);
    msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Time taken to find %d blocks: %d seconds %d milliseconds\n", b.count, msec/1000, msec%1000);

    start = clock();
    struct collisions c = getCollisions(b);
    msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Time taken to find %d collisions: %d seconds %d milliseconds\n", c.count, msec/1000, msec%1000);   



    
    return (EXIT_SUCCESS);
}
