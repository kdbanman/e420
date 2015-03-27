#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "graph.h"

void edge_init(edge_list_t *edge_list, int src, int dst)
{
  edge_list = (edge_list_t *) malloc(sizeof(edge_list_t));
  err_check(edge_list, "Allocating edge_list_t");
  edge_list->edge = (edge_t *) malloc(sizeof(edge_t));
  err_check(edge_list->edge, "Allocating edge_t");

  edge_list->edge->src = src;
  edge_list->edge->dst = dst;
  edge_list->next = NULL;
}

void edge_list_connect(edge_list_t *previous, edge_list_t *next)
{
  previous->next = next;
}

void adj_init(adj_t *adj, double rank, int *nbrs, int nbr_count)
{
  adj = (adj_t *) malloc(sizeof(adj_t));
  err_check(adj, "Allocating adj_t");

  adj->rank = rank;
  adj->nbrs = (int *) malloc(sizeof(int) * nbr_count);
  err_check(adj->nbrs, "Allocating adj_t nbrs");

  err_check(memcpy(adj->nbrs, nbrs, sizeof(int) * nbr_count), "Copying adj_t nbrs");
}

void adj_destroy(adj_t *adj)
{
  free(adj->nbrs);
  free(adj);
}
