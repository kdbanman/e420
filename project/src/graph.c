#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "io.h"
#include "graph.h"

/*--------------------------------------------------------------------*/
/*-------------------INTERNAL FUNCTIONS-------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
node_t ** resize_nodes(node_t **nodes, int size)
{
  debug(VERBOSE, "resizing node pointer array to %d\n", size);
  return (node_t **) realloc(nodes, sizeof(node_t *) * size);
}

/*--------------------------------------------------------------------*/
/*-------------------PUBLIC FUNCTIONS---------------------------------*/
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
edge_t * edge_init(int src, int dst)
{
  edge_t *edge = (edge_t *) malloc(sizeof(edge_t));
  err_check(edge, "Allocating edge_t");

  edge->src = src;
  edge->dst = dst;
  
  return edge;
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
  node->incoming_count = 0;
  node->outgoing_count = 0;
  node->outgoing_count_global = 0;
  node->incoming = malloc(0);
  node->outgoing = malloc(0);
  node->rank = 0.0;
  
  return node;
}

/*--------------------------------------------------------------------*/
void add_nbr(node_t *src, node_t *dst)
{
  src->empty = 0;
  dst->empty = 0;

  src->outgoing_count++;
  src->outgoing_count_global++;
  dst->incoming_count++;

  debug(VERBOSE,
        "reallocating node %d targets <0x%x> to length %d\n",
        src->idx,
        src->outgoing,
        src->outgoing_count);
  src->outgoing = resize_nodes(src->outgoing, src->outgoing_count);
  err_check(src->outgoing, "Reallocating src->targets");

  debug(VERBOSE,
        "reallocating node %d sources <0x%x> to length %d\n",
        dst->idx,
        dst->incoming,
        dst->incoming_count);
  dst->incoming = resize_nodes(dst->incoming, dst->incoming_count);
  err_check(dst->incoming, "Reallocating dst->sources");

  debug(VERBOSE, "adding node %d to %d src->targets\n", dst->idx, src->idx);
  src->outgoing[src->outgoing_count - 1] = dst;

  debug(VERBOSE, "adding node %d to %d dst->sources\n", src->idx, dst->idx);
  dst->incoming[dst->incoming_count - 1] = src;
}

/*--------------------------------------------------------------------*/
void node_destroy(node_t *node)
{
	int i;

	for (i = 0; i < node->incoming_count; i++) {
			free(node->incoming[i]);
	}
	for (i = 0; i < node->outgoing_count; i++) {
		free(node->outgoing[i]);
	}
  free(node->incoming);
  free(node->outgoing);
  free(node);
}

/*--------------------------------------------------------------------*/
graph_t * graph_init()
{
  graph_t *graph = (graph_t *) malloc(sizeof(graph_t));
  err_check(graph, "Allocating graph_t");

  graph->node_count = 0;
  graph->edge_count = 0;
  graph->nodes = malloc(0);

  return graph;
}

/*--------------------------------------------------------------------*/
void graph_ensure_node(graph_t *graph, int node_idx)
{
	int necessary_length, i;

	necessary_length = node_idx + 1;

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
			graph->nodes[i] = node_init(i);
		}

		graph->node_count = necessary_length;
	}
}

/*--------------------------------------------------------------------*/
void graph_add_edge(graph_t *graph, int src_idx, int dst_idx)
{
  int max_idx;

  debug(VERBOSE, "adding edge to graph from %d to %d\n", src_idx, dst_idx);

  // Ensure all nodes up to the highest index being added exist
  max_idx = src_idx >= dst_idx ? src_idx : dst_idx;

  graph_ensure_node(graph, max_idx);

  add_nbr(graph->nodes[src_idx], graph->nodes[dst_idx]);
  graph->edge_count++;
}

/*--------------------------------------------------------------------*/
void graph_build(graph_t *graph, edge_t *edges, int size)
{
	int i;

  // make graph from list of edges
  *graph = *(graph_init());
  for (i = 0; i < size; i++) {
  	graph_add_edge(graph, edges[i].src, edges[i].dst);
  }
  debug_print_graph(HIGH, *graph);
}

/*--------------------------------------------------------------------*/
void graph_destroy(graph_t *graph)
{
  int i;
  for (i = 0; i < graph->node_count; i++) {
    debug(VERBOSE, "destroying node %d\n", i);
    node_destroy(graph->nodes[i]);
  }

  debug(VERBOSE, "freeing node array\n");
  if (graph->nodes != NULL)
    free(graph->nodes);

  debug(VERBOSE, "freeing graph pointer\n");
  free(graph);
}
