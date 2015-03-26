#include "graph.h"

#define LINE_BUF_SIZE (1024)

int load_input(char *filename, adj_t **adjacency, int *node_count);
int save_ranks(char *filename,  adj_t *adjacency, int node_count);
