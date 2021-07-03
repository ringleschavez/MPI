#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int tag = 0;

MPI_Status status;

int manager_random(int n) {
    return rand() % n;
}

float** generate_matrix(int p_n_rows, int p_m_cols) {
    int i, j;
    float **x_matrix;
    //x_matrix = (float **) malloc(p_m_cols * sizeof(float));
    x_matrix = (float **) malloc(p_n_rows * sizeof(float));

    for (i = 0; i < p_n_rows; i++) {
        //x_matrx[i] = (float *) malloc(p_n_rows * sizeof(float));
        x_matrix[i] = (float *) malloc(p_m_cols * sizeof(float));
    }

    /* filling up */
    for (i = 0; i < p_n_rows; i++) {
        for (j = 0; j < p_m_cols; j++) {
            x_matrix[i][j] *= manager_random(100);
        }
    }
    return x_matrix;
}
int main(int argc, char** argv) {

    int my_rank;
    int num_procs;
    MPI_Comm workercomm;
    int n_rows = 4, m_cols = 5;
    float**matrix = generate_matrix(n_rows, m_cols);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //MPI_Comm_spawn("C:/Users/colegnou/workspace/worker/Debug/worker.exe",
    
#if 0
source: http://mpi.deino.net/mpi_functions/MPI_Comm_spawn.html
int MPI_Comm_spawn( char *command, char *argv[], int maxprocs, 
        MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);

int MPI_Comm_spawn( wchar_t *command, wchar_t *argv[], int maxprocs,
        MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
#endif

    /* a worker per row */
    MPI_Comm_spawn("worker",
            MPI_ARGV_NULL, n_rows,
            MPI_INFO_NULL, 0, MPI_COMM_SELF, &workercomm, MPI_ERRCODES_IGNORE);

    //for (int i = 0; i < m; i++) {
    for (int i = 0; i < n_rows; i++) {
        //MPI_Bcast(matrix[i], n, MPI_FLOAT, MPI_ROOT, workercomm);
        //matrix[i][0] *= (i+1)*6.66;
        MPI_Bcast(matrix[i], m_cols, MPI_FLOAT, MPI_ROOT, workercomm);
    }
    MPI_Finalize();
    return 0;
}
