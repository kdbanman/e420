#include <stdio.h>

#include "graph.h"

#define LINE_BUF_SIZE (1024)

int load_input(char *filename, adj_t **adjacency, int *node_count)
{	
  FILE* fp;
  char line[LINE_BUF_SIZE];
  int row;

  if ((fp=fopen(filename, "r"))==NULL)
  {
    fprintf(stderr, "Failed to open %s\n", filename);
    return 1;
  }

  while (fgets(line, LINE_BUF_SIZE, fp)) {
    // fgets safely terminates buffer with \0, regardless of line size
    // ignore #comments and empty lines
    if (line[0] == '\0' || line[0] == '\n' || line[0] == '#')
      continue;
    printf("%s", line);
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
