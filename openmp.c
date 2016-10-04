#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int main ( int argc, char *argv[] ) {
    int id;
    double wtime;
    char keys[20][14];

    printf ( "\n" );
    printf ( "HELLO_OPENMP\n" );
    printf ( "  C/OpenMP version\n" );

    printf ( "\n" );
    printf ( "  Number of processors available = %d\n", omp_get_num_procs ( ) );
    printf ( "  Number of threads =              %d\n", omp_get_max_threads ( ) );

    wtime = omp_get_wtime ( );

    printf ( "\n" );
    printf ( "  OUTSIDE the parallel region.\n" );
    printf ( "\n" );

    id = omp_get_thread_num ( );
    printf ( "  HELLO from process %d\n", id ) ;

    printf ( "\n" );
    printf ( "  Going INSIDE the parallel region:\n" );
    printf ( "\n" );
/*
  INSIDE THE PARALLEL REGION, have each thread say hello.
*/
    int a[10];

    #pragma omp parallel for
    for (int i = 0; i<10; i++) {
        a[i] = 2 * i;
        printf("Added %d on thread: %d\n", 2*i, omp_get_thread_num());
    }

    for (int i = 0; i<10; i++) {
        printf("A[%d]:%d\n", i, a[i]);  
    }
/*
  Finish up by measuring the elapsed time.
*/
    //wtime = omp_get_wtime ( ) - wtime;

    //printf ( "\n" );
    //printf ( "  Back OUTSIDE the parallel region.\n" );
/*
  Terminate.
*/
    //printf ( "\n" );
    //printf ( "HELLO_OPENMP\n" );
    //printf ( "  Normal end of execution.\n" );

    //printf ( "\n" );
    //printf ( "  Elapsed wall clock time = %f\n", wtime );

    /*
    struct num {
        int ndigits;
        char d[14];
    }
    */

    //stores the key in a struct
    /*storeKey(char[] key) {
        struct num n = 
    }
    */

    //Reverse the key for easier addition
    void reverseKey(char *key) {
        char temp;
        int i = 0;
        int j = strlen(key) - 1;
        while (i < j) {
            temp = key[i];
            key[i] = key[j];
            key[j] = temp;
            i++;
            j--;
        }
    }
    
    int loadKeys() {
        char buffer[1024];
        char *record, *line;
        int i = 0;
        FILE *fstream = fopen("testkeys.txt", "r");
        if (fstream == NULL) {
            printf("\n Opening key vector failed ");
            return EXIT_FAILURE;
        }
        while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL) {
            record = strtok(line, " ");
            while (record != NULL) {
                reverseKey(record);
                printf("Recording: %s to [%d] \n", record, i);
                strcpy(keys[i], record);
                keys[i][14] = '\0';
                record = strtok(NULL, " ");
                i++;
            }
        }
        return EXIT_SUCCESS;
    }

    char key[] = "12135267736472";
    reverseKey(key);
    //printf("%s", key);
    loadKeys();

  return 0;
}
