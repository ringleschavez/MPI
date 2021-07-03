#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /*  getpid() */

/*
 * source: http://www.umsl.edu/~siegelj/CS4740_5740/MPI_Revisited/mpi_spawn.html
 */

#define NUM_SPAWNS 3

int main(int argc, char *argv[])  {
    int numtasks, rank, task1=1, task2=2, rc, count,len,dest,source,buffer[2];  
    char inmsg[30], outmsg0[]="Hello Other Task.",outmsg1[]="You are Welcome Other Task.";
    char name[MPI_MAX_PROCESSOR_NAME];
    int np =NUM_SPAWNS;
    int errcodes[NUM_SPAWNS];
    MPI_Comm parentcomm, intercomm,intracomm;

#if 0
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <n> <m>\n", argv[0]);
        exit(1);
    }
#endif

    MPI_Init( &argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_get_parent( &parentcomm );

    if (parentcomm == MPI_COMM_NULL) {
        buffer[0]=atoi(argv[1]); 
        buffer[1]=atoi(argv[2]);

        fprintf(stdout, "manager,pid=%d \n",getpid());

        /* Create 3 more processes - Need to use the actual executables name. */

        MPI_Comm_spawn("mpi_spawn", 
                MPI_ARGV_NULL, np, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, errcodes );

        fprintf(stdout, "manager, after the Spawn\n");
        MPI_Get_processor_name(name, &len);
        MPI_Intercomm_merge(intercomm, 0, &intracomm);   /* The 0 for the merge is important here*/
        fprintf(stdout, "manager,rank=%i,processor=%s,pid=%d\n", rank, name, getpid());
        MPI_Bcast(buffer, 2, MPI_INT, 0, intracomm);
    } else {
        /*MPI_Comm_get_parent - Now returns the parent intercommunicator of current spawned process. */
        MPI_Comm_get_parent( &parentcomm );
        MPI_Get_processor_name(name, &len);
        MPI_Intercomm_merge(parentcomm, 1, &intracomm);   /* The 1 for the merge is important here*/
        MPI_Bcast(buffer, 2, MPI_INT, 0, intracomm);   
        MPI_Comm_rank(intracomm, &rank);//MPI_COMM_WORLD
        fprintf(stdout, ">worker,rank=%d,processor=%s,pid=%d\n",
                rank, name, getpid());
    }

    if (rank ==buffer[1]) {
        dest = buffer[0];
        source = buffer[0];
        MPI_Send(&outmsg0,strlen(outmsg0), MPI_CHAR, dest, 0,intracomm);
        fprintf(stdout, "\nTask_rank=%i has sent its  message to Task_rank=%i. \n",rank, dest);
        MPI_Recv(&inmsg, 30, MPI_CHAR, dest, 1, intracomm,  MPI_STATUS_IGNORE);
        fprintf(stdout, "Task_rank=%i received this  message from Task_rank=%i; %s\n\n",rank, dest, inmsg);
    } else if (rank ==buffer[0]) {
        dest = buffer[1];
        source =buffer[1];
        MPI_Recv(&inmsg,30, MPI_CHAR, source, 0, intracomm,  MPI_STATUS_IGNORE);
        fprintf(stdout, "Task_rank=%i received this message: %s\n", rank, inmsg);
        MPI_Send(&outmsg1,strlen(outmsg1), MPI_CHAR, source, 1, intracomm);    
    }
    MPI_Barrier(  intracomm ) ;
    printf("Got here %i\n",rank);
    MPI_Finalize();
    return 0;
}
