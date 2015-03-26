
/*
 * Intended to be stored in an array as an adjacency list element.
 * Stores page rank and an array of neighbors.
 */
typedef struct adj_t {
  double rank;
  int *nbrs;
} adj_t;

int adj_init(adj_t *adj, double rank, int *nbrs, int nbr_count);

