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
/*-------------------INTERNAL FUNCTIONS-------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int rank_init(graph_t *graph)
{
	int i;
	double rank;

	rank = 1.0 / (double) graph->node_count;
	debug(HIGH, "Initializing all nodes to rank %5.5f\n", rank);

	for (i = 0; i < graph->node_count; i++) {
		debug(VERBOSE, "Initializing node %d\n", i);
		graph->nodes[i].rank = rank;
	}

	return 0;
}

/*--------------------------------------------------------------------*/
/*-------------------PUBLIC FUNCTIONS---------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int rank(graph_t *graph, double threshold)
{
	double delta;

	rank_init(graph);

	delta = threshold;
	while (delta >= threshold) {
		debug(HIGH, "Running pagerank iteration...\n");
		delta = rank_iter(graph);
		debug(HIGH, "Total rank changed by %5.5f\n", delta);
	}

	return 0;
}

/*--------------------------------------------------------------------*/
double rank_iter(graph_t *graph)
{
	double new_ranks[graph->node_count];
	double delta;
	int i;

	delta = 0.0;
	for (i = 0; i < graph->node_count; i++) {
		debug(VERBOSE, "Calculating rank of node %d. Currently %5.3... ",
				i,
				graph->nodes[i].rank);

		new_ranks[i] = rank_node(graph, i);

		debug(VERBOSE, "now %5.3f\n", graph->nodes[i].rank);

		// add absolute value to delta
		delta += new_ranks[i] > 0 ? new_ranks[i] : -1.0 * new_ranks[i];
	}

	debug(HIGH, "Copying ranks to graph.\n");
	for (i = 0; i < graph->node_count; i++) {
		graph->nodes[i].rank = new_ranks[i];
	}

	return delta;
}

/*--------------------------------------------------------------------*/
double rank_node(graph_t *graph, int node_id)
{
	double rank;
	int i;

	for (i = 0; i < graph->nodes[i].target_count; i++) {
		//TODO
		rank = 0.0;
	}

	return rank;
}
