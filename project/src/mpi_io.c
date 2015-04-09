
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
	int proc, nbr_proc, req_no, num_sends;

	MPI_Request send_reqs[num_procs];

	// refactor:  only one send at a time per proc

	// for proc from 1
		// send edge counts

	// wait all recv

	// for proc from 1
		// send edge pairs

	// wait all recv

	// then for nbr_proc;

		// for proc from 1;
			// send incoming count for proc pair

		// wait all recv

		// for proc from 1;
			// send incoming actual for proc pair

	  // wait all recv

	  //above 6 for outgoing
	req_no = 0;
	for (proc = 0; proc < num_procs; proc++) {
		debug(HIGH, "Send %d:  edge count %d to proc %d.\n",
				req_no,
				edge_counts[proc],
				proc);
		isend_ints(&edge_counts[proc], 1, proc, &send_reqs[req_no]);
		req_no++;

		debug(HIGH, "Send %d:  edge pairs to proc %d.\n",
				req_no,
				proc);
		isend_ints(edge_pairs[proc], edge_counts[proc], proc, &send_reqs[req_no]);
		req_no++;

		for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {

			debug(VERBOSE, "Send %d:  incoming boundary count %d to proc %d for boundary with proc %d.\n",
					req_no,
					incoming_counts[proc][nbr_proc],
					proc,
					nbr_proc);
			isend_ints(&incoming_counts[proc][nbr_proc], 1, proc, &send_reqs[req_no]);
			req_no++;

			debug(VERBOSE, "Send %d:  actual incoming boundary to proc %d for boundary with proc %d.\n",
					req_no,
					proc,
					nbr_proc);
			isend_ints(incoming[proc][nbr_proc], incoming_counts[proc][nbr_proc], proc, &send_reqs[req_no]);
			req_no++;
		}

		for (nbr_proc = 0; nbr_proc < num_procs; nbr_proc++) {
			// isend outgoing_counts[proc][nbr_proc]
			debug(VERBOSE, "Send %d:  outgoing boundary count %d to proc %d for boundary with proc %d.\n",
					req_no,
					outgoing_counts[proc][nbr_proc],
					proc,
					nbr_proc);
			isend_ints(&outgoing_counts[proc][nbr_proc], 1, proc, &send_reqs[req_no]);
			req_no++;

			// isend outgoing[proc][nbr_proc] len above
			debug(VERBOSE, "Send %d:  actual outgoing boundary to proc %d for boundary with proc %d.\n",
					req_no,
					proc,
					nbr_proc);
			isend_ints(outgoing[proc][nbr_proc], outgoing_counts[proc][nbr_proc], proc, &send_reqs[req_no]);
			req_no++;
		}
	}

	// only wait on ranks that are not the master (probably 0)
	for (proc = 1; proc < num_procs; proc++) {
		MPI_Wait(&send_reqs[proc], MPI_STATUS_IGNORE);
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
					length * sizeof(int),
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
//		graph_t *graph
		)
{
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
int recv_ints(
		int *recv_buf,
		int length,
		int src_rank
		)
{
	int recvd_length;
	MPI_Status status;

	MPI_Recv(recv_buf,
					length * sizeof(int),
					MPI_INT,
					src_rank,
					0, // tag unused
					MPI_COMM_WORLD,
					&status
					);

	MPI_Get_count(&status, MPI_INT, &recvd_length);
	if (recvd_length != length) {
		fprintf(stderr, "Expected to receive %d elements, got %d!", length, recvd_length);
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
