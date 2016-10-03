#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main ( int argc, char *argv[] ) {
  int id;
  double wtime;

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
  wtime = omp_get_wtime ( ) - wtime;

  printf ( "\n" );
  printf ( "  Back OUTSIDE the parallel region.\n" );
/*
  Terminate.
*/
  printf ( "\n" );
  printf ( "HELLO_OPENMP\n" );
  printf ( "  Normal end of execution.\n" );

  printf ( "\n" );
  printf ( "  Elapsed wall clock time = %f\n", wtime );

  return 0;
}
