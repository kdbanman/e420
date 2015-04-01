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
    
    printf( "I am process %d of %d on machine %s, and I think Victoria is super hot.\n", rank, size, name);

    MPI_Finalize();
}
