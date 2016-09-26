#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * 
 */

float mat[20][10]; //test dimensions
char keys[20];

void loadMatrix() {
	
	char buffer[1024];
	char *record, *line;
	int i = 0, j = 0;
	FILE *fstream = fopen("testdata.txt", "r");
	if (fstream == NULL)
	{
		printf("\n Opening matrix failed ");
		return -1;
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
}

void loadKeys() {
	char buffer[1024];
	char *record, *line;
	int i = 0;
	FILE *fstream = fopen("testkeys.txt", "r");
	if (fstream == NULL)
	{
		printf("\n Opening key vector failed ");
		return -1;
	}
	while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL)
	{
		record = strtok(line, " ");
		while (record != NULL)
		{
			//printf("Recording: %s to [%d] \n", record, i);
			keys[i] = record;
			record = strtok(NULL, " ");
			i++;
		}
	}
	
}

int main(int argc, char** argv) {
	loadMatrix();
	loadKeys();
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 10; j++) {
			printf("%f ", mat[i][j]);
		}
		printf("\n");
	}

	for (int i = 0; i < 20; i++) {
		printf("%s \n", keys[i]);
	}
	
	
    return (EXIT_SUCCESS);
}

