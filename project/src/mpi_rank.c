/*
 * Compile:  mpicc -g -Wall -o mpi_rank mpi_rank.c
 * Run:      mpirun -np <number of processes> --hostfile <hostfile> ./mpi_rank
 */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "io.c"


typedef struct problem_size_t {
	int nodes;
	int edges;
} problem_size_t;

typedef struct result_ele_t {
	int idx;
	double rank;
} result_ele_t;


problem_size_t init_cluster(char *filename, int *part_sizes, int num_procs);
void receive_and_save(char *output_filename, int num_procs);
void sync_rank(graph_t *graph,
		double threshold,
		int total_size,
		int **out_by_idx,
		int **in_by_idx,
		int num_procs);

int main( int argc, char *argv[] )
{
	// problem-global data
	int num_procs;
	int *part_sizes;
	double time_start, time_end, threshold;
	problem_size_t prob_size;
	char *input_filename, *output_filename;

	// local data for each proc
	edge_t *my_edges;
	int *my_rank;


	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs );
	MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

	GET_TIME(time_start);

	io_dbg_lev = atoi(getenv("DEBUG"));

	/* Get number of threads and size from command line */
	if (argc != 3) usage(argv[0]);
	input_filename = argv[1];
	output_filename = argv[2];

	// if master, distribute work while rest wait
	if (my_rank == 0) {
		prob_size = init_cluster(input_filename, &part_sizes, num_procs);
	}
	MPI_Barrier( MPI_COMM_WORLD );

	// all procs receive work (master sends to self)



	// master receive result array from all (master sends self)
	if (my_rank == 0) {
		receive_and_save(output_filename, num_procs);
	}
	/* Record end time and report delta. */
	GET_TIME(time_end);
	printf("Elapsed time for size %d nodes, %d edges: %5.3fms\n",
			prob_size.nodes,
			prob_size.edges,
			time_end - time_start);

	MPI_Finalize();
	return 0;
} // main

/*-------------------------------------------------------------------*/
problem_size_t * init_cluster(char *filename, int *part_sizes, int num_procs)
{
	graph_t graph;
	int edge_count
	problem_size_t *prob_size;

	// load graph

	// partition graph

	// send work

	return prob_size;
}

/*-------------------------------------------------------------------*/
void receive_and_save(char *output_filename, int *part_sizes, int num_procs)
{
	// allocate receipt buffers for each proc

	// receive from all procs (including master self)

	// save each buffer to the same file
}

/*--------------------------------------------------------------------*/
void sync_rank(graph_t *graph,
		double threshold,
		int total_size,
		int **out_by_idx, //each proc: array of indices for ranks to send (-1 terminates)
		int **in_by_idx, //each proc: array of indices for ranks to recv (-1 terminates)
		int num_procs)
{
	double delta;

	rank_init(graph, total_size);

	while (delta >= threshold) {
		// rank iter on local graph

		// receive other rank arrays

		// include other rank arrays
	}
}

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <input filename> <output filename>\n", prog_name);
   exit(1);
}  // usage
