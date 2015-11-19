#include "mycblas.h"

void mycblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
		  const double alpha, const double *X, const int incX,
		  const double *Y, const int incY, double *A, const int lda){

  //printf("-==== DGER ====-\nM=%d; N=%d; lda=%d\n", M, N, lda);
  for(int j = 0; j < N; j++)
    for(int i = 0; i < M; i++){
      //printf("A[%d] = %3.3f = %3.3f - %3.3f * %3.3f = %3.3f - %3.3f = ", j*lda + i, A[ j*lda + i], A[ j*lda + i], X[i*incX], Y[j*incY], A[ j*lda + i], (X[i*incX] * Y[j*incY]));
      A[ j*lda + i] += (X[i*incX] * Y[j*incY] * alpha);
      //printf("%3.3f\n", A[j*lda+i]);
    }
  
  //printf("-==== FIN DGER ====-\n");
}
