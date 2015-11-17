#include "mycblas.h"

void mycblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
		  const double alpha, const double *X, const int incX,
		  const double *Y, const int incY, double *A, const int lda){

    for(int j = 0; j < N; j++)
      for(int i = 0; i < M; i++){
	//printf("%.20f = %.20f - %.20f * %.20f = %.20f - %.20f = ", A[ j*lda + i], A[ j*lda + i], X[i*incX], Y[j*incY], A[ j*lda + i], (X[i*incX] * Y[j*incY]));
	A[ j*lda + i] += (X[i*incX] * Y[j*incY] * alpha);
	//printf("%.20f\n", A[j*lda+i]);
      }
}
