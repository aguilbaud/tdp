#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "mycblas.h"
#include "util.h"


#define ERROR_BOUND 1E-6

/**********Checking functions************/
int check_vector(double *exp, double *res, int N){
    for (int i = 0; i < N; i++) {
	if( (fabs(res[i]-exp[i])/fabs(exp[i]) < ERROR_BOUND) == 0)
	    return 0;
    }
    return 1;
}

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


/*************Test functions*************/
int test_cblas_dscal(){
    double X[]     = {5, 6, 7, -4};
    double check[] = {10, 12, 14, -8};
    double alpha = 2;
    mycblas_dscal(4, alpha, X, 1);
    return check_vector(check, X, 4);
}

int test_cblas_dger(){
    double X[]     = {5.0, 6.0, 7.0};
    double Y[]     = {-2.0, 0.0, 4.0};
    double res[] = {-1,4,5,-2,5,7,-1,8,1}; 
    double alpha = 2.0;
    double check[] = {-21.0,-20.0,-23.0,-2.0,5.0,7.0,39.0,56.0,57.0};
    mycblas_dger(CblasRowMajor, 3, 3, alpha, X, 1, Y, 1, res, 3);
    return check_matrix(check, res, 3, 3);
}

int test_cblas_dgetf2_nopiv(){
    int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double *check = alloc_matrix(N , N);
    memcpy(check, A, N*N*sizeof(double));
    mycblas_dgetf2_nopiv(N, N, A, N);
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


int test_cblas_dtrsv_upper(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double x[] = {-4, 5, 6};
    double check[] = {-3.75, -3.5, 3};

    mycblas_dtrsv(CblasRowMajor, CblasUpper, CblasNoTrans, CblasNonUnit, N, A, N, x, 1);
    return check_vector(check, x, N);    
}

int test_cblas_dtrsv_lower(){
    const int N = 3;
	double A[] = {2,3,4,5,2,1,7,4,2};
	double x[] = {-4, 5, 6};
	double check[] = {-4, 17, 5};

	mycblas_dtrsv(CblasRowMajor, CblasLower, CblasNoTrans, CblasUnit, N, A, N, x, 1);
	return check_vector(check, x, N);    
    }

int test_cblas_dtrsm_upper(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double X[] = {-1,-5,3,2,7,-2,4,-1,1};
    double check[] = {8,-5.5,1.5,-9.25,5.5,-1,4,-1.5,0.5};
    
    mycblas_dtrsm(CblasRowMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, N, N, 1.0, A, N, X, N);
    return check_matrix(check, X, N, N);    

}

int test_cblas_dtrsm_lower(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double X[] = {-1,-5,3,2,7,-2,4,-1,-1};
    double check[] = {-1,-2,9,2,1,-11,4,-13,-4};
    mycblas_dtrsm(CblasRowMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, N, N, 1.0, A, N, X, N);
    
    return check_matrix(check, X, N, N);        
    
}

int test_dgetrf(){
    int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double B[] = {2,3,4,5,2,1,7,4,2};
    int ipiv[3];
    int info;
  
    mycblas_dgetf2_nopiv(N, N, A, N);  
    mycblas_dgetrf(N, N, B, N, ipiv, &info);

    return check_matrix(A, B, N, N);
}

int test_dgetrf2(){

    int N=8;
    double A[] = {5,-8,4,3,0,-6,-1,-10,
		  -7,7,-5.5555555555555555,0,-9,7,0,-7,
		  2.4444444444444444,-10,-4,-5,-2,7,5,-8,
		  8,-1.1111111111111111,0,1,-5,9,8,1,
		  7,1,-1,1,-6,-8,3,8.8888888888888888,
		  -1,-1,-3,-6.6666666666666666,7.7777777777777777,6,2,4,
		  4,6,-2,-2,5,-2,7,5,
		  -3,1.7777777777777777,10,-9,10,-9.9999999999999999,4,6};
    int ipiv[3];
    int info;
    double *check = alloc_matrix(N , N);
    memcpy(check, A, N*N*sizeof(double));

    mycblas_dgetrf(N, N, A, N, ipiv, &info);
    
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

    return res;

}

int test_dgesv(){
    int Na = 8;
	int Nb = 1;
	double A[] = {5,-8,4,3,0,-6,-1,-10,
		      -7,7,-5.5555555555555555,0,-9,7,0,-7,
		      2.4444444444444444,-10,-4,-5,-2,7,5,-8,
		      8,-1.1111111111111111,0,1,-5,9,8,1,
		      7,1,-1,1,-6,-8,3,8.8888888888888888,
		      -1,-1,-3,-6.6666666666666666,7.7777777777777777,6,2,4,
		      4,6,-2,-2,5,-2,7,5,
		      -3,1.7777777777777777,10,-9,10,-9.9999999999999999,4,6};
    
	double B[] = {-5.2,3,5.1,-8.1,71,-9.45,-45.32,12};
	double *C = calloc(Na*Nb, sizeof(double));
	double *Acopy = calloc(Na*Na, sizeof(double));
	double *check = malloc(sizeof(double) * Na*Nb);
	memcpy(check,B, sizeof(double) * Na*Nb);
	memcpy(Acopy,A, sizeof(double) * Na*Na);

	mycblas_dgesv(Na, Nb, A, Na, NULL, B, Na, NULL);
	
	mycblas_dgemm_scalaire(CblasRowMajor, CblasNoTrans,
			       CblasNoTrans, Na, Nb, Na, 1.0, Acopy, Na, B, Na, 0.0, C, Na);
	
	int ret = check_matrix(check, C, Na, Nb) ;
	free(C);
	free(Acopy);
	free(check);
	return ret;
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

	const int NB_TESTS = 10;
	test_function_t tests[] = {init_test(test_cblas_dscal,"DSCAL TEST"), 
				   init_test(test_cblas_dger, "DGER TEST"),
				   init_test(test_cblas_dgetf2_nopiv, "DGETF NO PIV"),
				   init_test(test_cblas_dtrsv_upper, "DTRSV UPPER"),
				   init_test(test_cblas_dtrsv_lower, "DTRSV LOWER"),
				   init_test(test_cblas_dtrsm_upper, "DTRSM UPPER"),
				   init_test(test_cblas_dtrsm_lower, "DTRSM LOWER"),
				   init_test(test_dgetrf, "DGETRF TEST"),
				   init_test(test_dgetrf2, "DGETRF2 TEST"),
				   init_test(test_dgesv, "DGESV TEST") };
	int ret;
	int passed = 0;

	for(int i=0; i<NB_TESTS; i++){
	    ret = tests[i].fun();
	    passed+=ret;
	    printf("%-25s%6s\n", tests[i].msg, (!ret)?"\033[31;1mFAILED\033[0m":"\033[32;1mPASSED\033[0m");
	}
	printf("\n%d out of %d tests passed.\033[0m\n",passed,NB_TESTS);
	return 0;
    }
