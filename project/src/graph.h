
#ifndef _GRAPH_H_
#define _GRAPH_H_

/*
 * Intended to be stored in an array as an adjacency list element.
 * Stores page rank and an array of neighbors.
 */
typedef struct adj_t {
  double rank;
  int *nbrs;
} adj_t;

int adj_init(adj_t *adj, double rank, int *nbrs, int nbr_count);
int adj_destroy(adj_t *adj);

#endif
