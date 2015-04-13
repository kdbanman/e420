/*
 * Kirby Banman
 * ECE 420 Project
 * 10 April 2015
 *
 * File:
 *    rank_util.c
 *
 * Purpose:
 *    Pagerank utility methods
 *
 * Compile:
 *    gcc -Wall -Wextra -o bin/rank_util.o -c src/rank_util.c
 *
 */

#include "io.h"
#include "graph.h"
#include "rank_util.h"

/*--------------------------------------------------------------------*/
int rank_init(graph_t *graph, int total_size)
{
	int i;
	double rank;

	rank = 1.0 / (double) total_size;
	debug(HIGH, "Initializing all nodes to rank %5.5f\n", rank);

	for (i = 0; i < graph->node_count; i++) {
		debug(VERBOSE, "Initializing node %d rank to %5.9f\n", i, rank);
		graph->nodes[i]->rank = rank;
	}

	return 0;
}

/*--------------------------------------------------------------------*/
int rank(graph_t *graph, double threshold, int total_size)
{
	double delta;

	rank_init(graph, total_size);

	delta = threshold;
	while (delta >= threshold) {
		debug(HIGH, "Running pagerank iteration...\n");
		delta = rank_iter(graph, total_size);
		debug(HIGH, "Total rank changed by %5.5f\n", delta);
	}

	return 0;
}

/*--------------------------------------------------------------------*/
double rank_iter(graph_t *graph, int total_size)
{
	double new_ranks[graph->node_count];
	double delta, old_rank, curr_delta, total_rank;
	int i;

	delta = 0.0;
	for (i = 0; i < graph->node_count; i++) {

		if (graph->nodes[i]->empty) {
			debug(EXTREME, "Skipping empty node %d\n", i);
			continue;
		}

		old_rank = graph->nodes[i]->rank;

		debug(VERBOSE, "Calculating rank of node %d. Currently %5.9f... ",
				i,
				old_rank);

		new_ranks[i] = rank_node(graph, i, total_size);

		debug(VERBOSE, "now %5.9f\n", old_rank);

		curr_delta = old_rank - new_ranks[i];
		// add absolute value to delta
		delta += curr_delta > 0 ? curr_delta : -1.0 * curr_delta;
	}

	total_rank = 0.0;
	debug(HIGH, "Copying ranks to graph.\n");
	for (i = 0; i < graph->node_count; i++) {

		if (graph->nodes[i]->empty) {
			debug(EXTREME, "Skipping empty node %d\n", i);
			continue;
		}

		graph->nodes[i]->rank = new_ranks[i];
		total_rank += new_ranks[i];
	}
	debug(HIGH, "Total graph rank: %f\n", total_rank);

	return delta;
}

/*--------------------------------------------------------------------*/
double rank_node(graph_t *graph, int node_id, int total_size)
{
	double rank;
	int i;
	node_t *node;

	rank = 0.0;
	node = graph->nodes[node_id];
	for (i = 0; i < node->incoming_count; i++) {
		node_t *src_nbr = node->incoming[i];

		double src_outgoing_count =
				src_nbr->outgoing_count_global != 0 ?
				(double) src_nbr->outgoing_count_global :
				(double) (total_size - 1);

		rank += src_nbr->rank / src_outgoing_count;
	}

	rank = (0.15 + (double) total_size * 0.85 * rank) / (double) total_size;

	if (rank < 0.0) printf("INSANITY\n");
	return rank;
}
