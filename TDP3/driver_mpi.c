#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
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
    printf("%f\n",sum_exp);
    double frobenius_norm_exp = sqrt(sum_exp);    
    printf("%20.20f %20.20f\n", frobenius_norm_exp, frobenius_norm_res);
    test_res = (fabs(frobenius_norm_res-frobenius_norm_exp)/frobenius_norm_exp < ERROR_BOUND);
    if(!test_res){
	return 0;
    }
    return 1;
}
 
/********************/

int test_lu_mpi(){    
    int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double *check = alloc_matrix(N , N);
    memcpy(check, A, N*N*sizeof(double));
    /*Sequential*/
    lu_mpi(N,N,A,N,NULL,NULL);
   
    double *L = alloc_matrix(N , N);
    double *U = alloc_matrix(N , N);
    for (int i = 0; i < N; i++) {
	for (int j = 0; j <= i; j++) {
	    if(i==j)
		L[j*N + i] = 1.0;
	    else
		L[j*N + i] = A[j*N + i];
	}
    }
    
    for (int i = 0; i < N; i++) {
	for (int j = i; j < N; j++) {
	    U[j*N + i] = A[j*N + i];
	}
    }
    
    init_matrix(N, N, A, 1);
    mycblas_dgemm_scalaire(CblasRowMajor, CblasNoTrans,
			   CblasNoTrans, N, N,
			   N, 1, L,
			   N, U, N,
			   0, A, N);

    
 
    
    int res = check_matrix(check, A, N, N);
    
    
    free(L);
    free(U);
    free(check);
    return res; 
}

/*************Test main*************/
typedef struct{
    int(*fun)(void);
	char *msg;
    }test_function_t;


    test_function_t init_test(int (*fun)(void),char *msg){
	test_function_t tf;
	tf.fun = fun;
	tf.msg = msg;
	return tf;
    }

int main(int argc, char *argv[])
{
    int comm_rank, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    const int NB_TESTS = 1;
    test_function_t tests[] = {init_test(test_lu_mpi,"LU MPI TEST")};
    int ret;
    int passed = 0;

    
    ret = tests[0].fun();
    passed+=ret;
    printf("%-25s%6s\n", tests[0].msg, (!ret)?"\033[31;1mFAILED\033[0m":"\033[32;1mPASSED\033[0m");

    printf("\n%d out of %d tests passed.\033[0m\n",passed,NB_TESTS);

    MPI_Finalize();
    return 0;
}
