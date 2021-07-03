#include "mpi.h"
#include <stdio.h>

/*
 * source: http://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+8+Parallel+Programming+with+MPI/8.2+Manager+Worker+Example/
 */
#define SIZE 1000
#define MIN( x, y ) ((x) < (y) ? x : y)

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

    numsent = 0;
    for ( i = 1; i < MIN( numprocs, SIZE ); i++ ) {
        MPI_Send(a[i-1],        /* buf */
                SIZE,           /* count */       
                MPI_DOUBLE,     /* datatype */
                i,              /* dest */ 
                i,              /* tag */
                MPI_COMM_WORLD  /* comm */);
        numsent++;
    }

    /* receive dot products back from workers */
    for ( i = 0; i < SIZE; i++ ) {
        MPI_Recv( &dotp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status );
        sender = status.MPI_SOURCE; /* the worker is waiting for next row or poison pill */
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


void worker_code( void )
{
    double b[SIZE], c[SIZE];
    int i, row, mpi_worker_rank;
    double dotp;
    MPI_Status status;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    for ( i = 0; i < SIZE; i++ ) /* (arbitrary) b initialization */
        b[i] = 1.0;

    MPI_Comm_rank( MPI_COMM_WORLD, &mpi_worker_rank );
    // Get the name of the processor
    MPI_Get_processor_name(processor_name, &name_len);
    if ( mpi_worker_rank <= SIZE ) {
        MPI_Recv( c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status );
#if 0
        __ric__
            fprintf(stdout, 
                    "worker,rank=%d,processor=%s,tag=%d\n", 
                    mpi_worker_rank, processor_name, status.MPI_TAG);
#endif

        while ( status.MPI_TAG > 0 ) {
            fprintf(stdout, 
                    "worker,rank=%d,processor=%s,tag=%d\n", 
                    mpi_worker_rank, processor_name, status.MPI_TAG);
            row = status.MPI_TAG - 1;
            dotp = 0.0;
            for ( i = 0; i < SIZE; i++ )
                dotp += c[i] * b[i];
            /* sending dot product to master */
            MPI_Send( &dotp, 1, MPI_DOUBLE, 0, row + 1,
                    MPI_COMM_WORLD );

            /* getting next row to work with */
            /* status.MPI_TAG = 0 = POISON PILL */
            MPI_Recv( c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG,
                    MPI_COMM_WORLD, &status );
        }
    } /* endif mpi_worker_rank <= SIZE */
}

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

