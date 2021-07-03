#define _GNU_SOURCE
#include <assert.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>

/*
 * source: https://mpi.deino.net/mpi_functions/MPI_Get_processor_processor_name.html
 */

/*
 * print_affinity
 * source: https://stackoverflow.com/questions/10490756/how-to-use-sched-getaffinity-and-sched-setaffinity-in-linux-from-c
 */
void print_affinity(const char *p_mpi_proc_name, const int p_mpi_rank) {
    cpu_set_t mask;
    long nproc, i;
    char affinity_msg[256] = {0};

    if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
        perror("sched_getaffinity");
        assert(false);
    }
    nproc = sysconf(_SC_NPROCESSORS_ONLN);
    sprintf(affinity_msg, 
            "rank=%d,processor=%s,sched_getaffinity=", 
            p_mpi_rank, p_mpi_proc_name);
    for (i = 0; i < nproc; i++) {
        sprintf(affinity_msg, "%s%d ", affinity_msg, CPU_ISSET(i, &mask));
    }
    fprintf(stdout, affinity_msg);
    fprintf(stdout, "\n");
}

int main(int argc, char *argv[])
{
    int rank, nprocs, len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(processor_name, &len);
    fprintf(stdout, "rank=%d,nprocs=%d,processor=%s\n", rank, nprocs, processor_name);
    print_affinity(processor_name, rank);
    fflush(stdout);
    MPI_Finalize();
    return 0;
}

