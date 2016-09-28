#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
/*
 * 
 */

float mat[20][10]; //test dimensions
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
            mat[i][j++] = atof(record);
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
    int dia = 0.000001;
    int 
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

int main(int argc, char* argv[]) {
    loadMatrix();
    loadKeys();
    
    /*
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < 20; i++) {
        printf("%s \n", keys[i]);
    }*/
    
    return (EXIT_SUCCESS);
}

