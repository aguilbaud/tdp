/* Contient les fonctions permettant de générer les courbes de performances*/

#include <stdlib.h>
#include "perf/perf.h"
#include "util.h"
#include "cblas.h"
#include "dgemm.h"

#define NB_TESTS 3
#define MAX_MATRIX_SIZE 1000
#define MAX_VECTOR_SIZE 1E6

void timing_cblas_ddot(){
  FILE * f = fopen("curves/data/ddot_mflops.dat","w+");
  int size = 50;

  while(size < MAX_VECTOR_SIZE){
    double *x = alloc_vector(size);
    double *y = alloc_vector(size);
    init_vector(size,x);
    init_vector(size,y);
    
    perf_t p1, p2;
    perf(&p1);
    cblas_ddot(size,x,1,y,1);
    perf(&p2);
    
    perf_diff(&p1,&p2);
    double mflops = perf_mflops(&p2,size*2);
    fprintf(f,"%d %f\n",size, mflops);
    
    size*=1.25;
    free(x);
    free(y);
  }
}

void timing_compare_cblas_dgemm_scalaire(){
  FILE * f = fopen("curves/data/dgemm_scalaires_mflops.dat","w+");
  int size = 100;
  void (*fun[3])() = {cblas_dgemm_scalaire_kij,
			cblas_dgemm_scalaire_ijk,
			cblas_dgemm_scalaire_jik};

  while(size < MAX_MATRIX_SIZE){
    int lda, ldb, ldc;
    lda=ldb=ldc=size;
    double *m1 = alloc_matrix(size,size); 
    double *m2 = alloc_matrix(size,size);
    double *m3 = alloc_matrix(size,size);
    init_matrix(size,size,m1,0);
    init_matrix(size,size,m2,0);
    init_matrix(size,size,m3,1);

    fprintf(f ,"%d ",size);
    for(int i=0; i <3 ; i++){
      init_matrix(size,size,m3,1);
      perf_t p1, p2;
      perf(&p1);
      fun[i]( size, size, size, m1, lda, m2, ldb, m3, ldc);  
      perf(&p2);
    
      perf_diff(&p1,&p2);
      double mflops = perf_mflops(&p2,2*size*size*size);
      fprintf(f, "%f ", mflops);
      
    }
    size*=1.15;
    fprintf(f, "\n");
    free(m1);
    free(m2);
    free(m3);
  }
}


void timing_cblas_dgemm_block_vs_scalaire(){
  FILE * f = fopen("curves/data/dgemm_block_vs_scalaire_mflops.dat","w+");
  int size = 100;
  void (*fun[2])() = {cblas_dgemm_scalaire,
		      cblas_dgemm};

  
  while(size < MAX_MATRIX_SIZE){
    int lda, ldb, ldc;
    lda=ldb=ldc=size;
    double *m1 = alloc_matrix(size,size); 
    double *m2 = alloc_matrix(size,size);
    double *m3 = alloc_matrix(size,size);
    init_matrix(size,size,m1,0);
    init_matrix(size,size,m2,0);
    init_matrix(size,size,m3,1);

    fprintf(f ,"%d ",size);
    for(int i=0; i <2 ; i++){
      init_matrix(size,size,m3,1);
      perf_t p1, p2;
      perf(&p1);
      fun[i](CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 0.0, m1, lda, m2, ldb, 0.0, m3, ldc);  
      perf(&p2);
    
      perf_diff(&p1,&p2);
      double mflops = perf_mflops(&p2,2*size*size*size);
      fprintf(f, "%f ", mflops);
      
    }
    size*=1.25;
    fprintf(f, "\n");
    free(m1);
    free(m2);
    free(m3);
  }
}

/*******************************************************************/

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
  timing_function_t tests[NB_TESTS] = {init_test(timing_cblas_ddot,"DDOT TIMING (curve: curves/ddot_mflops.gp)") ,
  				       init_test(timing_compare_cblas_dgemm_scalaire,"DGEMM SCALAIRE COMPARAISON (curve: curves/dgemm_scalaire_compare.gp)"),
  				       init_test(timing_cblas_dgemm_block_vs_scalaire, "DGEMM BLOCK vs SCALAIRE (curve: curves/dgemm_scalaire_vs_block.gp)")};


  for(int i=0; i<NB_TESTS; i++){
    printf("%s\n",tests[i].msg);
    tests[i].fun();
  }
    
  return 0;
}
