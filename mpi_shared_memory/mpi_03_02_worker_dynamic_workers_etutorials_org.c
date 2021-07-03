#include "mpi.h"
#include <stdio.h>

/*
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+8+Parallel+Programming+with+MPI/8.2+Manager+Worker+Example/
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+9+Advanced+Topics+in+MPI+Programming/
 */
#define SIZE 1000
#define MIN( x, y ) ((x) < (y) ? x : y)

#if 0
void manager_code( int numprocs )
{
    double a[SIZE][SIZE], c[SIZE];

    int i, j, sender, row, numsent = 0;
    double dotp;
    MPI_Status status;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    int manager_mpi_rank;

    MPI_Comm_rank( MPI_COMM_WORLD, &manager_mpi_rank );
    // Get the name of the processor
    MPI_Get_processor_name(processor_name, &name_len);
    fprintf(stdout, "manager,numprocs=%d,processor=%s,rank=%d\n", 
                    numprocs, processor_name, manager_mpi_rank);

    /* (arbitrary) initialization of a */
    for (i = 0; i < SIZE; i++ )
        for ( j = 0; j < SIZE; j++ )
            a[i][j] = ( double ) j;

    for ( i = 1; i < MIN( numprocs, SIZE ); i++ ) {
        MPI_Send( a[i-1], SIZE, MPI_DOUBLE, i, i, MPI_COMM_WORLD );
        numsent++;
    }
    /* receive dot products back from workers */
    for ( i = 0; i < SIZE; i++ ) {
        MPI_Recv( &dotp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                  MPI_COMM_WORLD, &status );
        sender = status.MPI_SOURCE;
        row    = status.MPI_TAG - 1;
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
}
#endif


//void worker_code( void )
int main(int argc, char *argv[])
{
    double b[SIZE], c[SIZE];
    int i, row, worker_mpi_rank;
    double dotp;
    MPI_Status status;
    MPI_Comm parentcomm;
    int numprocs;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &worker_mpi_rank);

    MPI_Comm_get_parent(&parentcomm);

    MPI_Bcast( b, SIZE, MPI_DOUBLE, 0, parentcomm);

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

    MPI_Comm_free(&parentcomm);
    MPI_Finalize();
    return 0;
}

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
