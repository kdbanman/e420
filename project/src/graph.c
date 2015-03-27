#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "io.h"
#include "graph.h"

edge_list_t * edge_init(int src, int dst)
{
  edge_list_t *edge_list = (edge_list_t *) malloc(sizeof(edge_list_t));
  err_check(edge_list, "Allocating edge_list_t");
  edge_list->edge = (edge_t *) malloc(sizeof(edge_t));
  err_check(edge_list->edge, "Allocating edge_t");

  edge_list->edge->src = src;
  edge_list->edge->dst = dst;
  edge_list->next = NULL;
  
  return edge_list;
}

void edge_list_connect(edge_list_t *previous, edge_list_t *next)
{
  debug(VERBOSE, "linking edge %x to %x\n", previous, next);
  previous->next = next;
}

node_t * node_init(int idx)
{
  node_t *node = (node_t *) malloc(sizeof(node_t));
  err_check(node, "Allocating node_t");

  node->idx = idx;
  node->nbr_count = 0;
  node->nbrs = NULL;
  node->rank = 0.0;
  
  return node;
}

void node_destroy(node_t *node)
{
  free(node->nbrs);
  free(node);
}
