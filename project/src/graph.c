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
  debug(VERBOSE, "resizing node array to %d\n", size);
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
  debug(VERBOSE, "linking edge <0x%x> to <0x%x>\n", previous, next);
  previous->next = next;
}

/*--------------------------------------------------------------------*/
node_t * node_init(int idx)
{
  node_t *node = (node_t *) malloc(sizeof(node_t));
  err_check(node, "Allocating node_t");

  node->empty = 1;
  node->idx = idx;
  node->source_count = 0;
  node->target_count = 0;
  node->sources = NULL;
  node->targets = NULL;
  node->rank = 0.0;
  
  return node;
}

/*--------------------------------------------------------------------*/
void add_nbr(node_t *src, node_t *dst)
{
  src->empty = 0;
  dst->empty = 0;

  src->target_count++;
  dst->source_count++;

  debug(VERBOSE,
        "reallocating node %d targets <0x%x> to length %d\n",
        src->idx,
        src->targets,
        src->target_count);
  src->targets = resize_nodes(src->targets, src->target_count);
  err_check(src->targets, "Reallocating src->targets");

  debug(VERBOSE,
        "reallocating node %d sources <0x%x> to length %d\n",
        dst->idx,
        dst->sources,
        dst->source_count);
  dst->sources = resize_nodes(dst->sources, dst->source_count);
  err_check(dst->sources, "Reallocating dst->sources");

  debug(VERBOSE, "adding node %d to %d src->targets\n", dst->idx, src->idx);
  src->targets[src->target_count - 1] = *dst;

  debug(VERBOSE, "adding node %d to %d dst->sources\n", src->idx, dst->idx);
  dst->sources[dst->source_count - 1] = *src;
}

/*--------------------------------------------------------------------*/
void node_destroy(node_t *node)
{
  free(node->sources);
  free(node->targets);
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
  int necessary_length, i;

  debug(VERBOSE, "adding edge to graph from %d to %d\n", src_idx, dst_idx);

  // Ensure all nodes up to the highest index being added exist
  necessary_length = 1 + (src_idx >= dst_idx ? src_idx : dst_idx);
  if (necessary_length >= graph->node_count) {
    debug(VERBOSE,
          "reallocating nodes <0x%x> from size %d to size %d\n",
          graph->nodes,
          graph->node_count,
          necessary_length);
    graph->nodes = resize_nodes(graph->nodes, necessary_length);

    debug(VERBOSE, "nodes new addr <0x%x>\n", graph->nodes);

    for (i = graph->node_count; i < necessary_length; i++) {
      debug(VERBOSE, "initializing node %d\n", i);
      graph->nodes[i] = *(node_init(i));
    }

    graph->node_count = necessary_length;
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
