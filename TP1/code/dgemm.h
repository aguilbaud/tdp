#ifndef _DGEMM_H
#define _DGEMM_H
#include "cblas.h"

void cblas_dgemm_block(const int M, const int N, const int K,
		       const double *A, const int lda, 
		       const double *B, const int ldb,
		       double *C, const int ldc);

void cblas_dgemm_scalaire(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
			  const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
			  const int K, const double alpha, const double *A,
			  const int lda, const double *B, const int ldb,
			  const double beta, double *C, const int ldc);



void cblas_dgemm_scalaire_ijk(const int M, const int N, const int K,
			      const double *A, const int lda, 
			      const double *B, const int ldb,
			      double *C, const int ldc);

void cblas_dgemm_scalaire_kij(const int M, const int N, const int K,
			      const double *A, const int lda, 
			      const double *B, const int ldb,
			      double *C, const int ldc);

void cblas_dgemm_scalaire_jik(const int M, const int N, const int K,
			      const double *A, const int lda, 
			      const double *B, const int ldb,
			      double *C, const int ldc);
#endif
