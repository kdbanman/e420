/* 
 * Kirby Banman
 * ECE 420 Lab 3
 * 26 Feb 2015
 *
 * File:  
 *    rank.c
 *
 * Purpose:
 *    Using the desired number of threads, solve pagerank
 *    
 * Input:
 *    command line parameters (see Usage below)
 * Output:
 *    output file as named in Usage
 *
 * Compile:
 *    gcc -Wall -Wextra -o bin/rank src/rank.c
 *
 * Usage:
 *    ./rank <input_filename> <output_filename>
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "timer.h"

/* Method declarations. */

void usage(char* prog_name);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  char    *input_filename, *output_filename;
  double  time_start, time_end;
  adj_t   *adj; /* adjacency list for graph nodes. */
  int     node_count; /* size of adjacency list. */

  debug_level = atoi(getenv("DEBUG"));
  
  /* Get number of threads and matrix size from command line */
  if (argc != 3) usage(argv[0]);
  input_filename = argv[1];
  output_filename = argv[2];

  /* Allocate and populate graph array. */
  if (load_input(input_filename, &adj, &node_count))
    return 1;

  /* Record start time */
  GET_TIME(time_start);

  /* Record end time and report delta. */
  GET_TIME(time_end);
  printf("Elapsed time for size %d: %5.3fms\n",
            node_count,
            time_end - time_start);

  /* Save output. */
  save_ranks(output_filename, adj, node_count);

  /* Deallocate graph array. */
  // TODO destroy every adj_t

  return 0;
} /* main */

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <input filename> <output filename>\n", prog_name);
   exit(1);
}  /* usage */

