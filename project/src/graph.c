#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "graph.h"

void adj_init(adj_t *adj, double rank, int *nbrs, int nbr_count)
{
  adj = malloc(sizeof(adj_t));
  err_check(adj, "Allocating adj_t");

  adj->rank = rank;
  adj->nbrs = malloc(sizeof(int) * nbr_count);
  err_check(adj->nbrs, "Allocating adj_t nbrs");

  err_check(memcpy(adj->nbrs, nbrs, sizeof(int) * nbr_count), "Copying adj_t nbrs");
}

void adj_destroy(adj_t *adj)
{
  free(adj->nbrs);
  free(adj);
}
