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

#include "graph.h"

#ifndef _RANK_UTIL_H_
#define _RANK_UTIL_H_

/*
 * Initialize all pageranks with 1/N.  Mutates graph.
 */
int rank_init(graph_t *graph, int total_size);

/*
 * Mutate the graph's node_t.rank at each node with the final rank.
 */
int rank(graph_t *graph, double threshold, int total_size);

/*
 * Mutate the graph's node_t.rank at each node for one iteration of rank.
 * Return the total change in rank.
 */
double rank_iter(graph_t *graph, int total_size);

/*
 * Return the rank of the node in the graph.  Do not mutate the node or graph.
 */
double rank_node(graph_t *graph, int node_id, int total_size);

#endif
