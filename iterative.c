/*
 * This program implements the iterative quicksort algorithm in parallel
 * using the OpenMPI library. The implementation is done in the 
 * following steps:
 *
 * 0. Interative version of quick sort
 * 1. read input from file
 * 2. Shows them on the screen
 * 3. Add MPI
 * 4. Start the wall timer
 * 5. Master distribute globaldata to all processes localdata
 * 6. Sort each localdata
 * 7. Gather them to the globaldata
 * 8. Sort semi-sorted globaldata
 * 9. Stop the wall timer
 * 10. Write the duration and final sorted data to the output file
 * 11. Add sorting checker
 * 12. input and output files entered as command line arguments
 * 13. Get input size from the input filename
 *
 * Compiling:
 *   mpicc   iterative.c -o iterative
 *
 * Running:
 * mpirun -np [number process] <program name> <input file> <output file>
 * e.g: mpirun -np 10 iterative input_100.txt out_iterative.txt
 *
 *
 * File: iterative.c		Author: M. Soulemane
 * Date: 18.01.2016     	version: v0.1
 *
 * History: none
 *
 */



#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MASTER 0                        /* root process's rank          */

void quickSortIterative (long long [], long long, long long);
long long partition (long long [], long long , long long );
void swap (long long [], long long , long long );
void sortCheckers (long long, long long []);
long long getSize ( char str[] );

int main (int argc, char **argv) {
  long long SIZE = 1;                   /* input size  read from file   */
  char strsize[]="";                    /* extracted filename           */
  /*  rank is the rank of the calling process in the communicator       */
  int rank;
  long long i;                          /* loop variable                */
  long long retscan;                    /* return value of scanf        */
  long long tmp;                        /* temporary variable           */
  double t_start, t_end;                /* variable used for clock time */
  long long test_size=5;                /* test loop size's variable    */
  FILE  *out, *inp;                     /* declare output/input stream  */
  int npes ;                            /* number of processes          */
  long long *globaldata = NULL;         /* for global array pointer     */
  long long *localdata = NULL;          /* for local  array pointer     */
  long long localsize;                  /* for local array size         */

  /*Checking that the run format is properly entered                    */
  if (argc != 3) {
    printf ("\n Properly specify the executable, input , output files");
    printf ("\nmpirun -np <process nber> %s <input file> <output file>\n"
                                                            , argv[0]);
    exit (1);
  }
  strcpy (strsize, argv[1]);
  SIZE = getSize (strsize);             /* get the SIZE                 */

  /* Initialize the MPI execution environment                           */
  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &npes);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  if (rank == MASTER) {
    globaldata = malloc (SIZE * sizeof (long long) );
    if (globaldata == NULL) {
      printf ("\n\n globaldata Memory Allocation Failed !  \n\n ");
      exit (EXIT_FAILURE);
    }
  inp = fopen (argv[1], "r");           /* Open file for reading        */
  if (inp == NULL) {
    printf ("\n\n inp Memory Allocation Failed !  \n\n ");
    exit (EXIT_FAILURE);
  }
  printf ("\n\nInput Data \n\n ");
  for (i = 0; i < SIZE; i++) {
    retscan = fscanf (inp, "%lld \t", &tmp);
    globaldata[i] = tmp;
  }

  printf ("\n\n End Input Data");

  fclose (inp);
  /*
  printf ("\n\nProcessor %d has data: ", rank);
  for ( i=0; i<test_size; i++) {
    printf ("%lld \t", globaldata[i]);
  }
  */
  printf ("\n");
  }


  /*Start wall  time                                                    */
  if (rank == MASTER)
  {
    t_start = MPI_Wtime ();
  }

  /*Getting the size to be used by each process                         */
  if (SIZE < npes) {
    printf ("\n\n SIZE is less than the number of process!  \n\n ");
    exit (EXIT_FAILURE);
  }
  localsize = SIZE/npes;
  
  /* Allocate memory to localdata of size localsize                     */
  localdata = (long long*) malloc (localsize* sizeof (long long));
  if (localdata == NULL) {
    printf ("\n\n localdata Memory Allocation Failed !  \n\n ");
    exit (EXIT_FAILURE);
  }

  /*Scatter the integers to each number of processes (npes)             */
  MPI_Scatter (globaldata, localsize, MPI_LONG_LONG, localdata,
             localsize, MPI_LONG_LONG, MASTER, MPI_COMM_WORLD);

  /* Perform local sort on each sub data by each process                */
  quickSortIterative (localdata, 0, localsize-1);

  /*
  printf ("\n\nProcessor %d has sorted data \n\n", rank);
	for ( i=0; i < test_size; i++) {
    printf ("%lld \t", localdata[i]);
  }
  */

  /* Merge locally sorted data of each process by MASTER to globaldata  */
  MPI_Gather (localdata, localsize, MPI_LONG_LONG, globaldata,
            localsize, MPI_LONG_LONG, MASTER, MPI_COMM_WORLD);
  free (localdata);

  if (rank == MASTER) {
  /* Final sorting                                                      */
  quickSortIterative (globaldata, 0, SIZE-1);

  /* End wall  time                                                     */
  t_end = MPI_Wtime ();

  /* Opening output file to write sorted data                           */
  out = fopen (argv[2], "w");
  if (out == NULL) {
    printf ("\n\n out Memory Allocation Failed !  \n\n ");
    exit (EXIT_FAILURE);
  }
  /* Write information to output file                                   */
  fprintf (out, "Iteratively Sorted  Data : ");
  fprintf (out, "\n\nInput size : %lld\t", SIZE);
  fprintf (out, "\n\nNber processes : %d\t", npes);
  fprintf (out, "\n\nWall time      : %7.4f\t", t_end - t_start);
  printf ("\n\nWall time      : %7.4f\t", t_end - t_start);
  fprintf (out, "\n\n");
  for (i = 0; i < SIZE; i++) {
    fprintf (out, " %lld \t", globaldata[i]);
  }
  fclose (out);                          /* closing the file            */
  /*
  for ( i = 0; i < test_size; i++) {
    printf ("%lld \t", globaldata[i]);
  }
  */
  printf ("\n\n");

  /* checking if the final globaldata content is properly sorted        */
  sortCheckers (SIZE, globaldata);
  printf ("\n\n");

  }

  if (rank == MASTER) {
    free (globaldata);                  /* free the allocated memory    */
  }

  /* MPI_Finalize Terminates MPI execution environment                  */
  MPI_Finalize ();

  return EXIT_SUCCESS;
}

/* This function divides elements of an array around a pivot element. All
 * elements less than  or equal to the pivot go on the left side and
 * those greater than the pivot go on the right side.
 *
 * Input:		x	input array
 *              first   leftmost element
 *              last    rightmost element
 * Output		none
 * Return value:	j is returned as the index of the pivot element
 * Sideeffects:		none
 *
 */
long long partition (long long x[], long long first, long long last)
{
  long long pivot;                       /* pivot variable              */
  long long  j, i;                       /* loop variable               */
  pivot = first;
  i = first;
  j = last;

  while (i < j) {
	/* move to the right                                                */
    while (x[i ] <= x[pivot] && i < last) {
      i++;
    }

    /* move to the left                                                 */
    while (x[j] > x[pivot]) {
      j--;
    }
    if (i < j) {
      swap (x, i, j);                   /* swap i and j                 */
    }
  }

  swap (x, pivot, j);                   /* swap pivot and j             */

  return j;
}


/* Swap elements at index m and n of the array s.
 *
 * Input:		s	array
 *              m   left index
 *              n   right index
 * Output		none
 * Return value:	none
 * Sideeffects:		none
 *
 */
void swap (long long s[], long long m, long long n) {
  long long tmp;                        /* temporary variable           */
  tmp = s[m];
  s[m] = s[n];
  s[n] = tmp;
}

/* This function iteratively sort an array arr
 *
 * Input:		arr	input array
 *              l   leftmost element
 *              h   rightmost element
 * Output		none
 * Return value:	none
 * Sideeffects:		none
 *
 */
void quickSortIterative (long long arr[], long long l, long long h)
{

  long long stack[ h - l + 1 ];         /*  Create an auxiliary stack   */
  long long top = -1;                   /*  initialize top of stack     */

  /*   push initial values of l and h to stack                          */
  stack[ ++top ] = l;
  stack[ ++top ] = h;

  /*  Keep popping from stack while is not empty                        */
  while ( top >= 0 )
  {
    /* Pop h and l                                                      */
    h = stack[ top-- ];
    l = stack[ top-- ];

    /* Set pivot element at its correct position in sorted array        */
    long long p = partition( arr, l, h );

    /* If there are elements on left side of pivot, then push left      */
    /* side to stack                                                    */
    if ( p-1 > l ) {
      stack[ ++top ] = l;
      stack[ ++top ] = p - 1;
    }

    /*  If there are elements on right side of pivot, then push right   */
    /* side to stack                                                    */
    if ( p+1 < h ) {
      stack[ ++top ] = p + 1;
      stack[ ++top ] = h;
    }
  }
}

/* Checking a list of sorted numbers. Make sure that each number is
 * less or equal to its immediate right neighbours / greater or equal to
 * its immediate left value.
 *
 * input parameters:	SIZE  total number of sorted items
 *                      input array containing sorted items
 *
 * output parameters:	input[index-1], input[index] shown on failure
 * return value:	none
 * side effects:	none
 *
 */
void sortCheckers (long long SIZE, long long input[]) {
  long long i;                           /* loop variable               */

  for (i = 1; i < SIZE; i++) {
    if (input[i-1] >  input[i]) {
      printf ("\n\n%lld -- %lld \t", input[i-1], input[i]);
      printf ("\n\nCheck failed. Array not sorted");
      break;
    }

  }

  printf("\n\nCheck successfully completed. Array Sorted");
}




/*  Extract size from a string and converts it to a number of type
 *  long long using some c built-in functions.
 *
 *
 * Input:		str   represents the extracted input filename from the
 *                    command line argument.
 * Output:		    none
 * Return value:	return the size of type long long
 * Sideeffects:		none
 *
 */
long long getSize ( char str[] ) {
  /*  This function split the input filename and get the size           */
  char * pch;                           /* strtok return value          */
  long long count = 0;                  /* counter variable             */
  char * e;                             /* parameter of strtoll function*/

  long long inpsize = 0;
  /*
   * The strtok() function breaks a string into a sequence of zero or
   * more nonempty tokens.  On the first call to strtok() the string to
   * be parsed should be specified in str.  In each subsequent call that
   * should parse the same string, str must be NULL.
   *
   */
  pch = strtok (str," ._");

  while ( pch != NULL )
  {
    if (count == 1 ) {
	  /* Convert string to unsigned long long integer (function )       */
	  inpsize = strtoll ( pch, &e, 10 );
    return inpsize;
	}
  pch = strtok ( NULL, " .-" );
  count ++;
  }

}
