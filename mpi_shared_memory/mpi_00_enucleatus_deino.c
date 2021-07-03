#include "mpi.h"
#include <stdio.h>
/*
 * source: https://mpi.deino.net/mpi_functions/MPI_Get_processor_name.html
 */
int main(int argc, char *argv[])
{
    int rank, nprocs, len;
    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(name, &len);
    fprintf(stdout, "rank=%d,nprocs=%d,processor=%s\n", rank, nprocs, name);
    fflush(stdout);
    MPI_Finalize();
    return 0;
}
 
