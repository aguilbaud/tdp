#include "mycblas.h"
#include <stdio.h>

#define BLOCK_SIZE 4
#define MIN(A,B) (((A)<(B)) ? (A) : (B))

void mycblas_dgetrf(const int M, const int N, double *A, const int lda, int *ipiv, int *info){
  if(BLOCK_SIZE>N){
    mycblas_dgetf2_nopiv(M, N, A, M);
    return;
  }
  double test;
  test=(double)M/(double)BLOCK_SIZE;
  for(int j=0; j<test; j++){
    /* printf("appel de dgetf2_nopiv(%d,%d,A[%d],%d)\n", M,  */
    /* 	   MIN(BLOCK_SIZE,N-(BLOCK_SIZE*j)),  */
    /* 	   (M*j*BLOCK_SIZE + j*BLOCK_SIZE),  */
    /* 	   M); */
    mycblas_dgetf2_nopiv( M-(BLOCK_SIZE*j), 
			  MIN(BLOCK_SIZE,N-(BLOCK_SIZE*j)), 
			  A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE), 
			  M);
    /* affiche(M,N,A,lda,stdout); */
    /* printf("=====================================\n"); */
    if(j<test-1){
      /* printf("appel de dtrsm(%d, %d, %d, A[%d], %d, A[%d], %d)\n",  */
      /* 	     BLOCK_SIZE, N-(BLOCK_SIZE * (j+1)), 1, */
      /* 	     (M*j*BLOCK_SIZE + j*BLOCK_SIZE), M, */
      /* 	     (M*j*BLOCK_SIZE + j*BLOCK_SIZE + M*BLOCK_SIZE), M); */
      mycblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit,
		    BLOCK_SIZE, N-(BLOCK_SIZE * (j+1)), 1,
		    A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE), M,
		    A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE + M*BLOCK_SIZE), M);
      /* affiche(M,N,A,lda,stdout); */
      /* printf("=====================================\n"); */
      /* printf("appel de dgemm_scalaire(%d, %d, %d, %d, A[%d], %d, A[%d], %d, %d, A[%d], %d)\n", M-((j+1)*BLOCK_SIZE), N-((j+1)*BLOCK_SIZE), BLOCK_SIZE, */
      /* 	     -1, (M*j*BLOCK_SIZE + j*BLOCK_SIZE + BLOCK_SIZE), M, */
      /* 	     (M*(j+1)*BLOCK_SIZE + j*BLOCK_SIZE), M, 1, */
      /* 	     (M*(j+1)*BLOCK_SIZE + (j+1)*BLOCK_SIZE), M); */
      mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans,
			     M-((j+1)*BLOCK_SIZE), N-((j+1)*BLOCK_SIZE), BLOCK_SIZE,
			     -1, A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE + BLOCK_SIZE), M,
			     A + (M*(j+1)*BLOCK_SIZE + j*BLOCK_SIZE), M, 1,
			     A + (M*(j+1)*BLOCK_SIZE + (j+1)*BLOCK_SIZE), M);
      /* affiche(M,N,A,lda,stdout); */
      /* printf("=====================================\n"); */
    }
  }
}
