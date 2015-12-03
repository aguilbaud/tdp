#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include "mpi_fox.h"
#include "util.h"
#include "mycblas.h"

#define ERROR_BOUND 1E-6

int check_matrix(double *exp, double *res, int M, int N){
    int test_res;
    double sum_res = 0.0;
    double sum_exp = 0.0;
    for (int i = 0; i < M; i++) {
	double sum_res_line = 0.0;
	double sum_exp_line = 0.0;
	for (int j = 0; j < N; j++) {
	    sum_res_line += res[j*N + i] * res[j*N + i];
	    sum_exp_line += exp[j*N + i] * exp[j*N + i];
	}
	sum_res += sum_res_line;
	sum_exp += sum_exp_line;
    }
    
    double frobenius_norm_res = sqrt(sum_res);
    double frobenius_norm_exp = sqrt(sum_exp);    
    //printf("%20.20f %20.20f\n", frobenius_norm_exp, frobenius_norm_res);
    test_res = (fabs(frobenius_norm_res-frobenius_norm_exp)/frobenius_norm_exp < ERROR_BOUND);
    if(!test_res){
	return 0;
    }
    return 1;
}

/*********************Tests functions*********************/
int test_fox_mpi(int dim){
    int comm_rank;
    int loc_N, N;
    loc_N = N = 0; /*Dummy init to avoid compiler warning*/

    double *A=NULL, *B=NULL, *C=NULL, *C_bis=NULL;
    char *path_A = "data/simple.mat";
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    /*One process load the matrice and scatter it*/
    if(comm_rank == 0){
	int na, nb, nc;
	FILE *fp;

	fp = fopen(path_A, "r");
	A = load_file(fp, &na);
	fclose(fp);

	fp = fopen(path_A, "r");
	B = load_file(fp, &nb);
	fclose(fp);

	fp = fopen(path_A, "r");
	C = load_file(fp, &nc);
	fclose(fp);
	
	if(na!=nb || nb!=nc || na!=nc){
	    fprintf(stderr, "Abort: Matrix have to be of the same order\n");
	    MPI_Abort(MPI_COMM_WORLD, -1);
	    return 0;
	}
	
	N=na;
	loc_N = N/dim;
	if(loc_N * dim != N){
	    fprintf(stderr, "Bad\n");
	    free(A); free(B); free(C);
	    MPI_Abort(MPI_COMM_WORLD, -1);
	    return 0;
	}
	
	C_bis = malloc(sizeof(double)*N*N);
	memcpy(C_bis, C, sizeof(double) * N*N);
    }

    fox(A, B, C, loc_N, dim);
    
    if(comm_rank == 0){
	mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans, N, N, N, 
			       1.0, A, N, B, N, 0.0, C_bis, N);
	
	int res = check_matrix(C_bis, C, N, N);
	free(A); free(B); free(C);
	return res;
    }
    
    free(A); free(B); free(C);
    return -1;
}

typedef struct{
    int(*fun)(int);
	char *msg;
    }test_function_t;


    test_function_t init_test(int (*fun)(int),char *msg){
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

	int s = sqrt(comm_size);
	if(s*s != comm_size){
	    fprintf(stderr, "The number of processus must be a power\n");
	    MPI_Finalize();
	    return 0;
	}
    
	const int NB_TESTS = 1;
	test_function_t tests[] = {init_test(test_fox_mpi,"FOX TEST")};
	int ret;
	int passed = 0;

    
	ret = tests[0].fun(dim);
	if(ret!=-1){
	
	    passed+=ret;
	    printf("%-25s%6s\n", tests[0].msg, (!ret)?"\033[31;1mFAILED\033[0m":"\033[32;1mPASSED\033[0m");
	
	    printf("\n%d out of %d tests passed.\033[0m\n",passed,NB_TESTS);
	}

    
	MPI_Finalize();
	return 0;
    }
