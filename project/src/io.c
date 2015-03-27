#include <stdio.h>
#include "graph.h"
#include "io.h"

int load_input(char *filename, adj_t **adjacency, int *node_count)
{	
  FILE* fp;
  char line[LINE_BUF_SIZE];
  int row;
  edge_list_t *first_edge, *previous_edge, *current_edge;

  if ((fp=fopen(filename, "r"))==NULL)
  {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  // populate linked list by grounding first edge, then
  // TODO THIS WON'T WORK I'M JUST MAKING A TINY LINKED CYCLE DAMMIT
  get_edge(first_edge, fp, NULL);
  previous_edge = first_edge;
  while (get_edge(current_edge, fp, previous_edge)) {
    previous_edge = current_edge;
  }

  if (ferror(fp)) {
    fprintf(stderr, "Failure while reading from %s\n", filename);
    return 1;
  }

  return 0;
}

int save_ranks(char *filename,  adj_t *adjacency, int node_count)
{
  FILE* fp;

  if ((fp=fopen(filename, "w"))==NULL)
  {
    printf("Fail to open the file!\n");
    return 1;
  }

  //for (i=0; i<n; ++i) fprintf(fp, "%f\n", b[i]);

  fclose(fp);
  return 0;
}

int get_edge(edge_list_t *edge, FILE *fp, edge_list_t *previous)
{
  int src, dst;

  src = -1;
  dst = -1;
  while (fgets(line, LINE_BUF_SIZE, fp)) {
    // fgets safely terminates buffer with \0, regardless of line size
    // ignore #comments and empty lines
    if (line[0] == '\0' || line[0] == '\n' || line[0] == '#')
      continue;
    // read tab separated integers from line, detect errors
    sscanf(line, "%d\t%d\n", &src, &dst);
    if (src < 1 || dst < 1)
      fprintf(stderr, "Failure reading ints from line: %s\n", line);

    // initialize edge and return control from loop
    edge_init(edge, src, dst, previous);
    return 1;
  }
  return 0;
}
