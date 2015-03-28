#include <stdio.h>
#include <stdarg.h>
#include "graph.h"
#include "io.h"

void debug(int level, const char *fmt, ...)
{
  if (io_dbg_lev >= level) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}

int load_input(char *filename, graph_t *graph)
{	
  FILE* fp;
  int edge_no;
  edge_list_t first_edge, previous_edge, current_edge;

  if ((fp=fopen(filename, "r"))==NULL)
  {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  // populate linked list by getting first edge, then iterating through
  // remaining edges assuming that previous edge exists.
  debug(VERBOSE, "getting first edge\n");
  get_edge(&first_edge, fp);
  previous_edge = first_edge;
  
  edge_no = 0;
  
  debug(VERBOSE, "getting edge %d\n", edge_no);
  while (get_edge(&current_edge, fp)) {
    edge_no++;
    
    debug(VERBOSE, "linking edge %d to %d\n", edge_no - 1, edge_no);
    edge_list_connect(&previous_edge, &current_edge);
    previous_edge = current_edge;
    
    debug(VERBOSE, "getting edge %d\n", edge_no);
  }
  
  debug(LOW, "%d edges read into linked list.\n", edge_no);
  
  // count nodes and make adjacency list
  

  if (ferror(fp)) {
    fprintf(stderr, "Failure while reading from %s\n", filename);
    return 1;
  }

  return 0;
}

int save_ranks(char *filename,  graph_t *graph)
{
  FILE* fp;

  if ((fp=fopen(filename, "w"))==NULL)
  {
    printf("Fail to open the file!\n");
    return 1;
  }

  //TODO actually save
  //for (i=0; i<n; ++i) fprintf(fp, "%f\n", b[i]);

  fclose(fp);
  return 0;
}

int get_edge(edge_list_t *edge, FILE *fp)
{
  
  static char line[LINE_BUF_SIZE];
  int src, dst;

  src = -1;
  dst = -1;
  while (fgets(line, LINE_BUF_SIZE, fp)) {
    // fgets safely terminates buffer with \0, regardless of line size
    // ignore #comments and empty lines
    if (line[0] == '\0' || line[0] == '\n' || line[0] == '#') {
      debug(VERBOSE, "line ignored :%s\n", line);
      continue;
    }
    // read tab separated integers from line, detect errors
    sscanf(line, "%d\t%d\n", &src, &dst);
    if (src < 1 || dst < 1)
      fprintf(stderr, "Failure reading ints from line: %s\n", line);

    debug(VERBOSE, "initializing edge (%d, %d)\n", src, dst);
    // initialize edge and return control from loop
    edge = edge_init(src, dst);
    return 1;
  }
  return 0;
}

void debug_print_node(int level, node_t node)
{
  int i;
  debug(level, "Node %d\n", node.idx);
  debug(level, "  Rank: %f\n", node.rank);
  debug(level, "  %d neighbors: ", node.nbr_count);
  for (i = 0; i < node.nbr_count; i++)
    debug(level, " %d", node.nbrs[i].idx);
  debug(level, "\n");
}

void debug_print_graph(int level, graph_t graph)
{
  int i;
  debug(level, "%d nodes, %d edges:\n", graph.node_count, graph.edge_count);
  for (i = 0; i < graph.node_count; i++)
    debug_print_node(level, graph.nodes[i]);
}
