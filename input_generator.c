/* this program is used to generate the input file of our parallel 
 * QuickSort Algorithm.
 * You just have to set the SIZE and 
 * the filename as input_SIZE.txt
 *
 * Compiling:
 *   gcc   input_generator.c -o input_generator
 *
 * Running:
 *   ./input_generator
 *
 *
 * File: input_generator.c		Author: M. Soulemane
 * Date: 25.10.2015
 *
 */

  #include<stdio.h>
  #include<stdlib.h>
  #define SIZE 10                       /* MAX ARRAY SIZE CONSTANT     */


  int main (void) {
  
  static long long globaldata[SIZE];   /* main input array            */
  long long value;
  long long i = 0;
  long long tmp=0;
     
  FILE *inp;                           
  inp = fopen ("input_10.txt","w+");
  for (i=0; i<SIZE; i++) {
    value = rand ()% SIZE;             /* Value range (0 - SIZE)     */
    fprintf (inp, "%lld \t", value);
  }                                                                    
  fclose (inp);                        /* Close the file             */
  
  inp = fopen ("input_10.txt","r");     /* Open file for reading      */
   for (i=0; i<SIZE; i++) {
    fscanf (inp, "%lld \t", &tmp);
    globaldata[i] = tmp;
    printf ("%lld \t", globaldata[i]);               
  }
  fclose (inp); 

  return 0;
 }
