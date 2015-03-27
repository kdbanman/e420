#include "graph.h"

#ifndef _IO_H_
#define _IO_H_

#define LINE_BUF_SIZE (1024)

int debug_level = 0;

void debug(int level, char *print_str);

int load_input(char *filename, adj_t **adjacency, int *node_count);
int save_ranks(char *filename,  adj_t *adjacency, int node_count);

int get_edge(edge_list_t *edge, FILE *fp);

#endif
