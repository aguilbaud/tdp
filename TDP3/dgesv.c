#include "mycblas.h"

void mycblas_dgesv(const int N, const int NHRS, double *A, const int lda,
		   int *ipiv, double *B, const int ldb, int *info){
    
    //A=LU
    mycblas_dgetrf(N, N, A, lda, ipiv, info);
    //Ly=b => b=y
    mycblas_dtrsm(CblasRowMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit, ldb, NHRS, 1.0, 
		  A, lda, B, ldb);
    //Ux=b => b=x
    mycblas_dtrsm(CblasRowMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, ldb, NHRS, 1.0, 
		  A, lda, B, ldb);    
    
}
