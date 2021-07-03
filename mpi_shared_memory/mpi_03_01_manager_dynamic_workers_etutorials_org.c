#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+8+Parallel+Programming+with+MPI/8.2+Manager+Worker+Example/
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+9+Advanced+Topics+in+MPI+Programming/
 */
#define SIZE 1000
#define MIN( x, y ) ((x) < (y) ? x : y)

//void manager_code( int numprocs )
int main(int argc, char *argv[])
{
    double a[SIZE][SIZE], b[SIZE], c[SIZE]; // double a[SIZE][SIZE], c[SIZE];

    int i, j, row, num_workers; // int i, j, sender, row, numsent = 0;
    int sender;
    int numsent = 0;
    double dotp;
    MPI_Status mpi_status;
    MPI_Comm worker_mpi_comm;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    int manager_mpi_rank;

    MPI_Init( &argc, &argv );
    //if ( argc != 2 || !isnumeric( argv[1] ))
    if ( argc != 2 || !isdigit( argv[1] ))
        printf( "usage: %s <number of workers>\n", argv[0] );
    else
        num_workers = atoi( argv[1] );

    MPI_Comm_rank( MPI_COMM_WORLD, &manager_mpi_rank );
    // Get the name of the processor
    MPI_Get_processor_name(processor_name, &name_len);
    fprintf(stdout, "manager,num_workers=%d,processor=%s,rank=%d\n", 
                    num_workers, processor_name, manager_mpi_rank);

    MPI_Comm_spawn( "worker", MPI_ARGV_NULL, num_workers,
               MPI_INFO_NULL,
               0, MPI_COMM_SELF, &worker_mpi_comm, MPI_ERRCODES_IGNORE );

    /* (arbitrary) initialization of a and b */
    for (i = 0; i < SIZE; i++ )
        b[i] = 1.0;
        for ( j = 0; j < SIZE; j++ )
            a[i][j] = ( double ) j;

    /* send b to each worker */
    MPI_Bcast( b, SIZE, MPI_DOUBLE, MPI_ROOT, worker_mpi_comm );

    for ( i = 1; i < MIN( num_workers, SIZE ); i++ ) {
        MPI_Send( a[i-1], SIZE, MPI_DOUBLE, i, i, MPI_COMM_WORLD );
        numsent++;
    }

    /* receive dot products back from workers */
    for ( i = 0; i < SIZE; i++ ) {
        MPI_Recv( &dotp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &mpi_status );
        sender = mpi_status.MPI_SOURCE;
        row    = mpi_status.MPI_TAG - 1;
        c[row] = dotp;
        /* send another row back to this worker if there is one */
        if ( numsent < SIZE ) {
            MPI_Send( a[numsent], SIZE, MPI_DOUBLE, sender,
                      numsent + 1, MPI_COMM_WORLD );
            numsent++;
        }
        else                    /* no more work */
            MPI_Send( MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0,
                       MPI_COMM_WORLD );
    }

    MPI_Finalize( );
    return 0;
}


#if 0
void worker_code( void )
{
    double b[SIZE], c[SIZE];
    int i, row, worker_mpi_rank;
    double dotp;
    MPI_Status status;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    for ( i = 0; i < SIZE; i++ ) /* (arbitrary) b initialization */
        b[i] = 1.0;

    MPI_Comm_rank( MPI_COMM_WORLD, &worker_mpi_rank );
    // Get the name of the processor
    MPI_Get_processor_name(processor_name, &name_len);
    fprintf(stdout, "worker,processor=%s,rank=%d\n", processor_name, worker_mpi_rank);
    if ( worker_mpi_rank <= SIZE ) {
        MPI_Recv( c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &status );
        while ( status.MPI_TAG > 0 ) {
            row = status.MPI_TAG - 1;
            dotp = 0.0;
            for ( i = 0; i < SIZE; i++ )
                dotp += c[i] * b[i];
            MPI_Send( &dotp, 1, MPI_DOUBLE, 0, row + 1,
                      MPI_COMM_WORLD );
            MPI_Recv( c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &status );
        }
    }
}
#endif

#if 0
int main( int argc, char *argv[] )
{
    int numprocs, myrank;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

    if ( myrank == 0 )          /* manager process */
        manager_code ( numprocs );
    else                        /* worker process */
        worker_code ( );
    MPI_Finalize( );
    return 0;
}
#endif

