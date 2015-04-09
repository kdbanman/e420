/*
 * Compile:  mpicc -g -Wall -o mpi_rank mpi_rank.c
 * Run:      mpirun -np <number of processes> --hostfile <hostfile> ./mpi_rank
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "timer.h"
#include "rank_util.h"
#include "mpi.h"

/**********************
 *                 DATA
 */

typedef struct problem_size_t {
	int nodes;
	int edges;
} problem_size_t;

typedef struct result_ele_t {
	int idx;
	double rank;
} result_ele_t;

/**********************
 *  FUNCTION SIGNATURES
 */

problem_size_t * init_cluster(
		char *filename,
		int **part_sizes,
		int num_procs);

void partition_graph_simple(
		graph_t *graph,
		int ***nodes,		      // addr of array[part][idx] = node
		int **partitions,    // addr of array[node] = part
		int **node_counts,
		int num_procs);

void transform_send_partition(
		graph_t *graph,
		int **nodes, // array of array[proc][idx] = node
		int *partitions,  // array of array[node_idx] = proc
		int *node_counts, // length of each above
		int num_procs);

void send_partition(
		graph_t *graph,
		int **nodes,
		int *node_counts,
		int **edge_pairs,
		int *edge_counts,
		int num_procs);

void receive_and_save(
		char *output_filename,
		int *part_sizes,
		int num_procs);

void sync_rank(graph_t *graph,
		double threshold,
		int total_size,
		int **out_by_idx, //each proc: array of indices for ranks to send (-1 terminates)
		int **in_by_idx, //each proc: array of indices for ranks to recv (-1 terminates)
		int num_procs);

void usage(char* prog_name);

/**********************
 *                 MAIN
 */

int main( int argc, char *argv[] )
{
	// problem-global data
	int num_procs;
	int *part_sizes;
	double time_start, time_end, threshold;
	problem_size_t * prob_size;
	char *input_filename, *output_filename;

	// local data for each proc
	edge_t *my_edges;
	int my_rank;


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
		receive_and_save(output_filename, part_sizes, num_procs);
	}
	/* Record end time and report delta. */
	GET_TIME(time_end);
	printf("Elapsed time for size %d nodes, %d edges: %5.3fms\n",
			prob_size->nodes,
			prob_size->edges,
			time_end - time_start);

	MPI_Finalize();
	return 0;
} // main

/**********************
 *            FUNCTIONS
 */

/*-------------------------------------------------------------------*/
problem_size_t * init_cluster(
		char *filename,
		int **part_sizes,
		int num_procs)
{
	graph_t graph;
	problem_size_t *prob_size;

	int **nodes; // for each proc, a list of member node indexes
	int *partitions; // for each node, the proc it belongs to
	int proc;

	debug(LOW, "Loading graph in master...\n");
	load_input(filename, &graph);

	debug(HIGH, "Getting problem size...\n");
	prob_size = (problem_size_t *) malloc(sizeof(problem_size_t));
	prob_size->nodes = graph.node_count;
	prob_size->edges = graph.edge_count;
	debug(LOW, "Graph of %d nodes and %d edges loaded.\n", prob_size->nodes, prob_size->edges);

	debug(LOW, "Partitioning graph in master...\n");
	partition_graph_simple(&graph, &nodes, &partitions, part_sizes, num_procs);

	for(proc = 0; proc < num_procs; proc++)
		debug(HIGH, "Proc %d assigned %d nodes.\n", proc, *part_sizes[proc]);

	// send work (send 1D arrays at a time!  double pointers in loops!)
	debug(LOW, "Transforming and distributing graph in master...\n");
  transform_send_partition(&graph, nodes, partitions, *part_sizes, num_procs);

	return prob_size;
}

void partition_graph_simple(
		graph_t *graph,
		int ***nodes,		      // addr of array[part][idx] = node
		int **partitions,    // addr of array[node] = part
		int **node_counts,
		int num_procs)
{
	int proc, node, part_len, i;
	
	debug(HIGH, "Allocating partition to node arr\n");
	*nodes = (int **) malloc(num_procs * sizeof(int *));

	debug(HIGH, "Allocating node to partition arr\n");
	*partitions = (int *) malloc(graph->node_count * sizeof(int));

	debug(HIGH, "Allocating partition node sizes\n");
  *node_counts = (int *) malloc(num_procs * sizeof(int));

  debug(HIGH, "Partitioning graph...\n");
  node = 0;
  for (proc = 0; proc < num_procs; proc++) {
  	(*node_counts)[proc] = 0;
  	part_len = (proc + 1) * (graph->node_count / num_procs);
  	debug(VERBOSE, "Assigning proc %d from %d to %d\n", proc, node, part_len);
  	debug(VERBOSE, "Allocating for %d assignemnts\n", part_len - node);
  	(*nodes)[proc] = (int *) malloc((part_len - node) * sizeof(int));
    for (i = 0; node < part_len; node++) {
    	(*partitions)[node] = proc;
    	(*nodes)[proc][i] = node;
    	(*node_counts)[proc]++;
    	i++;
    }
    debug(VERBOSE, "%d nodes assigned.\n", (*node_counts)[proc]);
  }
  
}

void transform_send_partition(
		graph_t *graph,
		int **nodes, // array of nodes for each partition
		int *partitions,
		int *node_counts, // length of each above
		int num_procs)
{
	int **edge_pairs; // edges for each proc
										// edge_pairs[p] = {s_1, t_1, s_2, t_2, ...}
	int *edge_counts; // length of each array above (2 * E)

	// add an edge from one node to another iff both nodes are in the partition


	send_partition(graph, nodes, node_counts, edge_pairs, edge_counts, num_procs);

}

void send_partition(
		graph_t *graph,
		int **nodes,
		int *node_counts,
		int **edge_pairs,
		int *edge_counts,
		int num_procs)
{
	int ***incoming; // each proc: for each proc, targets indexes of
	                 // incoming inter-proc edges. EX: proc 1 from proc 2
	                 // incoming[1][2] = { 4, 4, 5, 7, 10 }
	int **incoming_counts; // incoming_counts[1][2] = 5

	int ***outgoing; // each proc: for each proc, source indexes of
		               // outgoing inter-proc edges. EX: proc 1 to proc 2
		               // incoming[1][2] = { 9, 9, 14 }
	int **outgoing_counts; // incoming_counts[1][2] = 3

	int proc, nbr_proc;

	for (proc = 0; proc < num_procs; proc++) {
		// isend edge_counts[proc]
		// isend edge_pairs[proc] len (above)

		for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {
			// isend incoming_counts[proc][nbr_proc]
			// isend incoming[proc][nbr_proc] len above
		}

		for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {
			// isend outgoing_counts[proc][nbr_proc]
			// isend outgoing[proc][nbr_proc] len above
		}
	}
}

/*-------------------------------------------------------------------*/
void receive_and_save(
		char *output_filename,
		int *part_sizes,
		int num_procs)
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
