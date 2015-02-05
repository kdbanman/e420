/* 
 * Kirby Banman
 * ECE 420 Lab 1
 * 15 Jan 2015
 *
 * File:  
 *    fwarshall_async.c
 *
 * Purpose:
 *    Using the desired number of threads, compute a shortest path matrix
 *    asynchronously.
 *    
 * Input:
 *    "data_input" file, 2 command line parameters (see Usage below)
 * Output:
 *    "data_output" file
 *
 * Compile:  gcc -g -Wall -Wextra -o fwarshall fwarshall.c -lpthread
 * Usage:    ./fwarshall_async <thread_count> <matrix_size>
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "barrier.c"
#include "timer.h"

const int MAX_THREADS = 4096;
const int MAX_SIZE = 1000000;

/* Global variables:  accessible to all threads */

int thread_count;  
int **w_cube; /* Weight cube.  layer k+1 is k-th iteration result. */
sem_t **sem_cube; /* Cube of semaphores to protect w_cube. */
int cube_depth;
int size; /* Matrix size (number of cities). */
int fw_k; /* Global Floyd-Warshall algorithm iteration counter. */

/* Method declarations. */

void *thread_func(void* param);
void floyd_warshall_iter(int k, int start, int length, int size);
int coord(int i, int j, int size);
int row(int coord, int size);
int col(int coord, int size);

int load_input(int *in, int size);
int save_output(int *out, int size);
int print_matrix(int *A, int size);
void usage(char* prog_name);

/* Thread param struct denoting matrix subset. */
typedef struct block_param {
  int start;
  int length;
} block_param;

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  int         layer;
  long        thread;
  pthread_t   *thread_handles; 
  block_param *thread_params;
  double      time_start, time_end;


  /* Get number of threads and matrix size from command line */
  if (argc != 3) usage(argv[0]);

  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) usage(argv[0]);

  size = atoi(argv[2]);
  if (size <= 0 || size > MAX_THREADS) usage(argv[0]);

  /* Allocate cube pointer. Populate contents. Initialze semaphores to 0. */
  cube_depth = size + 1;
  w_cube = (int **) malloc(cube_depth * sizeof(int *));

  sem_cube = (sem_t **) malloc(cube_depth * sizeof(sem_t *));

  for (layer = 0; layer < cube_depth; layer++) {
    w_cube[layer] = (int *) malloc(size*size * sizeof(int));
    load_input(w_cube[layer], size);

    sem_cube[layer] = (sem_t *) malloc(size*size * sizeof(sem_t));
    int sem_idx;
    for (sem_idx = 0; sem_idx < size*size; sem_idx++) {
      sem_init(&sem_cube[layer][sem_idx], 0, layer == 0 ? 1 : 0);
    }
  }

  /* Initialize threads */
  thread_handles = (pthread_t *) malloc (thread_count*sizeof(pthread_t)); 
  thread_params = (block_param *) malloc (thread_count*sizeof(block_param)); 

  /* Allocate work blocks to first thread. */
  thread_params[0].start = 0;
  thread_params[0].length = size*size / thread_count;
  /* Allocate work blocks to all but the final thread. */
  for (thread = 1; thread < thread_count - 1; thread++) {
    thread_params[thread].start = thread_params[thread-1].start +
                                  thread_params[thread-1].length;
    thread_params[thread].length = size*size / thread_count;
  }
  /* Allocate remaining work block to the final thread. */
  thread_params[thread].start = thread_params[thread-1].start +
                                thread_params[thread-1].length;
  thread_params[thread].length = size*size - thread_params[thread].start;

  /* Set global iteration counter. */
  fw_k = 0;

  /* Record start time */
  GET_TIME(time_start);

  /* Send work to threads. */
  for (thread = 0; thread < thread_count; thread++)  
    pthread_create(&thread_handles[thread],
                   NULL,
                   thread_func,
                   (void*) &thread_params[thread]);
  
  /* Synchronize threads. */
  for (thread = 0; thread < thread_count; thread++) 
    pthread_join(thread_handles[thread], NULL); 

  /* Record end time and report delta. */
  GET_TIME(time_end);
  printf("Elapsed time for %d threads and %d cities: %5.3fms\n",
            thread_count,
            size,
            time_end - time_start);

  save_output(w_cube[cube_depth - 1], size);

  /* Deallocate arrays and semaphore. */
  for (layer = 0; layer < cube_depth; layer++) {
    free(w_cube[layer]);
    free(sem_cube[layer]);
  }
  free(w_cube);
  free(sem_cube);

  return 0;
} /* main */

/*--------------------------------------------------------------------*/
void *thread_func(void* param)
{
  block_param * block;
  int k;

  /* Get assigned work */
  block = (block_param *) param;

  /* Perform assigned work */
  for (k = 0; k < size; k++) {
    floyd_warshall_iter(k, block->start, block->length, size);
  }

  return 0;
} /* thread_func */

/*--------------------------------------------------------------------*/
void set_weight(int k, int i, int j, int weight)
{
  for (; k < cube_depth; k++) {
    w_cube[k][coord(i, j, size)] = weight;
  }
} /* set_weight */

/*--------------------------------------------------------------------*/
void floyd_warshall_iter(int k, int start, int length, int size)
{
    int i, j, c, src_idx, dest_idx, self_idx, src, dest, self;

    for (c = start; c < start + length; c++) {
      i = row(c, size);
      j = col(c, size);

      src_idx = coord(i, k, size);
      dest_idx = coord(k, j, size);
      self_idx = coord(i, j, size);

      /* Ensure previous writes are complete before reading. 
       * No need to block during read, so immediatel release. TODO*/
      sem_wait(&(sem_cube[k][src_idx]));
      sem_post(&(sem_cube[k][src_idx]));
      src = w_cube[k][src_idx];

      sem_wait(&(sem_cube[k][dest_idx]));
      sem_post(&(sem_cube[k][dest_idx]));
      dest = w_cube[k][dest_idx];

      self = w_cube[k][self_idx];

      int candidate_weight = src + dest;
      if (candidate_weight < self) {
        set_weight(k + 1, i, j, candidate_weight);
      }
      
      /* Allow reading from current position now that path is calculated. */
      sem_post(&(sem_cube[k + 1][self_idx]));
    }
}

/*--------------------------------------------------------------------*/
int coord(int i, int j, int size)
{
  return i * size + j;
}

/*--------------------------------------------------------------------*/
int row(int coord, int size)
{
  return coord / size;
}

/*--------------------------------------------------------------------*/
int col(int coord, int size)
{
  return coord % size;
}

/*--------------------------------------------------------------------*/
int load_input(int *in, int size)
{
  /*
  Load the matrix stored in the file "data_input".
  
  A is the pointer to the destination matrix. n is the size of the matrix to be stored in the array A[].
  The function will first compare the n to the size stored in the file "data_input" and will exit if they don't match. Then the matrix in the file "data_input" is loaded into the array A[].
  The element in the i th row and j th column will be mapped to A[n*i+j].
  */

  FILE* ip;
  int i,j,temp;
  if ((ip=fopen("data_input","r"))==NULL)
  {
    printf("Error opening the input data.\n");
    return 1;
  }
  fscanf(ip,"%d\n\n",&temp);
  if (temp!=size)
  {
    printf("City count does not match the data!\n");
    return 2;
  }

  for (i=0;i<size;i++)
    for (j=0;j<size;j++)
      fscanf(ip,"%d\t",in+size*i+j);
  fclose(ip);
  return 0;
  
}

/*--------------------------------------------------------------------*/
int save_output(int *out, int size)
{
  /*
  Save the matrix stored in array A[] into the file "data_output".
  
  A is the pointer to the array storing the matrix. n is the size of the matrix.
  The function will first write n into the file "data_output". Then it will write the elements in A[].
  A[I] will be mapped as the element in the floor(I/n) th row and the I%n th column of a matrix, i.e. A[n*i+j] will be mapped as the elment in the i th row and j th column of a matrix.
  */
  FILE* op;
  int i,j;
  if ((op=fopen("data_output","w"))==NULL)
  {
    printf("Error opening the file.\n");
    return 1;
  }
  fprintf(op,"%d\n\n",size);
  for (i=0;i<size;i++)
  {
    for (j=0;j<size;j++)
      fprintf(op,"%d\t",out[size*i+j]);
    fprintf(op,"\n");
  }
  fclose(op);
  
  return 0;
}

/*--------------------------------------------------------------------*/
int print_matrix(int* A, int size)
{
  /*
  Print the matrix stored in array A[] on the screen.
  */
  int i,j;
  for (i=0;i<size;i++)
  {
    for (j=0;j<size;j++)
      printf("%d\t", A[i*size+j]);
    printf("\n");
  }
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

