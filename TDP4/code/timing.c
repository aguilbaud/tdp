#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include "mpi_fox.h"
#include "util.h"
#include "mycblas.h"


/*********************Tests functions*********************/
void timing_fox_mpi(int dim, char *path_A, char *path_B, char *path_C){
    int comm_rank;
    int loc_N, N;
    loc_N = N = 0; /*Dummy init to avoid compiler warning*/

    double *A=NULL, *B=NULL, *C=NULL;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    /*One process load the matrice and scatter it*/
    if(comm_rank == 0){
	int na, nb, nc;
	FILE *fp;

	fp = fopen(path_A, "r");
	A = load_file(fp, &na);
	fclose(fp);

	fp = fopen(path_B, "r");
	B = load_file(fp, &nb);
	fclose(fp);

	fp = fopen(path_C, "r");
	C = load_file(fp, &nc);
	fclose(fp);
	
	
	if(na!=nb || nb!=nc || na!=nc){
	    fprintf(stderr, "Abort: Matrix have to be of the same order\n");
	    MPI_Abort(MPI_COMM_WORLD, -1);
	    return;
	}
	
	N=na;
	loc_N = N/dim;

	if(loc_N * dim != N){
	    fprintf(stderr, "There is too much processus for this problem\n");
	    free(A); free(B); free(C);
	    MPI_Abort(MPI_COMM_WORLD, -1);
	    return;
	}
    }

    MPI_Barrier(MPI_COMM_WORLD);
    fox(A, B, C, loc_N, dim, 1);
    

    free(A); free(B); free(C);
    return;
}

typedef struct{
    void(*fun)(int, char*, char *, char*);
	char *msg;
    }test_function_t;


    test_function_t init_test(void (*fun)(int, char*, char *, char*),char *msg){
	test_function_t tf;
	tf.fun = fun;
	tf.msg = msg;
	return tf;
    }


    int main(int argc, char *argv[])
    {
	int comm_size;	
	int comm_rank;
	int dim;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
	dim = sqrt(comm_size);

	if(dim*dim != comm_size){
	    fprintf(stderr, "The number of processus must be a power\n");
	    MPI_Finalize();
	    return 0;
	}
    
	test_function_t tests[] = {init_test(timing_fox_mpi,"FOX TIMING")};    
	tests[0].fun(dim, argv[1], argv[2], argv[3]);

        
	MPI_Finalize();
	return 0;
    }
