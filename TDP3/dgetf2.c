#include "cblas.h"
#include "util.h"

void cblas_dscal(const int N, const double alpha, double *X, const int incX){
  for(int i=0; i < N; i++, X+=incX)
	(*X) *= alpha;
}

void cblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
		const double alpha, const double *X, const int incX,
		const double *Y, const int incY, double *A, const int lda){

    for(int j = 0; j < N; j++)
	for(int i = 0; i < M; i++)   
	    A[ j*lda + i] += (X[i*incX] * Y[j*incY] * alpha);
}

void dgetf_2_nopiv(const int M, const int N, double * A, const int lda){
    for(int i = 0; i < M; i++){
	double *col = A + (i*lda + (i+1));
	double *line = A + ((i+1)*lda + i);
	double *ssmat = A + ((i+1)*lda + (i+1));
	
	cblas_dscal(M-(i+1), (1/A[i *lda + i]), col, 1);
	cblas_dger(CblasRowMajor, M-(i+1), N-(i+1), -1, col, 1, line, lda, ssmat, N);
    }
}
