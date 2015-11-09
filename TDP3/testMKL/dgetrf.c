#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mkl.h"
#include "../util.h"
#include "../perf.h"
#include "../dgetf2.h"

void print_arr(int N, char * name, double* array);
void init_arr(int N, double* a);
void Dgemm_multiply(double* a,double*  b,double*  c, int N);


#define MAX_MATRIX_SIZE 10000

void timing_cblas_dgetf2_nopiv(){
  FILE * f_mkl = fopen("dgetrf_mkl.dat","w+");
  FILE * f_pers = fopen("dgetrf_perso.dat","w+");
  perf_t start, end;
  int N = 100;
  
  while(N < MAX_MATRIX_SIZE){
    double *A = alloc_matrix(N, N);
    int *ipiv = malloc( sizeof(int) * N );
    init_matrix(N, N, A, 0);
    
    //Perf My dgetf
    /* perf(&start); */
    /* dgetf_2_nopiv(N, N, A, N); */
    /* perf(&end); */
    /* perf_diff(&start, &end); */
    /* double nb_op =  (N*N * (N-1.0)); */
    /* double mflops = nb_op/ (double)(end.tv_sec * 1000000 + end.tv_usec); */
    /* fprintf(f_pers,"%d %f\n",N, mflops); */
    
    //Perf MKL dgetrf
    perf(&start);
    LAPACKE_dgetrf(CblasRowMajor, N, N, A, N, ipiv);
    perf(&end);
    perf_diff(&start, &end);
    double nb_op =  (N*N * (N-1.0));
    double mflops = nb_op/ (double)(end.tv_sec * 1000000 + end.tv_usec);
    fprintf(f_mkl,"%d %f\n",N, mflops);

    N*=1.25;
    free(A);
  }
  fclose(f_pers);
  fclose(f_mkl);
}


typedef struct{
  void(*fun)(void);
  char *msg;
}timing_function_t;


timing_function_t init_test(void (*fun)(void),char *msg){
  timing_function_t tf;
  tf.fun = fun;
  tf.msg = msg;
  return tf;
}

int main(){

  int NB_TESTS = 1;
  timing_function_t tests[] = {init_test(timing_cblas_dgetf2_nopiv,
					 "DGETF2 TIMING")}; 


  for(int i=0; i<NB_TESTS; i++){
    printf("%s\n",tests[i].msg);
    tests[i].fun();
  }
    
  return 0;
}









