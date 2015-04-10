
#include <stdlib.h>
#include "io.h"
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
		int num_procs
		)
{
	int proc, nbr_proc, proc_pair_incoming_count, proc_pair_outgoing_count, tmp;

	MPI_Request send_reqs[num_procs];

	// send to all but self (master == 0)
	for (proc = 1; proc < num_procs; proc++) {
		debug(HIGH, "Send edge count %d to proc %d.\n",
				edge_counts[proc],
				proc);
		isend_ints(&edge_counts[proc], 1, proc, &send_reqs[proc]);
	}
	master_wait(send_reqs, num_procs);

	// send to all but self (master == 0)
	for (proc = 1; proc < num_procs; proc++) {
		debug(HIGH, "Send edge pairs to proc %d.\n",
						proc);
		isend_ints(edge_pairs[proc], edge_counts[proc], proc, &send_reqs[proc]);
	}
	master_wait(send_reqs, num_procs);

	// send incoming and outgoing boundary data for each proc pair
	for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {

		for (proc = 1; proc < num_procs; proc++) {
			// send incoming count for proc pair
			debug(VERBOSE, "Send incoming boundary count %d to proc %d for boundary with proc %d.\n",
					incoming_counts[proc][nbr_proc],
					proc,
					nbr_proc);
			isend_ints(&incoming_counts[proc][nbr_proc], 1, proc, &send_reqs[proc]);
		}
		master_wait(send_reqs, num_procs);

		for (proc = 1; proc < num_procs; proc++) {
			// send incoming actual for proc pair
			proc_pair_incoming_count = incoming_counts[proc][nbr_proc];
			if (proc_pair_incoming_count != 0) {
				debug(VERBOSE, "Send actual incoming boundary to proc %d for boundary with proc %d.\n",
						proc,
						nbr_proc);
				isend_ints(incoming[proc][nbr_proc], proc_pair_incoming_count, proc, &send_reqs[proc]);
			} else {
				debug(VERBOSE, "Not sending empty incoming boundary  to proc %d for boundary with proc %d.\n",
						proc,
						nbr_proc);
				send_reqs[proc] = NULL;
			}
		}
		master_wait(send_reqs, num_procs);
	}

	for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {

		for (proc = 1; proc < num_procs; proc++) {
			// send outgoing count for proc pair
			debug(VERBOSE, "Send outgoing boundary count %d to proc %d for boundary with proc %d.\n",
					outgoing_counts[proc][nbr_proc],
					proc,
					nbr_proc);
			isend_ints(&outgoing_counts[proc][nbr_proc], 1, proc, &send_reqs[proc]);
		}
		master_wait(send_reqs, num_procs);

		for (proc = 1; proc < num_procs; proc++) {
			// send outgoing actual for proc pair
			proc_pair_outgoing_count = outgoing_counts[proc][nbr_proc];
			if (proc_pair_outgoing_count != 0) {
				debug(VERBOSE, "Send actual outgoing boundary to proc %d for boundary with proc %d.\n",
						proc,
						nbr_proc);
				isend_ints(outgoing[proc][nbr_proc], proc_pair_outgoing_count, proc, &send_reqs[proc]);
			} else {
				debug(VERBOSE, "Not sending empty outgoing boundary  to proc %d for boundary with proc %d.\n",
						proc,
						nbr_proc);
				send_reqs[proc] = NULL;
			}
		}
		master_wait(send_reqs, num_procs);
	}

	// send to self
	debug(HIGH, "Send edge count %d to master self.\n",
			edge_counts[0],
			0);
	isend_ints(&edge_counts[0], 1, 0, &send_reqs[0]);
	MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);

	debug(HIGH, "Send edge pairs to master self.\n",
					0);
	isend_ints(edge_pairs[0], edge_counts[0], 0, &send_reqs[0]);
	MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);

	// send incoming boundary data for each proc pair
	for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {

		// send incoming count for proc pair
		debug(VERBOSE, "Send incoming boundary count %d to master self for boundary with proc %d.\n",
				incoming_counts[0][nbr_proc],
				nbr_proc);
		isend_ints(&incoming_counts[0][nbr_proc], 1, 0, &send_reqs[0]);
		MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);

		// send incoming actual for proc pair
		proc_pair_incoming_count = incoming_counts[0][nbr_proc];
		if (proc_pair_incoming_count != 0) {
			debug(VERBOSE, "Send actual incoming boundary to master self for boundary with proc %d.\n",
					nbr_proc);
			isend_ints(incoming[0][nbr_proc], incoming_counts[0][nbr_proc], 0, &send_reqs[0]);
			MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);
		} else {
			debug(VERBOSE, "Not sending empty incoming boundary to master self for boundary with proc %d.\n",
					nbr_proc);
		}
	}

	// send outgoing boundary data for each proc pair
	for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {

		// send outgoing count for proc pair
		debug(VERBOSE, "Send outgoing boundary count %d to proc %d for boundary with proc %d.\n",
				outgoing_counts[0][nbr_proc],
				0,
				nbr_proc);
		isend_ints(&outgoing_counts[0][nbr_proc], 1, 0, &send_reqs[0]);
		MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);

		// send outgoing actual for proc pair
		proc_pair_outgoing_count = outgoing_counts[0][nbr_proc];
		if (proc_pair_outgoing_count != 0) {
			debug(VERBOSE, "Send actual outgoing boundary to master self for boundary with proc %d.\n",
					nbr_proc);
			isend_ints(outgoing[0][nbr_proc], outgoing_counts[0][nbr_proc], 0, &send_reqs[0]);
			MPI_Test(send_reqs, &tmp, MPI_STATUS_IGNORE);
		} else {
			debug(VERBOSE, "Not sending empty outgoing boundary to master self for boundary with proc %d.\n",
					nbr_proc);
		}
	}
}

void master_wait(
		MPI_Request *send_reqs,
		int num_procs
		)
{
	int proc;

	// only wait on ranks that are not the master (probably 0)
	for (proc = 1; proc < num_procs; proc++) {
		debug(HIGH, "Waiting on proc %d...\n", proc);
		if (send_reqs[proc] != NULL)
			MPI_Wait(&send_reqs[proc], MPI_STATUS_IGNORE);
		debug(HIGH, "Done waiting on proc %d.\n", proc);
	}
}

/*-------------------------------------------------------------------*/
int isend_ints(
		int *to_send,
		int length,
		int target_rank,
		MPI_Request *request
		)
{
	int ierr;

	ierr = MPI_Isend(to_send,
					length,
					MPI_INT,
					target_rank,
					0, // tag unused
					MPI_COMM_WORLD,
					request
					);
	 if (ierr != MPI_SUCCESS) {
		 int buflen = 1024;
		 char err_buffer[buflen];
		 MPI_Error_string(ierr,err_buffer,&buflen);
		 fprintf(stderr, "%s", err_buffer);
		 MPI_Finalize();  /* abort*/
		 return 1;
	 }
	 return 0;
}

/*-------------------------------------------------------------------*/
void receive_partition_graph(
		graph_t *graph,
		int my_rank
		)
{
	int edge_count, i;
	int *edge_pairs;

	*graph = *(graph_init());

	debug(HIGH, "%3d:   Receiving number of edges...\n", my_rank);
	recv_ints(&edge_count, 1, 0);

	debug(HIGH, "%3d:   Allocating for %d edges (pair elements).\n", my_rank, edge_count);
	edge_pairs = (int *) malloc(edge_count * sizeof(int));

	debug(HIGH, "%3d:   Receiving edge pairs\n", my_rank);
	recv_ints(edge_pairs, edge_count, 0);

	debug(VERBOSE, "%3d: Received edge pairs:\n", my_rank);
	debug_print_edge_pairs(VERBOSE, edge_pairs, edge_count);

	debug(HIGH, "%3d:   Building graph...\n", my_rank);
	for (i = 0; i < edge_count; i += 2) {
		debug(VERBOSE, "%3d:   Adding edge (%d, %d) from list idx %d and %d\n",
				my_rank,
				edge_pairs[i],
				edge_pairs[i + 1],
				i,
				i + 1);
		graph_add_edge(graph, edge_pairs[i], edge_pairs[i + 1]);
	}

	debug(HIGH, "%3d:   Received graph:\n", my_rank);
	debug_print_graph(VERBOSE, *graph);
}

/*-------------------------------------------------------------------*/
void receive_partition_boundaries(
	  int ***my_incoming,
	  int **my_incoming_counts,
	  int ***my_outgoing,
	  int **my_outgoing_counts,
	  int num_procs
		)
{
	int proc, incoming_nbr_count, outgoing_nbr_count;
//
//	int *incoming_counts = *my_incoming_counts;
//	int **incoming = *my_incoming;
//	int *outgoing_counts = *my_outgoing_counts;
//	int **outgoing = *my_outgoing;

	*my_incoming_counts = (int *) malloc(num_procs * sizeof(int));
	*my_outgoing_counts = (int *) malloc(num_procs * sizeof(int));
	*my_incoming = (int **) malloc(num_procs * sizeof(int *));
	*my_outgoing = (int **) malloc(num_procs * sizeof(int *));

	for (proc = 0; proc < num_procs; proc++) {

		// recv size num_procs into incoming_count[proc]
		debug(HIGH, "Receiving incoming count for nbr proc %d.\n", proc);
		recv_ints(&((*my_incoming_counts)[proc]), 1, 0);

		incoming_nbr_count = (*my_incoming_counts)[proc];
		if (incoming_nbr_count != 0) {
			debug(HIGH, "Allocating for count %d\n", incoming_nbr_count);
			(*my_incoming)[proc] = (int *) malloc(incoming_nbr_count * sizeof(int));

			// recv size incoming_count[proc] into incoming[proc]
			debug(HIGH, "Receiving incoming for nbr proc %d (length %d).\n", proc, incoming_nbr_count);
			recv_ints((*my_incoming)[proc], incoming_nbr_count, 0);
		} else {
			(*my_incoming)[proc] = NULL;
			debug(HIGH, "Zero received!\n");
		}

		// recv size num_procs into outgoing_count[proc]
		debug(HIGH, "Receiving outgoing count for nbr proc %d.\n", proc);
		recv_ints(&((*my_outgoing_counts)[proc]), 1, 0);

		outgoing_nbr_count = (*my_outgoing_counts)[proc];
		if (outgoing_nbr_count != 0) {
			debug(HIGH, "Allocating for count %d\n", outgoing_nbr_count);
			(*my_outgoing)[proc] = (int *) malloc(outgoing_nbr_count * sizeof(int));

			// recv size outgoing_count[proc] into outgoing[proc]
			debug(HIGH, "Receiving outgoing for nbr proc %d (length %d).\n", proc, outgoing_nbr_count);
			recv_ints((*my_outgoing)[proc], outgoing_nbr_count, 0);
		} else {
			(*my_outgoing)[proc] = NULL;
			debug(HIGH, "Zero received!\n");
		}
	}

	debug(VERBOSE, "Received proc boundaries:\n");
	debug_print_proc_boundaries(VERBOSE,
			*my_incoming,
			*my_incoming_counts,
			*my_outgoing,
			*my_outgoing_counts,
			num_procs
			);
}

/*-------------------------------------------------------------------*/
int recv_ints(
		int *recv_buf,
		int length,
		int src_rank
		)
{
	int recvd_length;
	MPI_Status status;

	MPI_Recv(recv_buf,
					length,
					MPI_INT,
					src_rank,
					0, // tag unused
					MPI_COMM_WORLD,
					&status
					);
	debug(VERBOSE, "First element of received: %d\n", *recv_buf);

	MPI_Get_count(&status, MPI_INT, &recvd_length);
	if (recvd_length != length) {
		printf("Expected to receive %d elements, got %d!", length, recvd_length);
		return 1;
	}
	return 0;
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
