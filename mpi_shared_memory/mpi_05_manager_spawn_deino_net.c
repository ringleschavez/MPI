#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * source: https://mpi.deino.net/mpi_functions/MPI_Comm_spawn.html
 */

#define NUM_SPAWNS 2

int main( int argc, char *argv[] )
{
    int mpi_size, mpi_rank;
    char mpi_proc_name[MPI_MAX_PROCESSOR_NAME];
    int mpi_proc_name_len;

    int np = NUM_SPAWNS;
    int errcodes[NUM_SPAWNS];
    MPI_Comm parentcomm, intercomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
    MPI_Get_processor_name(mpi_proc_name, &mpi_proc_name_len);

    MPI_Comm_get_parent( &parentcomm );
    if (parentcomm == MPI_COMM_NULL)
    {
        /* Create 2 more processes - 
         * this example must be called spawn_example.exe for this to work. 
         */
        //MPI_Comm_spawn( "/home/mpiuser/clustershared/shared_memory/worker", MPI_ARGV_NULL, np, MPI_INFO_NULL, 
        MPI_Comm_spawn( "worker", MPI_ARGV_NULL, np, MPI_INFO_NULL, 
                        0, MPI_COMM_WORLD, &intercomm, errcodes );
        fprintf(stdout, "manager,rank=%d,size=%d,processor=%s\n",
                mpi_rank, mpi_size, mpi_proc_name);
    }
    else
    {
        fprintf(stdout, ">worker,rank=%d,size=%d,processor=%s\n", 
                mpi_rank, mpi_size, mpi_proc_name);
    }
    fflush(stdout);
    MPI_Finalize();
    return 0;
}
