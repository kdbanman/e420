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

int load_input(char *filename, adj_t **adjacency, int *node_count);
int save_ranks(char *filename,  adj_t *adjacency, int node_count);

int get_edge(edge_list_t *edge, FILE *fp);

#endif
