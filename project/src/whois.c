#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "mpi.h"

main(int argc, char **argv ) {
    int rank, size;
    int buflen = 512;
    char name[buflen];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    gethostname(name, buflen);
    
    printf( "P rank %d of %d, host %s\n", rank, size, name);

    MPI_Finalize();
}
