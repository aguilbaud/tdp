#include "mycblas.h"

#define BLOCK_SIZE 2
#define MIN(A,B) (((A)<(B)) ? (A) : (B))

void mycblas_dgetrf(const int M, const int N, double *A, const int lda, int *ipiv, int *info)
{
    for(int j=0; j<M; j+=BLOCK_SIZE){
	mycblas_dgetf2_nopiv( M-(BLOCK_SIZE*j), 
			      MIN(BLOCK_SIZE,N-(BLOCK_SIZE*j)), 
			      A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE), 
			      M);
	if(j<M-BLOCK_SIZE){
		   
	  mycblas_dtrsm(CblasColMajor, CblasLeft, CblasLower, CblasNoTrans, CblasUnit,
			BLOCK_SIZE, N-(BLOCK_SIZE * (j+1)), 1,
			A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE), M,
			A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE + M*BLOCK_SIZE), M);

	  mycblas_dgemm_scalaire(CblasColMajor, CblasNoTrans, CblasNoTrans,
				 M-((j+1)*BLOCK_SIZE), N-((j+1)*BLOCK_SIZE), BLOCK_SIZE,
				 -1, A + (M*j*BLOCK_SIZE + j*BLOCK_SIZE + BLOCK_SIZE), M,
				 A + (M*(j+1)*BLOCK_SIZE + j*BLOCK_SIZE), M, 1,
				 A + (M*(j+1)*BLOCK_SIZE + (j+1)*BLOCK_SIZE), M);
	}
    }
}
