#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "graph.h"

void edge_init(edge_list_t *edge, int src, int dst, edge_list_t *previous)
{
  edge = malloc(sizeof(edge_list_t));
  err_check(edge, "Allocating edge_list_t");
  edge->edge = malloc(sizeof(edge_t));
  err_check(edge->, "Allocating edge_t");

  edge->edge->src = src;
  edge->edge->dst = dst;
  edge->next = NULL;
  
  if (previous != NULL)
    previous.next = edge;
}

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
