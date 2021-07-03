#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int tag = 0;
MPI_Status status;

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);
    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    //int n = 4;
    //float*vector = (float *) malloc(n * sizeof(float));
    int m_cols = 5;
    float*vector = (float *) malloc(m_cols * sizeof(float));

    if (parent != MPI_COMM_NULL) {
        //MPI_Bcast(vector, n, MPI_FLOAT, MPI_ROOT, parent);
        MPI_Bcast(vector, m_cols, MPI_FLOAT, MPI_ROOT, parent);
    }
    fprintf(stdout, "rank=%d ->", mpi_rank);
    for (int i = 0; i < m_cols; i++) {
        fprintf(stdout, "%f ", vector[i]);
    }
    fprintf(stdout, "\n");

    MPI_Comm_free(&parent);
    free(vector);
    MPI_Finalize();
    return 0;
}
