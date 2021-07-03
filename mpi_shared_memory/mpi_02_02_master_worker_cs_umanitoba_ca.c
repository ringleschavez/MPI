/*
 * source: http://www.cs.umanitoba.ca/~comp4510/examplesDIR/mmult.c
 */
#include <stdio.h>
#include "mpi.h"

#define NRA 62         /* number of rows in matrix A */ 
#define NCA 15         /* number of cols in matrix A */ 
#define NCB 7          /* number of cols in matrix B */ 
#define MASTER 0       /* id of the first process */ 
#define FROM_MASTER 1  /* setting a message type */ 
#define FROM_WORKER 2  /* setting a message type */ 

MPI_Status status; 

int main(int argc, char **argv) {

    int numprocs,   /* number of processes in partition */
        mpi_rank_id,         /* a process identifier */ 
        numworkers,     /* number of worker processes */
        source,         /* process id of message source */
        dest,           /* process id of message destination */
        nbytes,         /* number of bytes in message */
        mtype,          /* message type */
        intsize,        /* size of an integer in bytes */
        dbsize,         /* size of a double float in bytes */
        rows,           /* rows of A sent to each worker */ 
        averow, extra, offset, 
        i, j, k, a_count, b_count, c_count; 

    double  a[NRA][NCA],   /* matrix A to be multiplied */
            b[NCA][NCB],   /* matrix B to be multiplied */
            c[NRA][NCB];   /* result matrix C */

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    intsize = sizeof(int);
    dbsize = sizeof(double);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank_id);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    numworkers = numprocs-1; 

    /******* master process ***********/
    if (mpi_rank_id == MASTER) {
        for (i=0; i<NRA; i++)
            for (j=0; j<NCA; j++)
                a[i][j]= i+j;
        for (i=0; i<NCA; i++)
            for (j=0; j<NCB; j++)
                b[i][j]= i*j; 

        /* send matrix data to the worker processes */

        averow = NRA/numworkers; extra = NRA%numworkers;
        offset = 0;  mtype = FROM_MASTER;
        for (dest=1; dest<=numworkers; dest++) {
            rows = (dest <= extra) ? averow+1 : averow;
            MPI_Send(&offset,1,MPI_INT,dest,mtype, MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,dest,mtype, MPI_COMM_WORLD);
            /* a */
            a_count = rows*NCA;
            MPI_Send(&a[offset][0],a_count,MPI_DOUBLE,dest,mtype,   
                    MPI_COMM_WORLD);
            /* b */
            b_count = NCA*NCB;
            MPI_Send(&b, b_count,MPI_DOUBLE,dest,mtype, MPI_COMM_WORLD);
            offset = offset + rows;
        } 

        /* wait for results from all worker processes */
        mtype = FROM_WORKER;
        for (i=1; i<=numworkers; i++) {
            source = i;
            MPI_Recv(&offset,1,MPI_INT,source,mtype,MPI_COMM_WORLD, 
                    &status);
            MPI_Recv(&rows,1,MPI_INT,source,mtype,MPI_COMM_WORLD, 
                    &status);
            c_count = rows*NCB;
            MPI_Recv(&c[offset][0],c_count,MPI_DOUBLE,source,mtype, 
                    MPI_COMM_WORLD, &status);
        } 
        printf("element c[0,0] is %lf.\n", c[0][0]);

    } /* end of master */

    /************ worker process *************/
    if (mpi_rank_id > MASTER) {
        mtype = FROM_MASTER;
        source = MASTER;
#if 0
int MPI_Recv( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status
);
#endif
        MPI_Get_processor_name(processor_name, &name_len);

        MPI_Recv(&offset,1,MPI_INT,source,mtype,MPI_COMM_WORLD, &status);
        MPI_Recv(&rows,1,MPI_INT,source,mtype,MPI_COMM_WORLD, &status);

        a_count = rows*NCA;
        MPI_Recv(&a,a_count,MPI_DOUBLE,source,mtype,MPI_COMM_WORLD, &status);

        b_count = NCA*NCB;
        MPI_Recv(&b,b_count,MPI_DOUBLE,source,mtype,MPI_COMM_WORLD, &status);

        fprintf(stdout, 
                "worker,rank=%d,processor=%s,a_count=%d,b_count=%d\n", 
                mpi_rank_id, processor_name, a_count, b_count);

        for (k=0; k<NCB; k++)       /* multiply our part */
            for (i=0; i<rows; i++) { 
                c[i][k] = 0.0; 
                for (j=0; j<NCA; j++) 
                    c[i][k] = c[i][k] + a[i][j] * b[j][k]; 
            } 
        mtype = FROM_WORKER; 
        MPI_Send(&offset,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD);
        MPI_Send(&rows,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD);
        MPI_Send(&c,rows*NCB,MPI_DOUBLE,MASTER,mtype,
                MPI_COMM_WORLD);
    } /* end of worker */ 
    MPI_Finalize();

    return 0;
} /* of main */ 

