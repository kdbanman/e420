/* 
 * Kirby Banman
 * ECE 420 Lab 3
 * 26 Feb 2015
 *
 * File:  
 *    gaussj.c
 *
 * Purpose:
 *    Using the desired number of threads, solve a linear system.
 *    
 * Input:
 *    "data_input" file, 2 command line parameters (see Usage below)
 * Output:
 *    "data_output" file
 *
 * Compile:
 *    gcc -Wall -Wextra -lm -fopenmp -o bin/gaussj src/gaussj.c
 *
 * Usage:
 *    ./gaussj <thread_count> <problem_size>
 */

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "timer.h"

const int MAX_THREADS = 4096;
const int MAX_SIZE = 1000000;

/* Global thread count */
int     thread_count;  

/* Method declarations. */

void gaussj(float *sys_matr, float *solution, int size);

void gauss_elim(float *sys_matr, int *index, int size);
void pivot_row(int row, float *sys_matr, int *index, int size);
void eliminate_beneath_row(int row, float *sys_matr, int *index, int size);
float max_in_column(int row, float *sys_matr, int *index, int size);

void jordan_elim(float *sys_matr, int *index, int size);

void print_index(int *arr, int size);
void print_floats(float *arr, int size);
int load_input(int n, float* D, int mode);
int save_output(int n, float* b);

void usage(char* prog_name);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  double  time_start, time_end;
  float   *sys_matr, /* Augmented matrix linear system. */
          *solution; /* Solution vector. */
  int     size; /* Matrix size (number of cities). */

  /* Get number of threads and matrix size from command line */
  if (argc != 3) usage(argv[0]);

  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) usage(argv[0]);

  size = atoi(argv[2]);
  if (size <= 0 || size > MAX_THREADS) usage(argv[0]);

  /* Allocate and populate arrays. */
  sys_matr = malloc(size * (size + 1) * sizeof(float));
  solution = malloc( size * sizeof(float));

  load_input(size, sys_matr, 0);

  /* Record start time */
  GET_TIME(time_start);

  gaussj(sys_matr, solution, size);

  /* Record end time and report delta. */
  GET_TIME(time_end);
  printf("Elapsed time for %d threads and %d size: %5.3fms\n",
            thread_count,
            size,
            time_end - time_start);

  /* Save output. */
  save_output(size, solution);

  /* Deallocate arrays and semaphore. */
  free(sys_matr);
  free(solution);

  return 0;
} /* main */


/*--------------------------------------------------------------------*/
void gaussj(float *sys_matr, float *solution, int size)
{
  int     *index, /* Vector of row indices. */
          i; /* Miscellaneous index counter. */

  /* Handle trivial case. */
  if (size == 1) {
    solution[0] = sys_matr[1]/sys_matr[0];
    return;
  }

  /* Allocate and populate index tracker. */
  index=malloc(size * sizeof(int));
  for (i = 0; i < size; i++)
    index[i]=i;

  {
    gauss_elim(sys_matr, index, size);
    jordan_elim(sys_matr, index, size);
  }

  /* Write solution from elimination results. */
  for (i = 0; i < size; ++i)
    solution[i] = sys_matr[index[i]*(size+1)+size] /
                  sys_matr[index[i]*(size+1)+i];

  /* Deallocate */
  free(index);

} /* gaussj */

/*--------------------------------------------------------------------*/
void gauss_elim(float *sys_matr, int *index, int size)
{
  int row; /* row iteration index */

  for (row=0; row<size-1; ++row)
  {
    pivot_row(row, sys_matr, index, size);
    eliminate_beneath_row(row, sys_matr, index, size);
  }
} /* gauss_elim */

/*--------------------------------------------------------------------*/
void pivot_row(int row, float *sys_matr, int *index, int size)
{
  int swap_candidate, swap_tmp;
  
  /* find maximum row for current column */
  swap_candidate = max_in_column(row, sys_matr, index, size);

  /* swap if different than current row */
  if (swap_candidate != row)
    {
      swap_tmp = index[swap_candidate];
      index[swap_candidate] = index[row];
      index[row] = swap_tmp;
    }
} /* pivot_row */

/*--------------------------------------------------------------------*/
float max_in_column(int row, float *sys_matr, int *index, int size)
{
  float max;
  int max_row, i;

  max = 0;
  for (i=row; i<size; ++i)
    if (max<sys_matr[index[i]*(size+1)+row]*sys_matr[index[i]*(size+1)+row])
    {
      max=sys_matr[index[i]*(size+1)+row]*sys_matr[index[i]*(size+1)+row];
      max_row=i;
    }
  return max_row;
} /* max_in_column */

/*--------------------------------------------------------------------*/
void eliminate_beneath_row(int row, float *sys_matr, int *index, int size)
{
  int column, row_below;
  float coeff;
  
  #pragma omp parallel for num_threads(thread_count) \
    default(none) shared(sys_matr, size, index, row) \
    private(column, row_below, coeff)
  for (column=row+1; column<size; ++column)
  {
    coeff = sys_matr[index[column]*(size+1)+row] /
            sys_matr[index[row]*(size+1)+row];
    for (row_below=row; row_below<size+1; ++row_below)
      sys_matr[index[column]*(size+1)+row_below]-=
        sys_matr[index[row]*(size+1)+row_below]*coeff;
  }    
} /* eliminate_beneath_row */

/*--------------------------------------------------------------------*/
void jordan_elim(float *sys_matr, int *index, int size)
{
  int   column,
        row_above;

  float inverse_coeff; /* elimination multiplier fraction (per column) */

  /* eliminate elements in each column (except first) right to left
   * for each row above the columnar nonzero */

  for (column=size-1; column>0; --column)
  {
    #pragma omp parallel for num_threads(thread_count) \
      default(none) shared(sys_matr, size, index) \
      private(row_above, inverse_coeff) \
      firstprivate(column)
    for (row_above=column-1; row_above>=0; --row_above )
    {
      inverse_coeff = sys_matr[index[row_above]*(size+1)+column] /
             sys_matr[index[column]*(size+1)+column];

      sys_matr[index[row_above]*(size+1)+column]-=
        inverse_coeff*sys_matr[index[column]*(size+1)+column];

      sys_matr[index[row_above]*(size+1)+size]-=
        inverse_coeff*sys_matr[index[column]*(size+1)+size];
    } 
  }
} /* jordan_elim */

/*--------------------------------------------------------------------*/
void print_index(int *arr, int size)
{
  int i;

  for (i = 0; i < size; i++)
    printf("%d\n", arr[i]);
} /* print_idx */

/*--------------------------------------------------------------------*/
void print_floats(float *arr, int size)
{
  int i;

  for (i = 0; i < size; i++)
    printf("%f\n", arr[i]);
} /* print_floats */

/*--------------------------------------------------------------------*/
int load_input(int n, float* D, int mode)
{  
  /*
  Load the data stored in the file "data_input". mode specifies the loading content. 

  n is the size of the problem. D is the pointer to the destination array. mode specifies the target data. When mode=0, the augmented matrix of size n by (n+1) will be stored in the array D[]; when mode=1, only the coefficient matrix of size n by n will be stored in the array D[]; when mode=2, only the constant vector will be stored in the array D[]. 

  The function will first compare the n to the size stored in the file "data_input" and will exit if they don't match.
  
  When mode=0, the loaded array is [A|b], i.e., an n by (n+1) matrix, the element in the i th row and j th column will be stored in D[(n+1)*i+j].
  When mode=1, the loaded array is A, i.e., an n by n matrix, the element in the i th row and j th column will be stored in D[n*i+j].
  When mode=2, the loaded array is b, i.e., an n by 1 matrix, the element int he i th row will be stored in D[i].
  */
  FILE* fp;
  int i,j;
  float temp;

  if ((fp=fopen("data_input","r"))==NULL)
  {
    printf("Fail to load the input data!\n");
    return 1;
  }
  fscanf(fp, "%d\n\n", &i);
  if (i!=n)
  {
    printf("Problem size does not match!\n");
    fclose(fp);
    return 2;
  }
  switch(mode)
  {
    case 0:/*Load the augemented matrix*/
    {
      for (i=0; i<n; ++i)
      {
        for (j=0; j<n; ++j)
        {
          fscanf(fp, "%f\t", &temp);
          D[i*(n+1)+j]=temp;
        }
        fscanf(fp, "\n");
      }
      fscanf(fp,"\n");
      for (i=0; i<n; ++i)
      {
        fscanf(fp, "%f\n", &temp);
        D[i*(n+1)+n]=temp;
      }
    }
      break;
    case 1:/*Load the A*/
    {
      for (i=0; i<n; ++i)
        {
          for (j=0; j<n; ++j)
          {
            fscanf(fp, "%f\t", &temp);
            D[i*n+j]=temp;
          }
          fscanf(fp, "\n");
        }
    }
      break;
    case 2:/*Load the b*/
    {
      for (i=0; i<n; ++i)
      {
        for (j=0; j<n; ++j)
        {
                fscanf(fp, "%f\t", &temp);
        }
        fscanf(fp, "\n");
      }
      fscanf(fp,"\n");
      for (i=0; i<n; ++i)
      {
        fscanf(fp, "%f\n", &temp);
        D[i]=temp;
      }
    }
      break;
    default:
    {
      printf("Please specify the loading type!\n");
      fclose(fp);
      return -1;
    }
  }
  fclose(fp);
  return 0;
}

/*--------------------------------------------------------------------*/
int save_output(int n, float* b)
{
  /*
  Save the result in array b[] into the file "data_output"

  n is the problem size. b is the pointer to the to be stored array. 

  b[i] will be stored in the i th row of the vector written in the output file.
  */
  int i;
  FILE* fp;

  if ((fp=fopen("data_output", "w"))==NULL)
  {
    printf("Fail to open the file!\n");
    return 1;
  }

  fprintf(fp, "%d\n\n", n);
  for (i=0; i<n; ++i)
    fprintf(fp, "%f\n", b[i]);

  fclose(fp);
  return 0;
}

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <number of threads> <problem size>\n", prog_name);
   fprintf(stderr, "  0 < number of threads <= %d\n", MAX_THREADS);
   fprintf(stderr, "  0 < problem size <= %d\n", MAX_SIZE);
   exit(1);
}  /* usage */

