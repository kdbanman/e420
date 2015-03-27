
#ifndef _GRAPH_H_
#define _GRAPH_H_

/*
 * Linked list and list member structures.
 */
typedef struct edge_t {
  int src;
  int dst;
} edge_t;

typedef struct edge_list_t {
  edge_t *edge;
  struct edge_list_t *next;
} edge_list_t;

void edge_init(edge_list_t *edge_list, int src, int dst);
void edge_list_connect(edge_list_t *previous, edge_list_t *next);

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
