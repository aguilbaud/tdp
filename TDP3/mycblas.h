#ifndef MYCBLAS_H
#define MYCBLAS_H
#include "cblas.h"

void mycblas_dtrsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
		   const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
		   const int N, const double *A, const int lda, double *X,
		   const int incX);

void mycblas_dgemm_block(const int M, const int N, const int K,
			 const double *A, const int lda, 
			 const double *B, const int ldb,
			 double *C, const int ldc);

void mycblas_dgemm_scalaire(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
			    const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
			    const int K, const double alpha, const double *A,
			    const int lda, const double *B, const int ldb,
			    const double beta, double *C, const int ldc);

void mycblas_dgesv(const int N, const int NHRS, double *A, const int lda,
		   int *ipiv, double *B, const int ldb, int *info);

void mycblas_dscal(const int N, const double alpha, double *X, const int incX);

void mycblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
		  const double alpha, const double *X, const int incX,
		  const double *Y, const int incY, double *A, const int lda);
     
void mycblas_dgetf2_nopiv(const int M, const int N, double * A, const int lda);

void mycblas_dgetrf(const int M, const int N, double *A, const int lda, int *ipiv, int *info);

void mycblas_dtrsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
		   const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
		   const enum CBLAS_DIAG Diag, const int M, const int N,
		   const double alpha, const double *A, const int lda,
		   double *B, const int ldb);

#endif /* MYCBLAS_H */
