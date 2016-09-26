/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Tom
 *
 * Created on 26 September 2016, 8:42 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * 
 */
int main(int argc, char** argv) {
	char buffer[1024];
	char *record, *line;
	int i = 0, j = 0;
	int mat[4400][500];
	FILE *fstream = fopen("\data.txt", "r");
	if (fstream == NULL)
	{
		printf("\n file opening failed ");
		return -1;
	}
	while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL)
	{
		record = strtok(line, ",");
		while (record != NULL)
		{
			printf("record : %s", record);    //here you can put the record into the array as per your requirement.
			mat[i][j++] = atoi(record);
			record = strtok(NULL, ",");
		}
		++i;
	}
	for (int k = 0; k < 100; k++) {
		printf(mat[k][0]);
	}

    printf("test");
    fprintf(stderr, "test");
    return (EXIT_SUCCESS);
}

