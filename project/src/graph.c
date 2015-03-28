#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "io.h"
#include "graph.h"

/*--------------------------------------------------------------------*/
/*-------------------INTERNAL FUNCTIONS-------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
node_t * resize_nodes(node_t *nodes, int size)
{
  return (node_t *) realloc(nodes, sizeof(node_t) * size);
}

/*--------------------------------------------------------------------*/
/*-------------------PUBLIC FUNCTIONS---------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*/
void edge_list_connect(edge_list_t *previous, edge_list_t *next)
{
  debug(VERBOSE, "linking edge %x to %x\n", previous, next);
  previous->next = next;
}

/*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*/
void add_nbr(node_t *src, node_t *dst)
{
  src->nbr_count++;
  debug(VERBOSE,
        "reallocating node %d nbrs length %d\n",
        src->idx,
        src->nbr_count);

  src->nbrs = resize_nodes(src->nbrs, src->nbr_count);
  err_check(src->nbrs, "Reallocating src->nbrs");

  debug(VERBOSE, "adding node %d to %d nbrs\n", dst->idx, src->idx);
  src->nbrs[src->nbr_count - 1] = *dst;
}

/*--------------------------------------------------------------------*/
void node_destroy(node_t *node)
{
  free(node->nbrs);
  free(node);
}

/*--------------------------------------------------------------------*/
graph_t * graph_init()
{
  graph_t *graph = (graph_t *) malloc(sizeof(graph_t));
  err_check(graph, "Allocating graph_t");

  graph->node_count = 0;
  graph->edge_count = 0;
  graph->nodes = NULL;

  return graph;
}

/*--------------------------------------------------------------------*/
void graph_add_edge(graph_t *graph, int src_idx, int dst_idx)
{
  int max_idx, i;

  // Ensure all nodes up to the highest index being added exist
  max_idx = src_idx >= dst_idx ? src_idx : dst_idx;
  if (src_idx >= graph->node_count || dst_idx >= graph->node_count) {
    debug(VERBOSE,
          "reallocating nodes from %d to %d\n",
          graph->node_count,
          max_idx);
    graph->nodes = resize_nodes(graph->nodes, max_idx);

    for (i = graph->node_count; i < max_idx; i++) {
      debug(VERBOSE, "initializing node %d\n", i);
      graph->nodes[i] = *(node_init(i));
    }

    graph->node_count = max_idx;
  }

  add_nbr(&(graph->nodes[src_idx]), &(graph->nodes[dst_idx]));
  graph->edge_count++;
}

/*--------------------------------------------------------------------*/
void graph_destroy(graph_t *graph)
{
  int i;
  for (i = 0; i < graph->node_count; i++) {
    debug(VERBOSE, "destroying node %d\n", i);
    node_destroy(&(graph->nodes[i]));
  }

  debug(VERBOSE, "freeing node array\n");
  if (graph->nodes != NULL)
    free(graph->nodes);

  debug(VERBOSE, "freeing graph pointer\n");
  free(graph);
}
