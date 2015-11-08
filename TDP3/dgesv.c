#include "dgesv.h"

void cblas_dgesv(const int N, const int NHRS, double *A, const int lda,
		 double *ipiv, double *B, const int ldb, int *info){
    
    //A=LU
    dgerf(N, N, A, lda, ipiv, info);
    //Ly=b => b=y
    dtrsm(CblasRowMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, ldb, NHRS, 1.0, 
	  A, lda, B, ldb);
    //Ux=b => b=x
    dtrsm(CblasRowMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, ldb, NHRS, 1.0, 
	  A, lda, B, ldb);    
    
}
