#include <stdio.h>
#include "graph.h"

#ifndef _IO_H_
#define _IO_H_

#define LINE_BUF_SIZE (1024)

#define LOW (1)
#define HIGH (2)
#define VERBOSE (3)

int io_dbg_lev;

void debug(int level, const char *fmt, ...);

int load_input(char *filename, graph_t *graph);
int save_ranks(char *filename,  graph_t *graph);
int load_edges(char *filename, edge_t **edges, int *edge_no);

int get_edge(edge_t *edge, FILE *fp);

void debug_print_node(int level, node_t node);
void debug_print_graph(int level, graph_t graph);
void debug_print_edge_array(int level, edge_t *edges, int size);

#endif
