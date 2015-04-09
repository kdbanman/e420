#include "mpi_io.h"
#include "mpi.h"

/*-------------------------------------------------------------------*/
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
		int num_procs)
{
	int proc, nbr_proc;

	for (proc = 0; proc < num_procs; proc++) {
		// isend edge_counts[proc] isend(&edge_counts[proc],num_procs,proc,SIZE_TAG);
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
int isend(
		int *to_send,
		int length,
		int target_rank)
{
	MPI_Isend(to_send,
					length * sizeof(int),
					MPI_INT,
					target_rank,
					SIZE_TAG,
					MPI_COMM_WORLD,
					NULL
					);
}

/*-------------------------------------------------------------------*/
void receive_partition_graph(
		graph_t *graph
		)
{
	// note: all sizes * sizeof(int)

	// recv size num_procs into edge_counts
	// recv size edge_counts[proc] into edge_pairs[proc]

	// build graph from edge list
}

/*-------------------------------------------------------------------*/
void receive_partition_boundaries(
//TODO params (by ref)
		)
{
	// note: all sizes * sizeof(int)

	// for each proc

		// recv size num_procs into incoming_count[proc]
		// recv size incoming_count[proc] into incoming[proc]

		// recv size num_procs into outgoing_count[proc]
		// recv size outgoing_count[proc] into outgoing[proc]
}

/*-------------------------------------------------------------------*/
void receive_results_and_save(
//		char *output_filename,
//		int *part_sizes,
//		int num_procs
		)
{
	// allocate receipt buffers for each proc

	// receive from all procs (including master self)

	// save each buffer to the same file
}
