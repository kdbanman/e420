#include "graph.h"
#include "mpi.h"

#ifndef _MPI_IOH_
#define _MPI_IOH_

void send_partition(
		int **edge_pairs,   // array[proc] = [s_1, t_1, s_2, t_2, ...]
		int *edge_counts,   // lengths of above

		int ***incoming, // each proc: for each proc, targets indexes of
										 // incoming inter-proc edges. EX: proc 1 from proc 2
									   // incoming[1][2] = { 4, 4, 5, 7, 10 }
		int **incoming_counts, // incoming_counts[1][2] = 5

		int ***outgoing, // each proc: for each proc, source indexes of
										 // outgoing inter-proc edges. EX: proc 1 to proc 2
										 // outgoing[1][2] = { 9, 9, 14 }
		int **outgoing_counts, // outgoing_counts[1][2] = 3
		int num_procs);


int isend(
		int *to_send,
		int length,
		int target_rank,
		MPI_Request *request
		);

void receive_partition_graph(
//		graph_t *graph
		);

void receive_partition_boundaries(
//TODO params (by ref)
		);

int recv(
		int *recv_buf,
		int length,
		int src_rank
		);

void receive_results_and_save(
//		char *output_filename,
//		int *part_sizes,
//		int num_procs
		);

#endif
