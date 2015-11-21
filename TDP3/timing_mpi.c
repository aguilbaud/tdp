#include <stdlib.h>
#include <mpi.h>
#include "util.h"
#include "perf.h"
#include "mycblas.h"

#define MAX_MATRIX_SIZE 5000


int main(int argc, char *argv[])
{
    
    int comm_size;	
    int comm_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);	
 
    int N = 8;
    double *A = alloc_matrix(N, N);
    init_matrix(N, N, A, 0);
    
    double start = MPI_Wtime();
    lu_mpi(N,N,A,N,NULL,NULL);
    double end = MPI_Wtime();
    double local_time = end-start;
    double global_time;
    MPI_Reduce(&local_time, &global_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(comm_rank == 0)
	printf("%d %f\n", comm_size, global_time);
 
    MPI_Finalize();
 
    return 0;
}

