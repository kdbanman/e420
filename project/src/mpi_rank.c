/*
 * Compile:  mpicc -g -Wall -o mpi_rank mpi_rank.c
 * Run:      mpirun -np <number of processes> --hostfile <hostfile> ./mpi_rank
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "io.h"
#include "timer.h"
#include "rank_util.h"
#include "mpi_io.h"
#include "mpi.h"

/**********************
 *              GLOBALS
 */

double threshold = 1E-5;

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
		int num_procs
		);

void partition_graph_simple(
		graph_t *graph,
		int ***nodes,		      // addr of array[part][idx] = node
		int **partitions,    // addr of array[node] = part
		int **node_counts,
		int num_procs
		);

void transform_send_partition(
		graph_t *graph,
		int **nodes, // array of array[proc][idx] = node
		int *partitions,  // array of array[node_idx] = proc
		int *node_counts, // length of each above
		int num_procs
		);

void add_proc_edge(
		int node_idx,
		int **proc_buffer,
		int *count
		);

void synced_rank(
//		graph_t *graph,
//		double threshold,
//		int total_size,
//		int **incoming, //each proc: array of indices for ranks to recv
//    int *incoming_counts,
//		int **outgoing, //each proc: array of indices for ranks to send
//    int *outgoing_counts,
//		int num_procs
		);

void usage(char* prog_name);

/**********************
 *                 MAIN
 */

int main( int argc, char *argv[] )
{
	// problem-global data
	int num_procs;

	// local data for each proc
	int my_rank;

	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs );
	MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
	MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);


  int name_len = 512;
  char hostname[name_len];

	int *part_sizes;
	double time_start, time_end;
	char *input_filename, *output_filename;

	graph_t my_graph;
	problem_size_t * prob_size;
  int **my_incoming, **my_outgoing;
  int *my_incoming_counts, *my_outgoing_counts;

	io_dbg_lev = atoi(getenv("DEBUG"));

	/* Get number of threads and size from command line */
	if (argc != 3) usage(argv[0]);
	input_filename = argv[1];
	output_filename = argv[2];

	// if master, distribute work while rest wait
	if (my_rank == 0) {
		GET_TIME(time_start);

		gethostname(hostname, name_len - 1);

		debug(LOW, "Master host: %s\n", hostname);
		prob_size = init_cluster(input_filename, &part_sizes, num_procs);


		// master must receive work from self
		debug(LOW, "%3d:   Master receiving graph...\n", my_rank);
		receive_partition_graph(&my_graph, my_rank);

		debug(LOW, "%3d:   Master receiving partition...\n", my_rank);
		receive_partition_boundaries(
				&my_incoming,
				&my_incoming_counts,
				&my_outgoing,
				&my_outgoing_counts,
				num_procs);

		debug(HIGH, "%3d:   Master sending node count %d\n", my_rank, prob_size->nodes);
		master_send_int(&(prob_size->nodes), num_procs);
		debug(HIGH, "%3d:   Master sending edge count %d\n", my_rank, prob_size->edges);
		master_send_int(&(prob_size->edges), num_procs);

	} else {
		debug(LOW, "%3d:   Proc receiving graph...\n", my_rank);
		receive_partition_graph(&my_graph, my_rank);

		debug(LOW, "%3d:   Proc receiving partition...\n", my_rank);
		receive_partition_boundaries(
				&my_incoming,
				&my_incoming_counts,
				&my_outgoing,
				&my_outgoing_counts,
				num_procs);

		prob_size = (problem_size_t *) malloc(sizeof(problem_size_t));

		// recv nodes then recv edges prob_size to all...
		debug(HIGH, "%3d:   Receiving node count...\n", my_rank);
		recv_ints(&(prob_size->nodes), 1, 0);
		debug(HIGH, "%3d:   Received node count %d...\n", my_rank, prob_size->nodes);

		debug(HIGH, "%3d:   Receiving edge count...\n", my_rank);
		recv_ints(&(prob_size->edges), 1, 0);
		debug(HIGH, "%3d:   Received edge count %d...\n", my_rank, prob_size->edges);
	}

	//debug(LOW, "%3d:   Proc entering rank procedure with %d nodes and %d edges...\n", my_rank, prob_size->nodes, prob_size->edges);
	synced_rank(
			&my_graph,
			threshold,
			prob_size->nodes,
			my_incoming,
			my_incoming_counts,
			my_outgoing,
			my_outgoing_counts,
			num_procs
			);

	MPI_Barrier(MPI_COMM_WORLD);

	// master receive result array from all (master sends self)
	if (my_rank == 0) {
		receive_results_and_save(output_filename, part_sizes, num_procs);

		/* Record end time and report delta. */
		GET_TIME(time_end);
		printf("Elapsed time for size %d nodes, %d edges: %5.3fms\n",
				prob_size->nodes,
				prob_size->edges,
				time_end - time_start);
	}

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
		int num_procs
		)
{
	graph_t graph;
	problem_size_t *prob_size;

	int **nodes; // for each proc, a list of member node indexes
	int *partitions; // for each node, the proc it belongs to
	int proc;

	debug(LOW, "Loading graph in master...\n");
	load_input(filename, &graph);

	debug_print_graph(VERBOSE, graph);

	debug(HIGH, "Getting problem size...\n");
	prob_size = (problem_size_t *) malloc(sizeof(problem_size_t));
	prob_size->nodes = graph.node_count;
	prob_size->edges = graph.edge_count;
	debug(LOW, "Graph of %d nodes and %d edges loaded.\n", prob_size->nodes, prob_size->edges);

	debug(LOW, "Partitioning graph in master...\n");
	partition_graph_simple(&graph, &nodes, &partitions, part_sizes, num_procs);

	for(proc = 0; proc < num_procs; proc++)
		debug(HIGH, "Proc %d assigned %d nodes.\n", proc, (*part_sizes)[proc]);

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
		int num_procs
		)
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
		int num_procs
		)
{
	int **edge_pairs; // edges for each proc
										// edge_pairs[p] = {s_1, t_1, s_2, t_2, ...}
	int *edge_counts; // length of each array above (2 * E)

	int ***incoming; // each proc: for each proc, targets indexes of
									 // incoming inter-proc edges. EX: proc 1 from proc 2
								   // incoming[1][2] = { 4, 4, 5, 7, 10 }
	int **incoming_counts; // incoming_counts[1][2] = 5

	int ***outgoing; // each proc: for each proc, source indexes of
									 // outgoing inter-proc edges. EX: proc 1 to proc 2
									 // incoming[1][2] = { 9, 9, 14 }
	int **outgoing_counts; // incoming_counts[1][2] = 3

	node_t *node;
	int proc, curr_size, proc_i, proc_node, src_idx, tgt_i, tgt_idx, tgt_proc;

	debug(HIGH, "Allocating for edge pairs arr\n");
	edge_pairs = (int **) malloc(num_procs * sizeof(int *));
	debug(HIGH, "Allocating for edge counts arr\n");
	edge_counts = (int *) malloc(num_procs * sizeof(int));
	debug(HIGH, "Init edge counts to zero\n");
	for (proc = 0; proc < num_procs; proc++) {
		edge_counts[proc] = 0;
	}

	debug(HIGH, "Allocating for incoming and outgoing arrays\n");
	incoming = (int ***) malloc(num_procs * sizeof(int **));
	outgoing = (int ***) malloc(num_procs * sizeof(int **));

	incoming_counts = (int **) malloc(num_procs * sizeof(int *));
	outgoing_counts = (int **) malloc(num_procs * sizeof(int *));

	for (proc = 0; proc < num_procs; proc++) {
		incoming[proc] = (int **) malloc(num_procs * sizeof(int *));
		outgoing[proc] = (int **) malloc(num_procs * sizeof(int *));

		incoming_counts[proc] = (int *) malloc(num_procs * sizeof(int));
		outgoing_counts[proc] = (int *) malloc(num_procs * sizeof(int));
		for (proc_i = 0; proc_i < num_procs; proc_i++) {
			incoming[proc][proc_i] = (int *) malloc(0);
			outgoing[proc][proc_i] = (int *) malloc(0);
			incoming_counts[proc][proc_i] = 0;
			outgoing_counts[proc][proc_i] = 0;
		}
	}

	// add an edge from one node to another iff both nodes are in the partition
	debug(LOW, "Transforming partition.");
	for (proc = 0; proc < num_procs; proc++) {

		debug(HIGH, "Allocating initially for proc %d pairs\n", proc);
		curr_size = 100;
		edge_pairs[proc] = (int *) malloc(curr_size * sizeof(int));

		debug(HIGH, "Populating proc %d edge pairs and proc boundaries (at sources and targets)", proc);
		for (proc_node = 0; proc_node < node_counts[proc]; proc_node++) { // iterate through partition's nodes

			debug(VERBOSE, "Get node %d for proc %d\n", proc_node, proc);
			src_idx = nodes[proc][proc_node];
			node = graph->nodes[src_idx];
			// need only look through outgoing - edges are doubly-linked
			for (tgt_i = 0; tgt_i < node->outgoing_count; tgt_i++) {
				
				debug(VERBOSE, "Get target  %d for proc %d\n", proc_node, proc);
				tgt_idx = node->outgoing[tgt_i]->idx;

				debug(VERBOSE, "Test edge between node id %d and %d\n", src_idx, tgt_idx);
				tgt_proc = partitions[tgt_idx];
				if (tgt_proc == proc) {
					// both source and target are in proc - add to proc edge_pairs
					debug(VERBOSE, "Target %d is in partition\n", tgt_idx);
					edge_pairs[proc][edge_counts[proc]] = src_idx;
					edge_pairs[proc][edge_counts[proc] + 1] = tgt_idx;

					debug(VERBOSE, "Proc %d's pairs list append: src_idx %d to list pos %d, tgt_idx %d to list pos %d\n",
							proc,
							src_idx,
							edge_counts[proc],
							tgt_idx,
							edge_counts[proc] + 1);

					edge_counts[proc] += 2;
					// reallocate if necessary
					if (edge_counts[proc] + 2 >= curr_size) {
						curr_size += 1000;
						debug(VERBOSE, "Resizing proc %d pairs array to size %d\n", proc, curr_size);
						edge_pairs[proc] = (int *) realloc(edge_pairs[proc], curr_size * sizeof(int));
					}
				} else {
					// source is in partition, target elsewhere - add to target partition's incoming edges
					debug(VERBOSE, "Proc edge found between source proc %d and target proc %d\n", proc, tgt_proc);

					debug(EXTREME, "Old boundaries for source: (%d)\n", proc);
					debug_print_proc_boundaries(EXTREME,
							incoming[proc],
							incoming_counts[proc],
							outgoing[proc],
							outgoing_counts[proc],
							num_procs);

					debug(EXTREME, "Old boundaries for target: (%d)\n", tgt_proc);
					debug_print_proc_boundaries(EXTREME,
							incoming[tgt_proc],
							incoming_counts[tgt_proc],
							outgoing[tgt_proc],
							outgoing_counts[tgt_proc],
							num_procs);

					// add edge target to target proc's incoming and edge source to (source) proc's outgoing
					debug(VERBOSE, "Adding incoming edge to target proc %d's boundary for proc %d (target node %d)\n", tgt_proc, proc, tgt_idx);
					add_proc_edge(tgt_idx,
							&incoming[tgt_proc][proc],
							&incoming_counts[tgt_proc][proc]);

					debug(VERBOSE, "Adding outgoing edge to source proc %d's boundary for proc %d (source node %d)\n", proc, tgt_proc, src_idx);
					add_proc_edge(src_idx,
							&outgoing[proc][tgt_proc],
							&outgoing_counts[proc][tgt_proc]);


					debug(EXTREME, "New boundaries for source: (%d)\n", proc);
					debug_print_proc_boundaries(EXTREME, incoming[proc], incoming_counts[proc], outgoing[proc], outgoing_counts[proc], num_procs);
					debug(EXTREME, "New boundaries for target: (%d)\n", tgt_proc);
					debug_print_proc_boundaries(EXTREME, incoming[tgt_proc], incoming_counts[tgt_proc], outgoing[tgt_proc], outgoing_counts[tgt_proc], num_procs);
				}
			} // end proc node outgoing nbrs loop
		} // end proc node loop

		debug(HIGH, "Finally resizing proc %d pairs array to size %d\n", proc, curr_size);
		edge_pairs[proc] = (int *) realloc(edge_pairs[proc], edge_counts[proc] * sizeof(int));

		debug(LOW, ".");
	}  // end proc loop
	debug(LOW, "\n");

	debug_print_all_edge_pairs(VERBOSE, edge_pairs, edge_counts, num_procs);

	debug(LOW, "Distributing partition in main.\n");
	send_partition(edge_pairs,
			edge_counts,
			incoming,
			incoming_counts,
			outgoing,
			outgoing_counts,
			num_procs
			);

}

void add_proc_edge(int node_idx, int **proc_buffer, int *count)
{
	int new_size;

	debug(VERBOSE, "Resizing proc buffer <0x%x> from %d\n", (*proc_buffer), *count);
	*count += 1;
	new_size = *count;

	debug(VERBOSE, "Incremented size var to %d for node idx %d\n", new_size, node_idx);
	*proc_buffer =  realloc((*proc_buffer), new_size * sizeof(int));
	debug(VERBOSE, "Resized proc buffer to %d\n", new_size);

	debug(VERBOSE, "Appending node %d to buffer.\n", node_idx);
	(*proc_buffer)[new_size - 1] = node_idx;
}

/*--------------------------------------------------------------------*/
void synced_rank(
		graph_t *graph,
		double threshold,
		int total_size,
		int **incoming, //each proc: array of indices for ranks to recv
    int *incoming_counts,
		int **outgoing, //each proc: array of indices for ranks to send
    int *outgoing_counts,
		int num_procs
		)
{
	double delta;

	rank_init(graph, total_size);

	delta = threshold;
	while (delta >= threshold) {
		delta = rank_iter(graph, total_size);
    delta += send_recv_ranks(graph, incoming, incoming_counts, outgoing, outgoing_counts, num_procs);

    debug(HIGH, "Sending local delta %f\n", delta);
    delta = get_global_delta(delta, num_procs);
    debug(HIGH, "Received global delta %f\n", delta);
	}

}

/*-------------------------------------------------------------------*/
void usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <input filename> <output filename>\n", prog_name);
   exit(1);
}  // usage
