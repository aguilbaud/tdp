#include "cblas.h"

void cblas_dscal(const int N, const double alpha, double *X, const int incX){
  for(int i=0; i < N; i+= incX)
    X[i] *= alpha;
}

void cblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda){
  
  for(int i = 0; i < M; i+=incX){
    for(int j = 0; j < N; j+=incY){
      A[ j*N + i] = X[i] * Y[j] * alpha;
    }
  }
}

void dgetf_2_nopiv(const int M, const int N, double * A, const int lda){
  for(int i = 1; i < M; i++){
    double *col = A + (i*N + (i+1));
    double *line = A + ((i+1)*N + i);
    double *ssmat = A + ((i+1)*N + (i+1));
    
    cblas_dscal(N-i, (1/A[i *N + i]), col, 1);
    cblas_dger(CblasRowMajor, M-i, N-i, -1, col, 1, line, lda, ssmat, N);
  }
}
