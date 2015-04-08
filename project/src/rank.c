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

extern int io_dbg_lev;

/* Method declarations. */

void usage(char* prog_name);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  char    *input_filename, *output_filename;
  double  time_start, time_end;
  edge_t	 *edges;
  int			 edge_count;
  graph_t  graph, graph2; /* adjacency list for graph nodes. */

  io_dbg_lev = atoi(getenv("DEBUG"));
  
  /* Get number of threads and matrix size from command line */
  if (argc != 3) usage(argv[0]);
  input_filename = argv[1];
  output_filename = argv[2];

  /* Record start time */
  GET_TIME(time_start);

  /* Allocate and populate graph array. */
  if (load_input(input_filename, &graph))
    return 1;

  if (load_edges(input_filename, &edges, &edge_count))
  	return 1;

  graph_build(&graph2, edges, edge_count);

  /* Record end time and report delta. */
  GET_TIME(time_end);
  printf("Elapsed time for size %d: %5.3fms\n",
            0, // TODO should be graph->node_count
            time_end - time_start);

  /* Save output. */
  save_ranks(output_filename, &graph);

  /* TODO Deallocate graph array. */

  return 0;
} /* main */

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <input filename> <output filename>\n", prog_name);
   exit(1);
}  /* usage */

