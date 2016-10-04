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

    //String reverser for easier addition
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

    int getSignature() {
        return 0;
    }

    //adds the 2 keys together
    char* addKey(char *key1, char *key2) {
        int longest = 1 + (strlen(key1) + strlen(key2))/2.0 + abs(strlen(key1) - strlen(key2))/2.0;
        char new1[longest];
        char new2[longest]; 
        memset(new1, '0', longest);
        memset(new2, '0', longest);
        strncpy(new1, key1, strlen(key1));
        strncpy(new2, key2, strlen(key2));    
        new1[longest] = '\0';
        new2[longest] = '\0';

        char placeValues[longest+1];
        memset(placeValues, '\0', sizeof(placeValues));
        int valueToAdd;
        int carryOver = 0;
        for (int i = 0; i<(longest-1); i++) {
            //while we arent at the end of the string
            while (i != (longest-2)) {
                valueToAdd = (new1[i] - '0') + (new2[i] - '0') + carryOver;
                carryOver = 0;
                if (valueToAdd > 9 ) {
                    valueToAdd = valueToAdd - 10;
                    carryOver++;
                }
                placeValues[i] = valueToAdd + '0';
                printf("%d placed at %d\n",valueToAdd,i);
                i++;    
            }
            valueToAdd = (new1[i] - '0') + (new2[i] - '0') + carryOver;
            carryOver = 0;
            //the case when we reach the end of the string, the final digit to add
            if (valueToAdd > 9) {
                valueToAdd = valueToAdd - 10;
                placeValues[(longest-2)] = valueToAdd + '0';
                printf("%d into 2nd last element\n",valueToAdd);
                carryOver++;
                placeValues[(longest-1)] = carryOver + '0';
                printf("%d into last element\n",carryOver);
            }
            else {
                placeValues[(longest-2)] = valueToAdd + '0';
                printf("%d placed at %d\n",valueToAdd,i);
            }
        }
        reverseKey(placeValues);        
        printf("%s",placeValues);
    }
       
    addKey("12", "91");




  return 0;
}
