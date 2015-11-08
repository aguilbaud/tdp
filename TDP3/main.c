#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dgemm.h"
#include "cblas.h"
#include "util.h"
#include "dgetf2.h"


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
    cblas_dscal(4, alpha, X, 1);
    return check_vector(check, X, 4);
}

int test_cblas_dger(){
    double X[]     = {5.0, 6.0, 7.0};
    double Y[]     = {-2.0, 0.0, 4.0};
    double res[] = {-1,4,5,-2,5,7,-1,8,1}; 
    double alpha = 2.0;
    double check[] = {-21.0,-20.0,-23.0,-2.0,5.0,7.0,39.0,56.0,57.0};
    cblas_dger(CblasRowMajor, 3, 3, alpha, X, 1, Y, 1, res, 3);
    return check_matrix(check, res, 3, 3);
}

int test_cblas_dgetf_2_nopiv(){
    int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double *check = alloc_matrix(N , N);
    memcpy(check, A, N*N*sizeof(double));
    dgetf_2_nopiv(N, N, A, N);

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
    cblas_dgemm_scalaire(CblasRowMajor, CblasNoTrans,
			 CblasNoTrans, N, N,
			 N, 0, L,
			 N, U, N,
			 0, A, N);
    
    return check_matrix(check, A, N, N);
}

int test_cblas_dtrsv_upper(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double x[] = {-4, 5, 6};
    double check[] = {-3.75, -3.5, 3};

    cblas_dtrsv(CblasRowMajor, CblasUpper, CblasNoTrans, CblasNonUnit, N, A, N, x, 1);
    return check_vector(check, x, N);    
}

int test_cblas_dtrsv_lower(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double x[] = {-4, 5, 6};
    double check[] = {-4, 17, 5};

    cblas_dtrsv(CblasRowMajor, CblasLower, CblasNoTrans, CblasUnit, N, A, N, x, 1);
    affiche(1, N, x, 1, stdout);
    return check_vector(check, x, N);    
}

int test_cblas_dtrsm_upper(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double X[] = {-1,-5,3,2,7,-2,4,-1,1};
    double check[] = {8,-5.5,1.5,-9.25,5.5,-1,4,-1.5,0.5};
    
    cblas_dtrsm(CblasRowMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, N, N, 1.0, A, N, X, N);
    affiche(N, N, X, N, stdout);
    return check_matrix(check, X, N, N);    

}

int test_cblas_dtrsm_lower(){
    const int N = 3;
    double A[] = {2,3,4,5,2,1,7,4,2};
    double X[] = {-1,-5,3,2,7,-2,4,-1,-1};
    double check[] = {-1,-2,9,2,1,-11,4,-13,-4};
    
    cblas_dtrsm(CblasRowMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, N, N, 1.0, A, N, X, N);
    affiche(N, N, X, N, stdout);
    return check_matrix(check, X, N, N);    

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
	const int NB_TESTS = 7;
	test_function_t tests[] = {init_test(test_cblas_dscal,"DSCAL TEST") , 
				   init_test(test_cblas_dger, "DGER TEST"),
				   init_test(test_cblas_dgetf_2_nopiv, "DGETF NO PIV"),
				   init_test(test_cblas_dtrsv_upper, "DTRSV UPPER"),
				   init_test(test_cblas_dtrsv_lower, "DTRSV LOWER"),
				   init_test(test_cblas_dtrsm_upper, "DTRSM UPPER"),
				   init_test(test_cblas_dtrsm_lower, "DTRSM LOWER")};
			       
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
