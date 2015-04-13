#include <stdio.h>
#include <stdlib.h>
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
  edge_t current_edge;

  if ((fp=fopen(filename, "r"))==NULL)
  {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  // make graph from list of edges
  *graph = *(graph_init());
  edge_no = 0;
  debug(VERBOSE, "getting edges\n");
  while (get_edge(&current_edge, fp)) {
    debug(VERBOSE, "retrieved edge %d\n", edge_no);
    graph_add_edge(graph, current_edge.src, current_edge.dst);

    debug(VERBOSE,
          "added edge %d,  from %d to %d\n",
          edge_no,
          current_edge.src,
          current_edge.dst);
    edge_no++;
  }

  debug(HIGH, "retrieved graph:\n");
  debug_print_graph(HIGH, *graph);
  
  if (ferror(fp)) {
    fprintf(stderr, "Failure while reading from %s\n", filename);
    return 1;
  }

  return 0;
}

int load_edges(char *filename, edge_t **edges, int *edge_no)
{
  FILE* fp;
  int curr_size;
  edge_t current_edge;

  if ((fp=fopen(filename, "r"))==NULL)
  {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  // make array of edges
  curr_size = 1000;
  *edges = (edge_t *) malloc(curr_size * sizeof(edge_t *));
  *edge_no = 0;
  debug(VERBOSE, "getting edges\n");
  while (get_edge(&((*edges)[*edge_no]), fp)) {
    debug(VERBOSE, "retrieved edge %d\n", *edge_no);
    debug(VERBOSE,
          "added edge %d,  from %d to %d\n",
          *edge_no,
          current_edge.src,
          current_edge.dst);
    (*edge_no)++;
    //  resize array if necessary.
    if (*edge_no + 1 >= curr_size) {
    	curr_size += 1000;
    	debug(HIGH, "resizing edge array to %d elements\n", curr_size);
    	*edges = (edge_t *) realloc(*edges, curr_size * sizeof(edge_t *));
    }
  }

  // resize array to edge_no
  debug(HIGH, "resizing edge array to %d elements\n", *edge_no);
  *edges = (edge_t *) realloc(*edges, *edge_no * sizeof(edge_t *));

  debug(HIGH, "retrieved edges:\n");
  debug_print_edge_array(HIGH, *edges, *edge_no);

  if (ferror(fp)) {
    fprintf(stderr, "Failure while reading from %s\n", filename);
    return 1;
  }

  return 0;
}

int save_ranks(char *filename,  graph_t *graph)
{
  FILE* fp;
  int i;

  if ((fp=fopen(filename, "w"))==NULL)
  {
    printf("Fail to open the file!\n");
    return 1;
  }

  for (i = 0; i < graph->node_count; ++i) {
  	fprintf(fp, "%d\t%f\n", i, graph->nodes[i]->rank);
  }


  fclose(fp);
  return 0;
}

int get_edge(edge_t *edge, FILE *fp)
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
    *edge = *(edge_init(src, dst));
    return 1;
  }
  return 0;
}

void debug_print_node(int level, node_t node)
{
  int i;
  debug(level, "Node %d\n", node.idx);
  debug(level, "  Rank: %f\n", node.rank);
  debug(level, "  %d sources: ", node.incoming_count);
  for (i = 0; i < node.incoming_count; i++)
    debug(level, " %d", node.incoming[i]->idx);
  debug(level, "\n");
  debug(level, "  %d targets: ", node.outgoing_count);
  for (i = 0; i < node.outgoing_count; i++)
    debug(level, " %d", node.outgoing[i]->idx);
  debug(level, "\n");

  debug(level, "  %d external targets\n", node.outgoing_count_external);
}

void debug_print_graph(int level, graph_t graph)
{
  int i;
  debug(level, "%d nodes, %d edges:\n", graph.node_count, graph.edge_count);
  for (i = 0; i < graph.node_count; i++)
    if (!(graph.nodes[i]->empty))
      debug_print_node(level + 1, *(graph.nodes[i]));
}

void debug_print_all_edge_pairs(int level, int **edge_pairs, int *edge_counts, int num_procs)
{
	int proc;
	for (proc = 0; proc < num_procs; proc++) {
		debug(level, "Edge pairs for proc %d:\n", proc);
		debug_print_edge_pairs(level, edge_pairs[proc], edge_counts[proc]);
	}
}

void debug_print_edge_pairs(int level, int *edge_pairs, int edge_count)
{
	int i;
	for (i = 0; i < edge_count - 1; i += 2) {
		debug(level, "  %4d %4d\n", edge_pairs[i], edge_pairs[i + 1]);
	}
}

void debug_print_edge_array(int level, edge_t *edges, int size)
{
	int i;
	debug(level, "%d edges\n", size);
	for (i = 0; i < size; i++) {
		debug(level + 1, "  src: %4d  dst: %4d\n", edges[i].src, edges[i].dst);
	}
}

void debug_print_boundaries(int level, int ***incoming, int **incoming_counts, int ***outgoing, int **outgoing_counts, int num_procs)
{
	int proc;
	for (proc = 0; proc < num_procs; proc++) {
		debug(level, "----------\n");
		debug(level, "Boundaries for proc %d:\n", proc);
		debug_print_proc_boundaries(level, incoming[proc], incoming_counts[proc], outgoing[proc], outgoing_counts[proc], num_procs);
	}
}

void debug_print_proc_boundaries(int level, int **incoming, int *incoming_counts, int **outgoing, int *outgoing_counts, int num_procs)
{
	int proc, i;
	for (proc = 0; proc < num_procs; proc++) {
		debug(level, "  Target nodes of incoming edges from proc %d (length %d):\n", proc, incoming_counts[proc]);
		for (i = 0; i < incoming_counts[proc]; i++) {
		  debug(level, "    %d\n", incoming[proc][i]);
		}
		debug(level, "  Source nodes of outgoing edges to proc %d (length %d):\n", proc, outgoing_counts[proc]);
		for (i = 0; i < outgoing_counts[proc]; i++) {
		  debug(level, "    %d\n", outgoing[proc][i]);
		}
}
}
