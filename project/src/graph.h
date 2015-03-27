
#ifndef _GRAPH_H_
#define _GRAPH_H_

/*
 * Linked list and list member structures.
 */
typedef struct edge_list_t {
  edge_t edge;
  edge_t *next;
} edge_list_t;

typedef struct edge_t {
  int src;
  int dst;
} edge_t;

void edge_init(edge_t *edge, int src, int dst, edge_t *previous);

/*
 * Intended to be stored in an array as an adjacency list element.
 * Stores page rank and an array of neighbors.
 */
typedef struct adj_t {
  double rank;
  int *nbrs;
} adj_t;

void adj_init(adj_t *adj, double rank, int *nbrs, int nbr_count);
void adj_destroy(adj_t *adj);

#endif
