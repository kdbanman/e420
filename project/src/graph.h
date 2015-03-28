
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

edge_list_t * edge_init(int src, int dst);
void edge_list_connect(edge_list_t *previous, edge_list_t *next);

/*
 * Intended to be stored in an array as an adjacency list element.
 * Stores page rank and an array of neighbors.
 */
typedef struct node_t {
  int idx;
  int nbr_count;
  struct node_t *nbrs;
  double rank;
} node_t;

/* Allocates a node_t struct and sets members to idx, 0, and NULL */
node_t * node_init(int idx);
/* Adds dst to src's adjacency list using realloc */
void add_nbr(node_t *src, node_t *dst);
void node_destroy(node_t *node);

typedef struct graph_t {
  int node_count;
  int edge_count;
  node_t *nodes;
} graph_t;

/* Allocates a graph struct to empty contents.
 * TODO Malloc zero size so realloc works later?
 */
graph_t * graph_init();
/* Reallocs node array if necessary, initializing additional elements to NULL.
 * Init src/dst node if they don't exist. Increments node count accordingly.
 * Add edge between src and dst using add_nbr(). (Does not check for existence.)
 * Increment edge count. TODO
 */
void graph_add_edge(int src_id, int dst_id);

#endif
