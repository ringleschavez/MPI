#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

/*
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+8+Parallel+Programming+with+MPI/8.1+Hello+World+in+MPI/
 */

int main( int argc, char *argv[] )
{
    int  mpi_comm_size, mpi_comm_rank, processor_name_len, i;
    char processor_name[MPI_MAX_PROCESSOR_NAME] = {0};
    char msg_to_be_sent[MPI_MAX_PROCESSOR_NAME + 80] = {0};
    MPI_Status status;

    char *recv_buffer;
    int recv_buffer_len, recv_buffer_src;

    double mpi_start_time, mpi_stop_time;

    /* initialization */
    MPI_Init( &argc, &argv );
    mpi_start_time = MPI_Wtime();
    /* ecosystem */
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_comm_size );
    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_comm_rank );
    /* processor = node */
    MPI_Get_processor_name( processor_name, &processor_name_len );

#define __ric__testing__
#ifdef __ric__testing__
    sprintf(msg_to_be_sent, "rank=%d of size=%d on processor=%s, start=%ld",
        mpi_comm_rank, 
        mpi_comm_size, 
        processor_name,
        mpi_start_time);

    if ( mpi_comm_rank == 0 ) {

        fprintf(stdout, "RANK==0,%s\n", msg_to_be_sent );

        for ( i = 1; i < mpi_comm_size; i++ ) {
#if 0
            MPI_Recv( msg_to_be_sent, sizeof( msg_to_be_sent ), MPI_CHAR,
                      i, 1, MPI_COMM_WORLD, &status );
#else
            MPI_Probe( MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status );
            MPI_Get_count(&status, MPI_CHAR, &recv_buffer_len);
            recv_buffer = (char *)malloc(recv_buffer_len);
            recv_buffer_src = status.MPI_SOURCE;
            /* recv */
            MPI_Recv(recv_buffer, 
                    recv_buffer_len, 
                    MPI_CHAR, 
                    recv_buffer_src, 
                    1, 
                    MPI_COMM_WORLD, 
                    &status );
#endif
            fprintf(stdout, "RANK0 got %s\n", recv_buffer);
        }
        fflush(stdout);
    }
    else {
        mpi_stop_time = MPI_Wtime();
        sprintf(msg_to_be_sent, 
                "%s,stop=%ld,%ld", 
                msg_to_be_sent, 
                mpi_stop_time,
                mpi_stop_time - mpi_start_time);

        MPI_Send(msg_to_be_sent,            /* buf */
                strlen(msg_to_be_sent) + 1, /* count */
                MPI_CHAR,                   /* MPI datatype */
                0,                          /* MPI dest */
                1,                          /* MPI tag */
                MPI_COMM_WORLD);            /* MPI comm */
    }

#else
    fprintf(stdout, "%s\n", msg_to_be_sent);
    fflush(stdout);
#endif
    MPI_Finalize( );
    return 0;
}

