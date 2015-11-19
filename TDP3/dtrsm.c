#include "mycblas.h"

void mycblas_dtrsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
		   const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
		   const enum CBLAS_DIAG Diag, const int M, const int N,
		   const double alpha, const double *A, const int lda,
		   double *B, const int ldb){
    if(Side == CblasLeft) {
	//A( LDA, k ) where k is m when SIDE = 'L' 
      for (int i = 0; i < N; i++) {
	mycblas_dtrsv(Order, Uplo, TransA, Diag, M, A, lda, B + (i*ldb + 0), 1);
	}
    }
}
