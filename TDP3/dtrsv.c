#include "cblas.h"

void cblas_dtrsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const double *A, const int lda, double *X,
                 const int incX){
    
    //Assuming Diag == CblasNonUnit
    if( Uplo == CblasUpper){
	for (int i = N-1; i >= 0; i--) {
	    double sum = 0.0;
	    for(int j = i+1; j < N; j++){
		sum += A[j*lda + i] * X[j];
	    }
	    X[i] = (X[i] - sum)/A[i*lda + i];
	}
    }
    else{//Assuming Diag == CblasUnit
	for(int i = 0; i<N; i++){
	    double sum = 0.0;
	    for (int j=0; j <= i-1; j++) {
		sum += A[j*lda + i] * X[j];
	    }
	    X[i] = X[i] - sum;
	}
    }
}
